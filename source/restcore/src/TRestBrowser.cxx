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

///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestBrowser.h"

ClassImp(TRestBrowser)
//______________________________________________________________________________
TRestBrowser::TRestBrowser()
{

Initialize();
}

//______________________________________________________________________________
TRestBrowser::~TRestBrowser()
{

frmMain->Cleanup();
delete frmMain;

}

void TRestBrowser::Initialize(){

frmMain = new TGMainFrame(gClient->GetRoot(),300,200);
frmMain->SetCleanup(kDeepCleanup);
frmMain->SetWindowName("Controller");

currentEvent=0;

setButtons( );


frmMain->DontCallClose();
frmMain->MapSubwindows();
//frmMain->Resize();
frmMain->Layout();
frmMain->MapWindow();

}

void TRestBrowser::setButtons( ){


    fVFrame = new TGVerticalFrame(frmMain);
    
    fNEvent= new TGNumberEntry(fVFrame,currentEvent);
    fNEvent->SetIntNumber(currentEvent);
    fVFrame->AddFrame(fNEvent,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
            
    fLoadEvent = new  TGTextButton(fVFrame,"LOAD");///< Load Event button
    fLoadEvent->Connect("Clicked()","TRestBrowser",this,"LoadEventAction()");
    fVFrame->AddFrame(fLoadEvent,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    
    TString icondir(TString::Format("%s/icons/", getenv("ROOTSYS")));
    
    fButNext = new TGPictureButton(fVFrame, gClient->GetPicture(icondir+"GoForward.gif"));
    fButNext->Connect("Clicked()","TRestBrowser",this,"LoadNextEventAction()");
    fVFrame->AddFrame(fButNext,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    
    fButPrev = new TGPictureButton(fVFrame, gClient->GetPicture(icondir+"GoBack.gif"));
    fButPrev->Connect("Clicked()","TRestBrowser",this,"LoadPrevEventAction()");
    fVFrame->AddFrame(fButPrev,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    
    fMenuOpen = new TGPictureButton(fVFrame, gClient->GetPicture(icondir+"bld_open.png"));
    fMenuOpen->Connect("Clicked()","TRestBrowser",this,"LoadFileAction()");
    fVFrame->AddFrame(fMenuOpen,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    
    fExit = new  TGTextButton(fVFrame,"EXIT");///< Exit button
    fExit->Connect("Clicked()","TRestBrowser",this,"ExitAction()");
    fVFrame->AddFrame(fExit,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    
    
frmMain->AddFrame(fVFrame,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

}


void TRestBrowser::LoadEventAction( ){

Int_t eventN = (Int_t)fNEvent->GetNumber();

cout<<"Loading event "<<eventN<<endl;

if(LoadEvent(eventN))currentEvent =eventN;

}

void TRestBrowser::LoadNextEventAction( ){

Int_t nextEvent = currentEvent+1;

cout<<" Next event "<<nextEvent<<"  "<<currentEvent<<endl;

if(LoadEvent(nextEvent)){
currentEvent=nextEvent;
fNEvent->SetIntNumber(currentEvent);
}

}

void TRestBrowser::LoadPrevEventAction( ){

Int_t prevEvent = currentEvent-1;

cout<<" Previous event "<<prevEvent<<endl;

if(LoadEvent(prevEvent)){
currentEvent =prevEvent;
fNEvent->SetIntNumber(currentEvent);

}
}

void TRestBrowser::LoadFileAction( ){
 
TGFileInfo fi;
new TGFileDialog (gClient->GetRoot(),gClient->GetRoot() , kFDOpen, &fi);

TString dir = fi.fFilename;

cout<<"Opening "<<dir.Data( )<<endl;

FILE *fExist;

	if((fExist=fopen(dir.Data(),"r"))!=NULL){
	fclose(fExist);
	OpenFile(dir);
	currentEvent=0;
	fNEvent->SetIntNumber(currentEvent);
	}

delete fExist;
//delete fDialog;

}



void TRestBrowser::ExitAction( ){
 
gApplication->Terminate(0);

}

