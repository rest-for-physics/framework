//////////////////////////////////////////////////////////////////////////
///
///
/// This class is inherted from TRestRun. It opens input file as TRestRun
/// defined, and shows a plot of the event contained in the file. The plot
/// is shown in a TBrowser embeded window, providing a customizable controller
/// on the side.
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
    if (gDirectory != NULL && gDirectory->GetFile() != NULL) {
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
    if (frmMain != NULL) frmMain->Cleanup();
    // delete frmMain;
}

void TRestBrowser::Initialize(TString opt) {
    pureAnalysis = kFALSE;

    r = new TRestRun();

    b = new TBrowser("Browser", 0, "REST Browser", opt);
    b->GetBrowserImp()->GetMainFrame()->DontCallClose();

    b->StartEmbedding(0, -1);
    frmMain = new TGMainFrame(gClient->GetRoot(), 300);
    frmMain->SetCleanup(kDeepCleanup);
    frmMain->SetWindowName("Controller");
    setButtons();
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
    if (fEventViewer != NULL) {
        cout << "Event viewer has already been set!" << endl;
        return;
    }
    if (eV != NULL) {
        fEventViewer = eV;
        b->StartEmbedding(1, -1);
        eV->Initialize();
        b->StopEmbedding();
    }
}

void TRestBrowser::SetViewer(TString viewerName) {
    if (Count((string)viewerName, "Viewer") > 0) {
        TRestEventViewer* viewer = REST_Reflection::Assembly((string)viewerName);
        if (viewer != NULL) {
            SetViewer(viewer);
        }
    } else {
        cout << "illegal viewer : " << viewerName << endl;
        exit(0);
    }
}

void TRestBrowser::setButtons() {
    TString icondir = (TString)getenv("ROOTSYS") + "/icons/";

    auto fVFrame = new TGVerticalFrame(frmMain);
    fVFrame->Resize(300, 200);

    // row in the tree
    fEventRowLabel = new TGLabel(fVFrame, "Row:");
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

    // plot option buttons
    fPlotOptionLabel = new TGLabel(fVFrame, "Plot Options:");
    fVFrame->AddFrame(fPlotOptionLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fPlotOptionTextBox = new TGTextEntry(fVFrame, "");
    fPlotOptionTextBox->SetText("");
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
    if (r != NULL) {
        r->SetInputEvent(eve);
    }
}

Bool_t TRestBrowser::LoadEventEntry(Int_t n) {
    if (r->GetInputFile() == NULL || r->GetInputFile()->IsZombie()) {
        warning << "TRestBrowser::LoadEventEntry. No File..." << endl;
        return kFALSE;
    }
    if (pureAnalysis) {
        warning << "TRestBrowser::LoadEventEntry. This is a pure analysis file..." << endl;
        return kFALSE;
    }

    if (r->GetAnalysisTree() != NULL && n < r->GetAnalysisTree()->GetEntries() && n >= 0) {
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

    if (fEventViewer != NULL) {
        fEventViewer->AddEvent(r->GetInputEvent());
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
        cout << endl;
    }

    fCanDefault->cd();
    return kTRUE;
}

Bool_t TRestBrowser::LoadEventId(Int_t id, Int_t subid) {
    if (r->GetInputFile() == NULL || r->GetInputFile()->IsZombie()) {
        warning << "TRestBrowser::LoadEventEntry. No File..." << endl;
        return kFALSE;
    }
    if (pureAnalysis) {
        cout << "" << endl;
        warning << "TRestBrowser::LoadEventEntry. This is a pure analysis file..." << endl;
        return kFALSE;
    }

    if (r->GetAnalysisTree() != NULL && r->GetAnalysisTree()->GetEntries() > 0) {
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

    if (fEventViewer != NULL) {
        fEventViewer->AddEvent(r->GetInputEvent());
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
        cout << endl;
    }

    fCanDefault->cd();
    return kTRUE;
}

Bool_t TRestBrowser::OpenFile(TString filename) {
    if (TRestTools::fileExists(filename.Data())) {
        fInputFileName = filename;
        r->OpenInputFile(fInputFileName);
        TFile* f = r->GetInputFile();
        TTree* t = r->GetEventTree();

        TGeoManager* geometry = gGeoManager;

        if (t != NULL) {
            // init viewer
            pureAnalysis = kFALSE;
            if (fEventViewer == NULL) SetViewer("TRestEventViewer");
            if (geometry != NULL && fEventViewer != NULL) fEventViewer->SetGeometry(geometry);
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
    }
    return true;
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
    if (fEventViewer != NULL) {
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
