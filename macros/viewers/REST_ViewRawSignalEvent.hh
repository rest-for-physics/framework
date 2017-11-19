#include "TRestTask.h"
#include "TRestGenericEventViewer.h"
#include "TRestRawSignalEvent.h"
#include "TRestBrowser.h"
Int_t REST_Viewer_RawSignalEvent( TString fName = " " )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
    TRestRawSignalEvent *sEvent = new TRestRawSignalEvent();
    viewer->SetEvent(sEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}
