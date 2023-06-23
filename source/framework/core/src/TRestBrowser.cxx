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

#include <TTreeFormula.h>

using namespace std;

TRestBrowser::TRestBrowser() {
    if ((TDirectory*)gDirectory != nullptr && gDirectory->GetFile() != nullptr) {
        Initialize();
        SetViewer("TRestEventViewer");
        OpenFile(gDirectory->GetFile()->GetName());
        cout << "Loaded File : " << fInputFileName << endl;
    } else {
        fBrowser = new TBrowser("Browser", nullptr, "REST Browser");
        fRestRun = new TRestRun();
    }
}

TRestBrowser::TRestBrowser(const TString& viewerName, Double_t geomScale) {
    Initialize("I");
    SetViewer(viewerName, geomScale);
}

TRestBrowser::~TRestBrowser() {
    if (frmMain != nullptr) frmMain->Cleanup();
    // delete frmMain;
}

void TRestBrowser::Initialize(const TString& opt) {
    pureAnalysis = kFALSE;

    fRestRun = new TRestRun();

    fBrowser = new TBrowser("Browser", 0, "REST Browser", opt);
    TGMainFrame* fr = fBrowser->GetBrowserImp()->GetMainFrame();
    if (fr == nullptr) {
        RESTWarning << "No x11 interface is available. Cannot call the browser window!" << RESTendl;
        exit(1);
    }
    fr->DontCallClose();

    fBrowser->StartEmbedding(0, -1);
    frmMain = new TGMainFrame(gClient->GetRoot(), 300);
    frmMain->SetCleanup(kDeepCleanup);
    frmMain->SetWindowName("Controller1");
    SetLeftPanelButtons();
    fBrowser->StopEmbedding();

    fBrowser->StartEmbedding(1, -1);
    fCanDefault = new TCanvas();
    fBrowser->StopEmbedding();

    fBrowser->StartEmbedding(2, -1);
    frmBot = new TGMainFrame(gClient->GetRoot(), 300);
    frmBot->SetCleanup(kDeepCleanup);
    frmBot->SetWindowName("Controller2");
    SetBottomPanelButtons();
    fBrowser->StopEmbedding();

    //// frmMain->DontCallClose();
    // frmMain->MapSubwindows();
    //// frmMain->Resize();
    // frmMain->Layout();
    // frmMain->MapWindow();
}

void TRestBrowser::SetViewer(TRestEventViewer* eV, Double_t geomScale) {
    if (fEventViewer != nullptr) {
        cout << "Event viewer has already been set!" << endl;
        return;
    }
    if (eV != nullptr) {
        fEventViewer = eV;
        fEventViewer->SetGeomScale(geomScale);
        // b->StartEmbedding(1, -1);
        eV->Embed(fBrowser);
        // b->StopEmbedding();
    }
}

void TRestBrowser::SetViewer(const TString& viewerName, Double_t geomScale) {
    if (Count((string)viewerName, "Viewer") > 0) {
        TRestEventViewer* viewer = REST_Reflection::Assembly((string)viewerName);
        viewer->SetGeomScale(geomScale);
        if (viewer != nullptr) {
            SetViewer(viewer, geomScale);
        } else {
            RESTError << viewerName << " not recognized! Did you install the corresponding library?"
                      << RESTendl;
            RESTError << "Also check EVE feature is turned on in REST for 3d event viewing." << RESTendl;
            RESTWarning << "Using default event viewer" << RESTendl;
        }
    } else {
        cout << "illegal viewer : " << viewerName << endl;
        exit(0);
    }
}

void TRestBrowser::SetLeftPanelButtons() {
    fVFrame = new TGVerticalFrame(frmMain);
    fVFrame->Resize(300, 200);

    // row in the tree
    fEventRowLabel = new TGLabel(fVFrame, "Entry:");
    fVFrame->AddFrame(fEventRowLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fEventRowNumberBox = new TGNumberEntry(fVFrame, fEventRow);
    fEventRowNumberBox->Connect("ValueSet(Long_t)", "TRestBrowser", this, "RowValueChangedAction(Long_t)");
    fVFrame->AddFrame(fEventRowNumberBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // event id and sub event id
    auto labelBar = new TGHorizontalFrame(fVFrame);
    {
        fEventIdLabel = new TGLabel(labelBar, "Event ID:");
        labelBar->AddFrame(fEventIdLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fEventSubIdLabel = new TGLabel(labelBar, "Sub ID:");
        labelBar->AddFrame(fEventSubIdLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(labelBar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto numberBoxBar = new TGHorizontalFrame(fVFrame);
    {
        fEventIdNumberBox = new TGNumberEntry(numberBoxBar, fEventId);
        fEventIdNumberBox->Connect("ValueSet(Long_t)", "TRestBrowser", this, "IdValueChangedAction(Long_t)");
        numberBoxBar->AddFrame(fEventIdNumberBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fEventSubIdNumberBox = new TGNumberEntry(numberBoxBar, fEventSubId);
        fEventSubIdNumberBox->Connect("ValueSet(Long_t)", "TRestBrowser", this,
                                      "IdValueChangedAction(Long_t)");
        numberBoxBar->AddFrame(fEventSubIdNumberBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(numberBoxBar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

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

    auto switchButtonBar = new TGHorizontalFrame(fVFrame);
    {
        fButOptPrev = new TGPictureButton(switchButtonBar, gClient->GetPicture("bld_undo.png"));
        fButOptPrev->Connect("Clicked()", "TRestBrowser", this, "PreviousPlotOptionAction()");
        switchButtonBar->AddFrame(fButOptPrev, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButOptRefresh = new TGTextButton(switchButtonBar, "Plot");
        fButOptRefresh->Connect("Clicked()", "TRestBrowser", this, "PlotAction()");
        switchButtonBar->AddFrame(fButOptRefresh, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButOptNext = new TGPictureButton(switchButtonBar, gClient->GetPicture("bld_redo.png"));

        fButOptNext->Connect("Clicked()", "TRestBrowser", this, "NextPlotOptionAction()");
        switchButtonBar->AddFrame(fButOptNext, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(switchButtonBar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    // tool buttons
    fMenuOpen = new TGPictureButton(fVFrame, gClient->GetPicture("bld_open.png"));
    fMenuOpen->Connect("Clicked()", "TRestBrowser", this, "LoadFileAction()");
    fVFrame->AddFrame(fMenuOpen, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fExit = new TGPictureButton(fVFrame, gClient->GetPicture("bld_exit.png"));

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

void TRestBrowser::SetBottomPanelButtons() {
    fHFrame = new TGVerticalFrame(frmBot);
    fHFrame->Resize(300, 100);

    fSelectionTextBoxLabel = new TGLabel(fHFrame, "Selection:");
    fHFrame->AddFrame(fSelectionTextBoxLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fSelectionTextBox = new TGTextEntry(fHFrame, "");
    fHFrame->AddFrame(fSelectionTextBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto bottomBar = new TGHorizontalFrame(fHFrame);
    {
        fButEveNext = new TGTextButton(bottomBar, "Previous Event");  ///< Exit button
        fButEveNext->Connect("Clicked()", "TRestBrowser", this, "PreviousEventAction()");
        bottomBar->AddFrame(fButEveNext, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButEvePrev = new TGTextButton(bottomBar, "Next Event");  ///< Exit button
        fButEvePrev->Connect("Clicked()", "TRestBrowser", this, "NextEventAction()");
        bottomBar->AddFrame(fButEvePrev, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fHFrame->AddFrame(bottomBar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    frmBot->AddFrame(fHFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    frmBot->MapSubwindows();
    frmBot->Layout();
    frmBot->MapWindow();
}

void TRestBrowser::InitFromConfigFile() { cout << __PRETTY_FUNCTION__ << endl; }

void TRestBrowser::SetInputEvent(TRestEvent* event) {
    if (fRestRun != nullptr) {
        fRestRun->SetInputEvent(event);
    }
}

Bool_t TRestBrowser::LoadEventEntry(Int_t n) {
    if (fRestRun->GetInputFile() == nullptr || fRestRun->GetInputFile()->IsZombie()) {
        RESTWarning << "TRestBrowser::LoadEventEntry. No File..." << RESTendl;
        return kFALSE;
    }
    if (pureAnalysis) {
        RESTWarning << "TRestBrowser::LoadEventEntry. This is a pure analysis file..." << RESTendl;
        return kFALSE;
    }

    if (fRestRun->GetAnalysisTree() != nullptr && n < fRestRun->GetAnalysisTree()->GetEntries() && n >= 0) {
        fRestRun->GetEntry(n);
        TRestEvent* ev = fRestRun->GetInputEvent();
        if (!ev) {
            RESTError << "internal error!" << RESTendl;
            return kFALSE;
        } else {
            fEventRow = fRestRun->GetCurrentEntry();
            fEventId = ev->GetID();
            fEventSubId = ev->GetSubID();

            fEventRowNumberBox->SetIntNumber(fEventRow);
            fEventIdNumberBox->SetIntNumber(fEventId);
            fEventSubIdNumberBox->SetIntNumber(fEventSubId);
            fRestRun->GetAnalysisTree()->PrintObservables();
        }
    } else {
        RESTWarning << "TRestBrowser::LoadEventEntry. Event out of limits" << RESTendl;
        return kFALSE;
    }

    if (fEventViewer != nullptr) {
        fEventViewer->AddEvent(fRestRun->GetInputEvent());
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
        cout << endl;
    }

    fCanDefault->cd();
    return kTRUE;
}

Bool_t TRestBrowser::LoadEventId(Int_t eventID, Int_t subEventID) {
    if (fRestRun->GetInputFile() == nullptr || fRestRun->GetInputFile()->IsZombie()) {
        RESTWarning << "TRestBrowser::LoadEventEntry. No File..." << RESTendl;
        return kFALSE;
    }
    if (pureAnalysis) {
        cout << "" << endl;
        RESTWarning << "TRestBrowser::LoadEventEntry. This is a pure analysis file..." << RESTendl;
        return kFALSE;
    }

    if (fRestRun->GetAnalysisTree() != nullptr && fRestRun->GetAnalysisTree()->GetEntries() > 0) {
        TRestEvent* event = fRestRun->GetEventWithID(eventID, subEventID);
        if (event != nullptr) {
            RESTWarning << "Event ID : " << eventID << " with sub ID : " << subEventID << " not found!"
                        << RESTendl;
            return kFALSE;
        } else {
            fEventRow = fRestRun->GetCurrentEntry();
            fEventId = event->GetID();
            fEventSubId = event->GetSubID();

            fEventRowNumberBox->SetIntNumber(fEventRow);
            fEventIdNumberBox->SetIntNumber(fEventId);
            fEventSubIdNumberBox->SetIntNumber(fEventSubId);
            fRestRun->GetAnalysisTree()->PrintObservables();
        }
    } else {
        RESTWarning << "TRestBrowser::LoadEventEntry. Event out of limits" << RESTendl;
        return kFALSE;
    }

    if (fEventViewer != nullptr) {
        fEventViewer->AddEvent(fRestRun->GetInputEvent());
        fEventViewer->Plot(fPlotOptionTextBox->GetText());
        cout << endl;
    }

    fCanDefault->cd();
    return kTRUE;
}

Bool_t TRestBrowser::OpenFile(const TString& filename) {
    if (filename.Contains("http") || TRestTools::fileExists(filename.Data())) {
        fInputFileName = filename;

        fRestRun->OpenInputFile(fInputFileName);
        fRestRun->GetInputFile()->cd();
        TTree* t = fRestRun->GetEventTree();

        TGeoManager* geometry = gGeoManager;

        if (t != nullptr) {
            // add entry for other event types
            TObjArray* branches = t->GetListOfBranches();
            for (int i = 0; i <= branches->GetLast(); i++) {
                auto branch = (TBranch*)branches->At(i);
                if (((string)branch->GetName()).find("EventBranch") != string::npos) {
                    string eventType = Replace((string)branch->GetName(), "Branch", "");
                    fEventTypeComboBox->AddEntry(eventType.c_str(), fEventTypeComboBox->GetNumberOfEntries());
                    // we make the entry of input event being selected
                    if (fRestRun->GetInputEvent() != nullptr &&
                        (string)fRestRun->GetInputEvent()->ClassName() == eventType) {
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

        TRestEvent* ev = fRestRun->GetInputEvent();
        if (!ev) {
            RESTError << "internal error!" << RESTendl;
        } else {
            fEventRowNumberBox->SetIntNumber(fRestRun->GetCurrentEntry());
            fEventIdNumberBox->SetIntNumber(ev->GetID());
            fEventSubIdNumberBox->SetIntNumber(ev->GetSubID());
        }
        return true;
    } else {
        RESTError << "file: " << filename << " does not exist!" << RESTendl;
    }
    return false;
}

void TRestBrowser::NextPlotOptionAction() {
    string text = fPlotOptionTextBox->GetText();
    if (text.empty()) {
        text = "0";
    } else if (isANumber(text)) {
        text = ToString(StringToInteger(text) + 1);
    }

    fPlotOptionTextBox->SetText(text.c_str());
    PlotAction();
}

void TRestBrowser::PreviousPlotOptionAction() {
    string text = fPlotOptionTextBox->GetText();
    if (text.empty()) {
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
    int eventRow = fEventRow;
    fEventRow = (Int_t)fEventRowNumberBox->GetNumber();

    RESTDebug << "TRestBrowser::LoadEventAction. Entry:" << fEventRow << RESTendl;

    bool success = LoadEventEntry(fEventRow);

    if (!success) {
        fEventRow = eventRow;
        fEventRowNumberBox->SetIntNumber(fEventRow);
    }
}

void TRestBrowser::EventTypeChangedAction(Int_t id) {
    string eventType = fEventTypeComboBox->GetSelectedEntry()->GetTitle();
    TRestEvent* eve = REST_Reflection::Assembly(eventType);

    if (eve != nullptr) {
        fRestRun->SetInputEvent(eve);
        RowValueChangedAction(0);
    }
}

void TRestBrowser::IdValueChangedAction(Long_t val) {
    int eventID = fEventId;
    int subEventID = fEventSubId;

    fEventId = (Int_t)fEventIdNumberBox->GetNumber();
    fEventSubId = (Int_t)fEventSubIdNumberBox->GetNumber();

    RESTDebug << "TRestBrowser::LoadEventAction. Event ID: " << fEventId << ", Sub ID: " << fEventSubId
              << RESTendl;

    bool success = LoadEventId(fEventId, fEventSubId);

    if (!success) {
        fEventId = eventID;
        fEventSubId = subEventID;
        fEventIdNumberBox->SetIntNumber(fEventId);
        fEventSubIdNumberBox->SetIntNumber(fEventSubId);
    }
}

void TRestBrowser::NextEventAction() {
    string sel = (string)fSelectionTextBox->GetText();
    if (sel.empty()) {
        fEventRow++;
        LoadEventEntry(fEventRow);
    } else {
        TRestAnalysisTree* tree = fRestRun->GetAnalysisTree();
        TTreeFormula formula("Selection", sel.c_str(), tree);
        if (formula.GetNdim() > 0) {  // valid expression
            fEventRow++;
            while (true) {
                tree->GetEntry(fEventRow);
                if (formula.EvalInstance(fEventRow) == 1) {
                    LoadEventEntry(fEventRow);
                    break;
                } else {
                    cout << fEventRow << endl;
                    fEventRow++;
                }
                if (fEventRow >= fRestRun->GetAnalysisTree()->GetEntries()) {
                    LoadEventEntry(fEventRow);
                    break;
                }
            }
        } else {
            cout << "invalid selection!" << endl;
        }
    }
}

void TRestBrowser::PreviousEventAction() {
    string sel = (string)fSelectionTextBox->GetText();
    if (sel.empty()) {
        fEventRow--;
        LoadEventEntry(fEventRow);
    } else {
        TRestAnalysisTree* tree = fRestRun->GetAnalysisTree();
        TTreeFormula formula("Selection", sel.c_str(), tree);
        if (formula.GetNdim() > 0) {  // valid expression
            fEventRow--;
            while (true) {
                tree->GetEntry(fEventRow);
                if (formula.EvalInstance(fEventRow) == 1) {
                    LoadEventEntry(fEventRow);
                    break;
                } else {
                    cout << fEventRow << endl;
                    fEventRow--;
                }
                if (fEventRow < 0) {
                    LoadEventEntry(fEventRow);
                    break;
                }
            }
        } else {
            cout << "invalid selection!" << endl;
        }
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
