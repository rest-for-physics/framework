#include "TRestTask.h"
#include "TRestGenericEventViewer.h"
#include "TRestBrowser.h"

#include "TRestLinearTrackEvent.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"
#include "TRestTrackEvent.h"

#ifndef RestTask_ViewEvents
#define RestTask_ViewEvents


Int_t REST_ViewGenericEvents(TString fName , TString EventType = "")
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

Int_t REST_ViewLinearTrackEvent(TString fName)
{
	REST_ViewGenericEvents(fName, "TRestLinearTrackEvent");
	return 0;
}

Int_t REST_ViewRawSignalEvent(TString fName)
{
	REST_ViewGenericEvents(fName, "TRestRawSignalEvent");
	return 0;
}


Int_t REST_ViewSignalEvent(TString fName)
{
	REST_ViewGenericEvents(fName, "TRestSignalEvent");
	return 0;
}

Int_t REST_ViewTrackEvent(TString fName)
{
	REST_ViewGenericEvents(fName, "TRestTrackEvent");
	return 0;
}


#endif // !REST_ViewEvents

