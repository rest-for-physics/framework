#include "TRestTask.h"
#include "TRestHitsEventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewHitsEvents
#define RestTask_ViewHitsEvents

Int_t REST_Viewer_HitsEvent( TString fName)
{
    TRestBrowser *browser = new TRestBrowser("TRestHitsEventViewer" );

	TRestEvent*eve = new TRestHitsEvent();
	browser->SetInputEvent(eve);

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