#include "TRestTask.h"
#include "TRestReadoutEventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewReadoutEvents
#define RestTask_ViewReadoutEvents

Int_t REST_ViewReadoutEvent(TString fName, TString cfgFilename = "template/config.rml" ){

    TRestBrowser *browser = new TRestBrowser( );

    TRestReadoutEventViewer *viewer = new TRestReadoutEventViewer( (char *) cfgFilename.Data() );
    browser->SetViewer( viewer );

    browser->OpenFile(fName);

	//when we run this macro from restManager from bash,
	//we need to call TRestMetadata::GetChar() to prevent returning,
	//while keeping GUI alive.
#ifdef REST_MANAGER
	browser->GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}

#endif





