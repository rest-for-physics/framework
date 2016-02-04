




REST_VIEWER_ReadoutEvent(TString fName=" ", char *cfgFilename = "myConfig.rml" ){

TRestBrowser *browser = new TRestBrowser( );

TRestReadoutEventViewer *viewer = new TRestReadoutEventViewer(cfgFilename);
browser->SetViewer( viewer );

browser->OpenFile(fName);

}





