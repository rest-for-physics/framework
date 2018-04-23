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



class REST_ViewEvents :public TRestTask {
public:
	ClassDef(REST_ViewEvents, 1);

	REST_ViewEvents() { fNRequiredArgument = 1; }
	~REST_ViewEvents() {}

	TString filename = " ";
	TString eventype = "";

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer_GenericEvents(filename, eventype);
		GetChar("Running...\nPress a key to exit");
	}

};


#endif // !REST_ViewEvents

