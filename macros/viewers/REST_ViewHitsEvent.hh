#include "TRestTask.h"
#include "TRestHitsEventViewer.h"
#include "TRestBrowser.h"

#ifndef RestTask_ViewHitsEvents
#define RestTask_ViewHitsEvents

Int_t REST_Viewer_HitsEvent( TString fName)
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestHitsEventViewer *viewer = new TRestHitsEventViewer( );
    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}

class REST_ViewHitsEvent :public TRestTask {
public:
	ClassDef(REST_ViewHitsEvent, 1);

	REST_ViewHitsEvent() { fNRequiredArgument = 1; }
	~REST_ViewHitsEvent() {}

	TString filename = " ";

	void RunTask(TRestManager*mgr)
	{
		REST_Viewer_HitsEvent(filename);
		gApplication->Run();
	}

};

#endif