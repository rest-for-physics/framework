#include "TRestTask.h"
#include "TRestReadoutEventViewer.h"
#include "TRestBrowser.h"
Int_t REST_Viewer_ReadoutEvent(TString fName=" ", TString cfgFilename = "template/config.rml" ){

    TRestBrowser *browser = new TRestBrowser( );

    TRestReadoutEventViewer *viewer = new TRestReadoutEventViewer( (char *) cfgFilename.Data() );
    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}





