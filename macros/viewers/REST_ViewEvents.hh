#include "TRestTask.h"
#include "TRestGenericEventViewer.h"
#include "TRestBrowser.h"

#include "TRestLinearTrackEvent.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"
#include "TRestTrackEvent.h"

#ifndef RESTTask_ViewEvents
#define RESTTask_ViewEvents


Int_t REST_Viewer(TString fName = " ", TString EventType = "")
{
	TRestBrowser *browser = new TRestBrowser();
	TRestGenericEventViewer *viewer = new TRestGenericEventViewer();
	browser->SetViewer(viewer);

	TClass*cl = TClass::GetClass(EventType);
	if (cl == NULL||EventType=="") {
		browser->OpenFile(fName);
		TRestEvent*eve = browser->GetInputEvent();
		if (eve != NULL) {
			cout << "setting viewer with event : " << eve->ClassName() << endl;
			viewer->SetEvent(eve);
		}
		else
		{
			cout << "Failed to launch viewer!" << endl;
			return 0;
		}
	}
	else
	{
		TRestEvent*eve = (TRestEvent*)cl->New();
		browser->SetInputEvent(eve);
		viewer->SetEvent(eve);
		browser->OpenFile(fName);

	}

	return 0;
}

Int_t REST_Viewer_LinearTrackEvent(TString fName = " ")
{
	TRestBrowser *browser = new TRestBrowser();

	TRestGenericEventViewer *viewer = new TRestGenericEventViewer();
	TRestLinearTrackEvent *tEvent = new TRestLinearTrackEvent();
	viewer->SetEvent(tEvent);

	browser->SetViewer(viewer);

	browser->OpenFile(fName);

	return 0;
}

Int_t REST_Viewer_RawSignalEvent(TString fName = " ")
{
	TRestBrowser *browser = new TRestBrowser();

	TRestGenericEventViewer *viewer = new TRestGenericEventViewer();
	TRestRawSignalEvent *sEvent = new TRestRawSignalEvent();
	viewer->SetEvent(sEvent);

	browser->SetViewer(viewer);

	browser->OpenFile(fName);

	return 0;
}


Int_t REST_Viewer_SignalEvent(TString fName = " ")
{
	TRestBrowser *browser = new TRestBrowser();

	TRestGenericEventViewer *viewer = new TRestGenericEventViewer();
	TRestSignalEvent *sEvent = new TRestSignalEvent();
	viewer->SetEvent(sEvent);

	browser->SetViewer(viewer);

	browser->OpenFile(fName);

	return 0;
}

Int_t REST_Viewer_TrackEvent(TString fName = " ")
{
	TRestBrowser *browser = new TRestBrowser();

	TRestGenericEventViewer *viewer = new TRestGenericEventViewer();
	TRestTrackEvent *tEvent = new TRestTrackEvent();
	viewer->SetEvent(tEvent);

	browser->SetViewer(viewer);

	browser->OpenFile(fName);

	return 0;
}



class REST_ViewEvents :public TRestTask {
public:
	ClassDef(REST_ViewEvents, 1);

	REST_ViewEvents() { fNRequiredArgument = 0; }
	~REST_ViewEvents() {}

	TString filename = " ";
	TString eventype = "";

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer(filename, eventype);
		gApplication->Run();
	}

};


#endif // !REST_ViewEvents

