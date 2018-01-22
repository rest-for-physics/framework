#include "TRestTask.h"
#include "TRestReadoutEventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewReadoutEvents
#define RestTask_ViewReadoutEvents

Int_t REST_Viewer_ReadoutEvent(TString fName, TString cfgFilename = "template/config.rml" ){

    TRestBrowser *browser = new TRestBrowser( );

    TRestReadoutEventViewer *viewer = new TRestReadoutEventViewer( (char *) cfgFilename.Data() );
    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}

class REST_ViewReadoutEvent :public TRestTask {
public:
	ClassDef(REST_ViewReadoutEvent, 1);

	REST_ViewReadoutEvent() { fNRequiredArgument = 1; }
	~REST_ViewReadoutEvent() {}

	TString filename = " ";
	TString cfgFilename = "template/config.rml";

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer_ReadoutEvent(filename, cfgFilename);
		gApplication->Run();
	}

};


#endif





