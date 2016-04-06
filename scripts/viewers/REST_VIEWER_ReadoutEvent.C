




REST_VIEWER_ReadoutEvent(TString fName=" ", char *cfgFilename = "template/config.rml" ){

TRestBrowser *browser = new TRestBrowser( );

TRestReadoutEventViewer *viewer = new TRestReadoutEventViewer(cfgFilename);
browser->SetViewer( viewer );

browser->OpenFile(fName);

}





