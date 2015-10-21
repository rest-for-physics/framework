//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>
#include <unistd.h>
#include <iostream>
//#include <TRestRun.h>
//#include <TRestG4Event.h>
//#include <TRestG4EventViewer.h>

using namespace std;

Int_t REST_VIEWER_Event(TString fName = "")
{

gSystem->Load("librestcore.so");
gSystem->Load("librestsim.so");

 
TRestBrowserEvent *browser = new TRestBrowserEvent( );
browser->OpenFile(fName.Data());


}




