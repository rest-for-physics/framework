///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestBrowser.cxx
///
///             G4 class description
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestBrowser.h"


using namespace std;

ClassImp(TRestBrowser)
//______________________________________________________________________________
TRestBrowser::TRestBrowser()
{
	Initialize();
	SetViewer("TRestGenericEventViewer");
	if (gDirectory->GetFile() != NULL) {
		OpenFile(gDirectory->GetFile()->GetName());
		cout << "Loaded File : " << fInputFileName << endl;
	}
}

TRestBrowser::TRestBrowser(TString viewerName) 
{
	Initialize();
	SetViewer(viewerName);
	if (gDirectory->GetFile() != NULL) {
		OpenFile(gDirectory->GetFile()->GetName());
		cout << "Loaded File : " << fInputFileName << endl;
	}
}

//______________________________________________________________________________
TRestBrowser::~TRestBrowser()
{
	if (frmMain != NULL)
		frmMain->Cleanup();
	//delete frmMain;

}

void TRestBrowser::Initialize() {

	isFile = kFALSE;
	fCurrentEvent = 0;

	b = new TBrowser("Browser", 0, "REST Browser", "FI");
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
	//frmMain->DontCallClose();
	frmMain->MapSubwindows();
	//frmMain->Resize();
	frmMain->Layout();
	frmMain->MapWindow();

}

void TRestBrowser::SetViewer(TRestEventViewer *eV)
{
	if (fEventViewer != NULL) {
		warning << "Event viewer has already been set!" << endl;
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
	TClass *cl = TClass::GetClass(viewerName);
	if (cl == NULL) {
		warning << "cannot find viewer: " << viewerName << " !" << endl;
	}
	else
	{
		TObject*obj = (TObject*)cl->New();
		if (obj->InheritsFrom("TRestEventViewer"))
		{
			SetViewer((TRestEventViewer*)obj);
		}
		else
		{
			warning << viewerName << " is not a viewer!" << endl;
		}
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

		fLoadEvent = new  TGPictureButton(controlbar, gClient->GetPicture(icondir + "refresh.png"));///< Load Event button
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

	fExit = new  TGTextButton(fVFrame, "EXIT");///< Exit button
	fExit->Connect("Clicked()", "TRestBrowser", this, "ExitAction()");
	fVFrame->AddFrame(fExit, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	addFrame(fVFrame);



	//frmMain->AddFrame(fVFrame,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

}

void TRestBrowser::addFrame(TGFrame*f)
{
	frmMain->Resize(TGDimension(300, frmMain->GetHeight() + f->GetHeight()));

	frmMain->AddFrame(f, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	//frmMain->DontCallClose();
	frmMain->MapSubwindows();
	//frmMain->Resize();
	frmMain->Layout();
	frmMain->MapWindow();
}

TGVerticalFrame *TRestBrowser::generateNewFrame()
{
	auto frame = new TGVerticalFrame(frmMain);
	frame->Resize(300, 200);
	return frame;
}

void TRestBrowser::LoadEventAction() {

	Int_t eventN = (Int_t)fNEvent->GetNumber();

	cout << "Loading event " << eventN << endl;

	if (LoadEvent(eventN))fCurrentEvent = eventN;

}

void TRestBrowser::LoadNextEventAction() {

	Int_t nextEvent = fCurrentEvent + 1;

	cout << " Next event " << nextEvent << endl;

	if (LoadEvent(nextEvent))
	{
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

	if (fileExists(dir.Data())) {
		OpenFile(dir);
		fCurrentEvent = 0;
		fNEvent->SetIntNumber(fCurrentEvent);
	}

}

Bool_t TRestBrowser::OpenFile(TString fName)
{

	TGeoManager *geometry = NULL;

	string fname = fName.Data();
	if (!fileExists(fname)) {
		cout << "WARNING. Input file " << fname << " does not exist" << endl;
		return kFALSE;
	}

	if (fInputFile != NULL) fInputFile->Close();
	OpenInputFile(fName);
	fInputFileName = fname;
	if (fInputFile == NULL) {
		error << "failed to open input file" << endl;
		exit(0);
	}
	TIter nextkey(fInputFile->GetListOfKeys());
	TKey *key;
	while ((key = (TKey*)nextkey())) {

		string className = key->GetClassName();

		if (className == "TGeoManager")
			geometry = (TGeoManager *)fInputFile->Get(key->GetName());
		//if(className=="TRestAnalysisTree")
		   // fAnalysisTree= (TRestAnalysisTree *)fInputFile->Get(key->GetName());
	}
	if (fAnalysisTree == NULL && fEventTree == NULL)
	{
		cout << "REST ERROR (OpenFile) : No REST Tree was not found" << endl;
		cout << "Inside file : " << fInputFileName << endl;
		exit(1);
	}

	if (fAnalysisTree != NULL) {
		fAnalysisTree->ConnectEventBranches();
		fAnalysisTree->ConnectObservables();
	}

	if (fEventTree != NULL) {
		fEventTree->ConnectEventBranches();
		//init viewer
		if (fEventViewer == NULL) {
			SetViewer("TRestGenericEventViewer");
		}
	}

	if (geometry != NULL && fEventViewer != NULL)fEventViewer->SetGeometry(geometry);

	//PrintAllMetadata();
	isFile = kTRUE;
	LoadEventAction();

	return kTRUE;

}

Bool_t TRestBrowser::LoadEvent(Int_t n) {

	if (!isFile) { cout << "No file..." << endl; return kFALSE; }
	if (fAnalysisTree != NULL && n < fAnalysisTree->GetEntries() && n >= 0) {
		this->GetEntry(n);
	}
	else { cout << "Event out of limits" << endl; return kFALSE; }
	if (fEventViewer != NULL) {
		fEventViewer->AddEvent(fInputEvent);
	}
	if (fAnalysisTree != NULL)
		GetAnalysisTree()->PrintObservables();


	fCanDefault->cd();
	return kTRUE;
}


void TRestBrowser::ExitAction() {

	gSystem->Exit(0);


}

void TRestBrowser::InitFromConfigFile()
{
	cout << __PRETTY_FUNCTION__ << endl;

}

