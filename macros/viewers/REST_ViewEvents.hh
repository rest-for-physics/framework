#include "TRestTask.h"
#include "TRestGenericEventViewer.h"
#include "TRestBrowser.h"

#include "TRestLinearTrackEvent.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"
#include "TRestTrackEvent.h"

#ifndef RestTask_ViewEvents
#define RestTask_ViewEvents


Int_t REST_Viewer_GenericEvents(TString fName , TString EventType = "")
{
	TRestBrowser *browser = new TRestBrowser("TRestGenericEventViewer");
	//TRestGenericEventViewer *viewer = (TRestGenericEventViewer*)browser->GetViewer();
	TRestStringOutput cout;
	cout.setorientation(1);
	TClass*cl = TClass::GetClass(EventType);
	if (cl == NULL||EventType=="") {
		if (cl == NULL) {
			cout << "REST WARNING : unknown event type \"" << EventType << "\"" << endl;
			cout << "Using default event in file..." << endl;
			cout << endl;
		}
		browser->OpenFile(fName);
	}
	else
	{
		TRestEvent*eve = (TRestEvent*)cl->New();
		browser->SetInputEvent(eve);
		//viewer->SetEvent(eve);
		browser->OpenFile(fName);

	}

#ifdef REST_MANAGER
	browser->GetChar("Running...\nPress a key to exit");
#endif

	return 0;
}

Int_t REST_Viewer_LinearTrackEvent(TString fName)
{
	REST_Viewer_GenericEvents(fName, "TRestLinearTrackEvent");
	return 0;
}

Int_t REST_Viewer_RawSignalEvent(TString fName)
{
	REST_Viewer_GenericEvents(fName, "TRestRawSignalEvent");
	return 0;
}


Int_t REST_Viewer_SignalEvent(TString fName)
{
	REST_Viewer_GenericEvents(fName, "TRestSignalEvent");
	return 0;
}

Int_t REST_Viewer_TrackEvent(TString fName)
{
	REST_Viewer_GenericEvents(fName, "TRestTrackEvent");
	return 0;
}


#endif // !REST_ViewEvents

