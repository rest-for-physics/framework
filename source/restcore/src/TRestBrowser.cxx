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
//delete frmMain;

}

void TRestBrowser::Initialize(){

isFile=kFALSE;

run = new TRestRun();

frmMain = new TGMainFrame(gClient->GetRoot(),300,200);
frmMain->SetCleanup(kDeepCleanup);
frmMain->SetWindowName("Controller");

fCurrentEvent=0;

setButtons( );

canvas = new TCanvas("TEST");

frmMain->DontCallClose();
frmMain->MapSubwindows();
//frmMain->Resize();
frmMain->Layout();
frmMain->MapWindow();

}

void TRestBrowser::setButtons( ){


    fVFrame = new TGVerticalFrame(frmMain);
    
    fNEvent= new TGNumberEntry(fVFrame,fCurrentEvent);
    fNEvent->SetIntNumber(fCurrentEvent);
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

if(LoadEvent(eventN))fCurrentEvent =eventN;

}

void TRestBrowser::LoadNextEventAction( ){

Int_t nextEvent = fCurrentEvent+1;

cout<<" Next event "<<nextEvent<<"  "<<fCurrentEvent<<endl;

if(LoadEvent(nextEvent)){
fCurrentEvent=nextEvent;
fNEvent->SetIntNumber(fCurrentEvent);
}

}

void TRestBrowser::LoadPrevEventAction( ){

Int_t prevEvent = fCurrentEvent-1;

cout<<" Previous event "<<prevEvent<<endl;

if(LoadEvent(prevEvent)){
fCurrentEvent =prevEvent;
fNEvent->SetIntNumber(fCurrentEvent);

}
}

void TRestBrowser::LoadFileAction( ){
 
TGFileInfo fi;
new TGFileDialog (gClient->GetRoot(),gClient->GetRoot() , kFDOpen, &fi);

TString dir = fi.fFilename;

cout<<"Opening "<<dir.Data( )<<endl;

	if(!run->fileExists( dir.Data() )){
	OpenFile(dir);
	fCurrentEvent=0;
	fNEvent->SetIntNumber(fCurrentEvent);
	}

}

Bool_t TRestBrowser::OpenFile( TString fName )
{

    string fname = fName.Data();
    if( !run->fileExists( fname ) ) {
        cout << "WARNING. Input file does not exist" << endl;
        return kFALSE; 
    }
    else
        run->OpenInputFile( fName );


    if( run == NULL ){
        cout << "WARNING no TRestG4Run class was found" << endl;
        return kFALSE; 
    }
    
    isFile=kTRUE;
    run->PrintInfo();
    
    run->SetInputEvent( ev );

    tr = (TTree *) run->GetInputEventTree();
    
    
    return kTRUE; 

}

Bool_t TRestBrowser::LoadEvent( Int_t n ){

    if(!isFile){cout<<"No file found, please load a valid file"<<endl; return kFALSE;}

    if(n<0||n >= tr->GetEntries()){

        cout<<"Event out of range 0-"<<tr->GetEntries()-1<<endl;

        return kFALSE;
    }

    cout<<"Loading Event "<<n<<endl;
    
    
    tr->GetEntry( n );

    canvas->cd();
    pad = ev->DrawEvent( );
    if(pad==NULL){cout<<"Empty event "<<endl;return kTRUE;}
    pad->Draw( );
    pad->Update();
    canvas->Update( );

    //viewer->ViewEvent( "ogl" );


    return kTRUE;

}



void TRestBrowser::ExitAction( ){
 
gApplication->Terminate(0);

}

