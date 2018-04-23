#include "TRestTask.h"
#include "TRestG4EventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewG4Events
#define RestTask_ViewG4Events

Int_t REST_Viewer_G4Event( TString fName)
{
    TRestBrowser *browser = new TRestBrowser( "TRestG4EventViewer");

    browser->OpenFile(fName);
	
    return 0;
}


class REST_ViewG4Events :public TRestTask {
public:
	ClassDef(REST_ViewG4Events, 1);

	REST_ViewG4Events() { fNRequiredArgument = 1; }
	~REST_ViewG4Events() {}

	TString filename = " ";

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer_G4Event(filename);
		GetChar("Running...\nPress a key to exit");
	}

};

#endif