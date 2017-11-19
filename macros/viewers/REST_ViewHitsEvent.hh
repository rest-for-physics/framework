#include "TRestTask.h"
#include "TRestHitsEventViewer.h"
#include "TRestBrowser.h"
Int_t REST_Viewer_HitsEvent( TString fName=" " )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestHitsEventViewer *viewer = new TRestHitsEventViewer( );
    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}
