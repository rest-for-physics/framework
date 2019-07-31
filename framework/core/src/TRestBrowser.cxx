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
        SetViewer("TRestGenericEventViewer");
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
        eV->SetController(this);
        b->StartEmbedding(1, -1);
        eV->Initialize();
        b->StopEmbedding();
    }
}

void TRestBrowser::SetViewer(TString viewerName) {
    if (Count((string)viewerName, "Viewer") > 0) {
        TClass* cl = TClass::GetClass(viewerName);
        if (cl == NULL) {
            cout << "cannot find viewer: " << viewerName << " !" << endl;
        } else {
            TObject* obj = (TObject*)cl->New();

            SetViewer((TRestEventViewer*)obj);
        }
    } else {
        cout << "illegal viewer : " << viewerName << endl;
        exit(0);
    }
}

void TRestBrowser::setButtons() {
    TString icondir = (TString)getenv("ROOTSYS") + "/icons/";

    fVFrame = generateNewFrame();

    fLabel = new TGLabel(fVFrame, "EventId:");
    fVFrame->AddFrame(fLabel, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fNEvent = new TGNumberEntry(fVFrame, fCurrentEvent);
    fNEvent->SetIntNumber(fCurrentEvent);
    fVFrame->AddFrame(fNEvent, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    auto controlbar = new TGHorizontalFrame(fVFrame);
    {
        fButPrev = new TGPictureButton(controlbar, gClient->GetPicture(icondir + "GoBack.gif"));
        fButPrev->Connect("Clicked()", "TRestBrowser", this, "LoadPrevEventAction()");
        controlbar->AddFrame(fButPrev, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fLoadEvent =
            new TGPictureButton(controlbar,
                                gClient->GetPicture(icondir + "refresh.png"));  ///< Load Event button
        fLoadEvent->Connect("Clicked()", "TRestBrowser", this, "LoadEventAction()");
        controlbar->AddFrame(fLoadEvent, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

        fButNext = new TGPictureButton(controlbar, gClient->GetPicture(icondir + "GoForward.gif"));
        fButNext->Connect("Clicked()", "TRestBrowser", this, "LoadNextEventAction()");
        controlbar->AddFrame(fButNext, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fVFrame->AddFrame(controlbar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fMenuOpen = new TGPictureButton(fVFrame, gClient->GetPicture(icondir + "bld_open.png"));
    fMenuOpen->Connect("Clicked()", "TRestBrowser", this, "LoadFileAction()");
    fVFrame->AddFrame(fMenuOpen, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    fExit = new TGTextButton(fVFrame, "EXIT");  ///< Exit button
    fExit->Connect("Clicked()", "TRestBrowser", this, "ExitAction()");
    fVFrame->AddFrame(fExit, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

    addFrame(fVFrame);

    // frmMain->AddFrame(fVFrame,new TGLayoutHints(kLHintsExpandX |
    // kLHintsExpandY));
}

void TRestBrowser::addFrame(TGFrame* f) {
    frmMain->Resize(TGDimension(300, frmMain->GetHeight() + f->GetHeight()));

    frmMain->AddFrame(f, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    // frmMain->DontCallClose();
    frmMain->MapSubwindows();
    // frmMain->Resize();
    frmMain->Layout();
    frmMain->MapWindow();
}

TGVerticalFrame* TRestBrowser::generateNewFrame() {
    auto frame = new TGVerticalFrame(frmMain);
    frame->Resize(300, 200);
    return frame;
}

void TRestBrowser::LoadEventAction() {
    Int_t eventN = (Int_t)fNEvent->GetNumber();

    cout << "Loading event " << eventN << endl;

    if (LoadEvent(eventN)) fCurrentEvent = eventN;
}

void TRestBrowser::LoadNextEventAction() {
    Int_t nextEvent = fCurrentEvent + 1;

    cout << " Next event " << nextEvent << endl;

    if (LoadEvent(nextEvent)) {
        fCurrentEvent = nextEvent;
        fNEvent->SetIntNumber(fCurrentEvent);
    }
}

void TRestBrowser::LoadPrevEventAction() {
    Int_t prevEvent = fCurrentEvent - 1;

    cout << " Previous event " << prevEvent << endl;

    if (LoadEvent(prevEvent)) {
        fCurrentEvent = prevEvent;
        fNEvent->SetIntNumber(fCurrentEvent);
    }
}

void TRestBrowser::LoadFileAction() {
    TGFileInfo fi;
    new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &fi);

    TString dir = fi.fFilename;

    cout << "Opening " << dir.Data() << endl;

    if (TRestTools::fileExists(dir.Data())) {
        OpenFile(dir);
        fNEvent->SetIntNumber(fCurrentEvent);
    }
}

Bool_t TRestBrowser::OpenFile(TString fName) {
    if (!TRestTools::fileExists((string)fName)) {
        cout << "WARNING. Input file " << fName << " does not exist" << endl;
        return kFALSE;
    }

    fInputFileName = fName;
    r->OpenInputFile(fName);
    TFile* f = r->GetInputFile();
    TTree* t = r->GetEventTree();
    fCurrentEvent = r->GetCurrentEntry();

    TGeoManager* geometry = gGeoManager;

    if (t != NULL) {
        // init viewer
        pureAnalysis = kFALSE;
        if (fEventViewer == NULL) SetViewer("TRestGenericEventViewer");
        if (geometry != NULL && fEventViewer != NULL) fEventViewer->SetGeometry(geometry);
        LoadEventAction();
    } else {
        pureAnalysis = kTRUE;
    }

    return kTRUE;
}

void TRestBrowser::SetInputEvent(TRestEvent* eve) {
    if (r != NULL) {
        r->SetInputEvent(eve);
    }
}

int TRestBrowser::GetChar(string hint) {
    if (r != NULL) {
        r->GetChar(hint);
    }
    return 0;
}

Bool_t TRestBrowser::LoadEvent(Int_t n) {
    if (r->GetInputFile() == NULL || r->GetInputFile()->IsZombie()) {
        cout << "No File..." << endl;
        return kFALSE;
    }
    if (pureAnalysis) {
        cout << "This is a pure analysis file..." << endl;
        return kFALSE;
    }
    if (r->GetAnalysisTree() != NULL && n < r->GetAnalysisTree()->GetEntries() && n >= 0) {
        r->GetEntry(n);
    } else {
        cout << "Event out of limits" << endl;
        return kFALSE;
    }

    if (fEventViewer != NULL) {
        fEventViewer->AddEvent(r->GetInputEvent());
    }

    r->GetAnalysisTree()->PrintObservables();

    fCanDefault->cd();
    return kTRUE;
}

void TRestBrowser::ExitAction() { gSystem->Exit(0); }

void TRestBrowser::InitFromConfigFile() { cout << __PRETTY_FUNCTION__ << endl; }
