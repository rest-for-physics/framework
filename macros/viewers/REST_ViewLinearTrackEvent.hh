#include "TRestTask.h"
#include "TRestGenericEventViewer.h"
#include "TRestLinearTrackEvent.h"
#include "TRestBrowser.h"
Int_t REST_Viewer_LinearTrackEvent(TString fName=" ")
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
    TRestLinearTrackEvent *tEvent = new TRestLinearTrackEvent();
    viewer->SetEvent(tEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}
