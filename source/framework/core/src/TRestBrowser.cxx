//////////////////////////////////////////////////////////////////////////
///
///
/// This class opens input file with TRestRun and shows the plot of each event
/// The plot is shown through TRestEventViewer interface on the right. On the left
/// there is a control bar to switch the events. Plot options can also be given.
///
/// \class TRestBrowser
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///            code (REST v2)
///            Igor G. Irastorza
///
/// 2017-Aug:  Major change: added for multi-thread capability
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestBrowser.h"

using namespace std;
//______________________________________________________________________________
TRestBrowser::TRestBrowser() {
    if ((TDirectory*)gDirectory != nullptr && gDirectory->GetFile() != nullptr) {
        Initialize();
        SetViewer("TRestEventViewer");
        OpenFile(gDirectory->GetFile()->GetName());
        cout << "Loaded File : " << fInputFileName << endl;
    } else {
        b = new TBrowser("Browser", 0, "REST Browser");
        r = new TRestRun();
    }
}

TRestBrowser::TRestBrowser(TString viewerName) {
    Initialize("I");
    SetViewer(viewerName);
}

//______________________________________________________________________________
TRestBrowser::~TRestBrowser() {
    if (frmMain != nullptr) frmMain->Cleanup();
    // delete frmMain;
}

void TRestBrowser::Initialize(TString opt) {
    pureAnalysis = kFALSE;

    r = new TRestRun();

    b = new TBrowser("Browser", 0, "REST Browser", opt);
    TGMainFrame* fr = b->GetBrowserImp()->GetMainFrame();
    if (fr == nullptr) {
        warning << "No x11 interface is available. Cannot call the browser window!" << endl;
        exit(1);
    }
    fr->DontCallClose();

    b->StartEmbedding(0, -1);
    frmMain = new TGMainFrame(gClient->GetRoot(), 300);
    frmMain->SetCleanup(kDeepCleanup);
    frmMain->SetWindowName("Controller");
    SetButtons();
    b->StopEmbedding();

    b->StartEmbedding(1, -1);
    fCanDefault = new TCanvas();
    b->StopEmbedding();
    // frmMain->DontCallClose();
    frmMain->MapSubwindows();
    // frmMain->Resize();
    frmMain->Layout();
    frmMain->MapWindow();
}

void TRestBrowser::SetViewer(TRestEventViewer* eV) {
    if (fEventViewer != nullptr) {
        cout << "Event viewer has already been set!" << endl;
        return;
    }
    if (eV != nullptr) {
        fEventViewer = eV;
        // b->StartEmbedding(1, -1);
        eV->Embed(b);
        // b->StopEmbedding();
    }
}

void TRestBrowser::SetViewer(TString viewerName) {
    if (Count((string)viewerName, "Viewer") > 0) {
        TRestEventViewer* viewer = REST_Reflection::Assembly((string)viewerName);
        if (viewer != nullptr) {
            SetViewer(viewer);
        } else {
            ferr << viewerName << " not recoginzed! Did you install the corresponding library?" << endl;
            ferr << "Also check EVE feature is turned on in REST for 3d event viewing." << endl;
            warning << "Using default event viewer" << endl;
        }
    } else {
        cout << "illegal viewer : " << viewerName << endl;
        exit(0);
    }
}

void TRestBrowser::SetButtons() {
    TString icondir = (TString)getenv("ROOTSYS") + "/icons/";

    auto fVFrame = new TGVerticalFrame(frmMain);
    fVFrame->Resize(300, 200);

    // row in the tree
    fEventRowLabel = new TGLabel(fVFrame, "Entry:");
    fVFrame->AddFrame(fEventRowLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fEventRowNumberBox = new TGNumberEntry(fVFrame, fEventRow);
    fEventRowNumberBox->Connect("ValueSet(Long_t)", "TRestBrowser", this, "RowValueChangedAction(Long_t)");
    fVFrame->AddFrame(fEventRowNumberBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // event id and sub event id
    auto labelbar = new TGHorizontalFrame(fVFrame);
    {
        fEventIdLabel = new TGLabel(labelbar, "Event ID:");
        labelbar->AddFrame(fEventIdLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fEventSubIdLabel = new TGLabel(labelbar, "Sub ID:");
        labelbar->AddFrame(fEventSubIdLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(labelbar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto numberboxbar = new TGHorizontalFrame(fVFrame);
    {
        fEventIdNumberBox = new TGNumberEntry(numberboxbar, fEventId);
        fEventIdNumberBox->Connect("ValueSet(Long_t)", "TRestBrowser", this, "IdValueChangedAction(Long_t)");
        numberboxbar->AddFrame(fEventIdNumberBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fEventSubIdNumberBox = new TGNumberEntry(numberboxbar, fEventSubId);
        fEventSubIdNumberBox->Connect("ValueSet(Long_t)", "TRestBrowser", this,
                                      "IdValueChangedAction(Long_t)");
        numberboxbar->AddFrame(fEventSubIdNumberBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(numberboxbar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // event type to choose
    fEventTypeLabel = new TGLabel(fVFrame, "Event Type:");
    fVFrame->AddFrame(fEventTypeLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fEventTypeComboBox = new TGComboBox(fVFrame);
    fEventTypeComboBox->Connect("Selected(Int_t)", "TRestBrowser", this, "EventTypeChangedAction(Int_t)");
    fVFrame->AddFrame(fEventTypeComboBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // plot option buttons
    fPlotOptionLabel = new TGLabel(fVFrame, "Plot Options:");
    fVFrame->AddFrame(fPlotOptionLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fPlotOptionTextBox = new TGTextEntry(fVFrame, "");
    fPlotOptionTextBox->SetText("");
    fPlotOptionTextBox->Connect("ReturnPressed()", "TRestBrowser", this, "PlotAction()");
    fVFrame->AddFrame(fPlotOptionTextBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto switchbuttonbar = new TGHorizontalFrame(fVFrame);
    {
        fButOptPrev = new TGTextButton(switchbuttonbar, "<<Previous");  ///< Load Event button
        fButOptPrev->Connect("Clicked()", "TRestBrowser", this, "PreviousPlotOptionAction()");
        switchbuttonbar->AddFrame(fButOptPrev, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButOptRefresh = new TGTextButton(switchbuttonbar, "Plot");  ///< Load Event button
        fButOptRefresh->Connect("Clicked()", "TRestBrowser", this, "PlotAction()");
        switchbuttonbar->AddFrame(fButOptRefresh, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButOptNext = new TGTextButton(switchbuttonbar, "Next>>");  ///< Load Event button
        fButOptNext->Connect("Clicked()", "TRestBrowser", this, "NextPlotOptionAction()");
        switchbuttonbar->AddFrame(fButOptNext, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(switchbuttonbar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // tool buttons
    fMenuOpen = new TGPictureButton(fVFrame, gClient->GetPicture(icondir + "bld_open.png"));
    fMenuOpen->Connect("Clicked()", "TRestBrowser", this, "LoadFileAction()");
    fVFrame->AddFrame(fMenuOpen, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fExit = new TGTextButton(fVFrame, "EXIT");  ///< Exit button
    fExit->Connect("Clicked()", "TRestBrowser", this, "ExitAction()");
    fVFrame->AddFrame(fExit, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    frmMain->Resize(TGDimension(300, frmMain->GetHeight() + fVFrame->GetHeight()));

    frmMain->AddFrame(fVFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    // frmMain->DontCallClose();
    frmMain->MapSubwindows();
    // frmMain->Resize();
    frmMain->Layout();
    frmMain->MapWindow();
}

void TRestBrowser::InitFromConfigFile() { cout << __PRETTY_FUNCTION__ << endl; }

void TRestBrowser::SetInputEvent(TRestEvent* eve) {
    if (r != nullptr) {
        r->SetInputEvent(eve);
    }
}

Bool_t TRestBrowser::LoadEventEntry(Int_t n) {
    if (r->GetInputFile() == nullptr || r->GetInputFile()->IsZombie()) {
        warning << "TRestBrowser::LoadEventEntry. No File..." << endl;
        return kFALSE;
    }
    if (pureAnalysis) {
        warning << "TRestBrowser::LoadEventEntry. This is a pure analysis file..." << endl;
        return kFALSE;
    }

    if (r->GetAnalysisTree() != nullptr && n < r->GetAnalysisTree()->GetEntries() && n >= 0) {
        r->GetEntry(n);
        TRestEvent* ev = r->GetInputEvent();
        if (!ev) {
            ferr << "internal error!" << endl;
            return kFALSE;
        } else {
            fEventRow = r->GetCurrentEntry();
            fEventId = ev->GetID();
            fEventSubId = ev->GetSubID();

            fEventRowNumberBox->SetIntNumber(fEventRow);
            fEventIdNumberBox->SetIntNumber(fEventId);
            fEventSubIdNumberBox->SetIntNumber(fEventSubId);
            r->GetAnalysisTree()->PrintObservables();
        }
    } else {
        warning << "TRestBrowser::LoadEventEntry. Event out of limits" << endl;
        return kFALSE;
    }

    if (fEventViewer != nullptr) {
        fEventViewer->AddEvent(r->GetInputEvent());
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
        cout << endl;
    }

    fCanDefault->cd();
    return kTRUE;
}

Bool_t TRestBrowser::LoadEventId(Int_t id, Int_t subid) {
    if (r->GetInputFile() == nullptr || r->GetInputFile()->IsZombie()) {
        warning << "TRestBrowser::LoadEventEntry. No File..." << endl;
        return kFALSE;
    }
    if (pureAnalysis) {
        cout << "" << endl;
        warning << "TRestBrowser::LoadEventEntry. This is a pure analysis file..." << endl;
        return kFALSE;
    }

    if (r->GetAnalysisTree() != nullptr && r->GetAnalysisTree()->GetEntries() > 0) {
        TRestEvent* ev = r->GetEventWithID(id, subid);
        if (!ev) {
            warning << "Event ID : " << id << " with sub ID : " << subid << " not found!" << endl;
            return kFALSE;
        } else {
            fEventRow = r->GetCurrentEntry();
            fEventId = ev->GetID();
            fEventSubId = ev->GetSubID();

            fEventRowNumberBox->SetIntNumber(fEventRow);
            fEventIdNumberBox->SetIntNumber(fEventId);
            fEventSubIdNumberBox->SetIntNumber(fEventSubId);
            r->GetAnalysisTree()->PrintObservables();
        }
    } else {
        warning << "TRestBrowser::LoadEventEntry. Event out of limits" << endl;
        return kFALSE;
    }

    if (fEventViewer != nullptr) {
        fEventViewer->AddEvent(r->GetInputEvent());
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
        cout << endl;
    }

    fCanDefault->cd();
    return kTRUE;
}

Bool_t TRestBrowser::OpenFile(TString filename) {
    if (filename.Contains("http") || TRestTools::fileExists(filename.Data())) {
        fInputFileName = filename;

        r->OpenInputFile(fInputFileName);
        TFile* f = r->GetInputFile();
        TTree* t = r->GetEventTree();

        TGeoManager* geometry = gGeoManager;

        if (t != nullptr) {
            // add entry for other event types
            TObjArray* branches = t->GetListOfBranches();
            for (int i = 0; i <= branches->GetLast(); i++) {
                TBranch* br = (TBranch*)branches->At(i);
                if (((string)br->GetName()).find("EventBranch") != -1) {
                    string eventtype = Replace((string)br->GetName(), "Branch", "");
                    fEventTypeComboBox->AddEntry(eventtype.c_str(), fEventTypeComboBox->GetNumberOfEntries());
                    // we make the entry of input event being selected
                    if (r->GetInputEvent() != nullptr &&
                        (string)r->GetInputEvent()->ClassName() == eventtype) {
                        fEventTypeComboBox->Select(fEventTypeComboBox->GetNumberOfEntries() - 1, false);
                    }
                }
            }

            // init viewer
            pureAnalysis = kFALSE;
            if (fEventViewer == nullptr) SetViewer("TRestEventViewer");
            if (geometry != nullptr && fEventViewer != nullptr) fEventViewer->SetGeometry(geometry);
            RowValueChangedAction(0);
        } else {
            pureAnalysis = kTRUE;
        }

        TRestEvent* ev = r->GetInputEvent();
        if (!ev) {
            ferr << "internal error!" << endl;
        } else {
            fEventRowNumberBox->SetIntNumber(r->GetCurrentEntry());
            fEventIdNumberBox->SetIntNumber(ev->GetID());
            fEventSubIdNumberBox->SetIntNumber(ev->GetSubID());
        }
        return true;
    } else {
        ferr << "file: " << filename << " does not exist!" << endl;
    }
    return false;
}

void TRestBrowser::NextPlotOptionAction() {
    string text = fPlotOptionTextBox->GetText();
    if (text == "") {
        text = "0";
    } else if (isANumber(text)) {
        text = ToString(StringToInteger(text) + 1);
    }

    fPlotOptionTextBox->SetText(text.c_str());
    PlotAction();
}

void TRestBrowser::PreviousPlotOptionAction() {
    string text = fPlotOptionTextBox->GetText();
    if (text == "") {
        text = "0";
    } else if (isANumber(text)) {
        text = ToString(StringToInteger(text) - 1);
    }

    fPlotOptionTextBox->SetText(text.c_str());
    PlotAction();
}

void TRestBrowser::PlotAction() {
    if (fEventViewer != nullptr) {
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
    }
}

void TRestBrowser::RowValueChangedAction(Long_t val) {
    int rowold = fEventRow;
    fEventRow = (Int_t)fEventRowNumberBox->GetNumber();

    debug << "TRestBrowser::LoadEventAction. Entry:" << fEventRow << endl;

    bool success = LoadEventEntry(fEventRow);

    if (!success) {
        fEventRow = rowold;
        fEventRowNumberBox->SetIntNumber(fEventRow);
    }
}

void TRestBrowser::EventTypeChangedAction(Int_t id) {
    string eventtype = fEventTypeComboBox->GetSelectedEntry()->GetTitle();
    TRestEvent* eve = REST_Reflection::Assembly(eventtype);

    if (eve != nullptr) {
        r->SetInputEvent(eve);
        RowValueChangedAction(0);
    }
}

void TRestBrowser::IdValueChangedAction(Long_t val) {
    int idold = fEventId;
    int subidold = fEventSubId;

    fEventId = (Int_t)fEventIdNumberBox->GetNumber();
    fEventSubId = (Int_t)fEventSubIdNumberBox->GetNumber();

    debug << "TRestBrowser::LoadEventAction. Event ID: " << fEventId << ", Sub ID: " << fEventSubId << endl;

    bool success = LoadEventId(fEventId, fEventSubId);

    if (!success) {
        fEventId = idold;
        fEventSubId = subidold;
        fEventIdNumberBox->SetIntNumber(fEventId);
        fEventSubIdNumberBox->SetIntNumber(fEventSubId);
    }
}

void TRestBrowser::LoadFileAction() {
    TGFileInfo fi;
    new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &fi);

    TString dir = fi.fFilename;

    cout << "Opening " << dir.Data() << endl;

    OpenFile(dir);
}

void TRestBrowser::ExitAction() { gSystem->Exit(0); }
