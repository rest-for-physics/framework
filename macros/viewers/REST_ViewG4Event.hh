#include "TRestTask.h"
#include "TRestG4EventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewG4Events
#define RestTask_ViewG4Events

Int_t REST_Viewer_G4Event( TString fName)
{
    TRestBrowser *browser = new TRestBrowser( "TRestG4EventViewer");

	TRestEvent*eve = new TRestG4Event();
	browser->SetInputEvent(eve);

    browser->OpenFile(fName);
	
    return 0;
}


class REST_ViewG4Event :public TRestTask {
public:
	ClassDef(REST_ViewG4Event, 1);

	REST_ViewG4Event() { fNRequiredArgument = 1; }
	~REST_ViewG4Event() {}

	TString filename = " ";

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer_G4Event(filename);
		GetChar("Running...\nPress a key to exit");
	}

};

#endif