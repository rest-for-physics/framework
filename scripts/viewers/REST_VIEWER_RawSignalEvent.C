

REST_VIEWER_RawSignalEvent(TString fName=" "){

TRestBrowser *browser = new TRestBrowser( );

TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
TRestRawSignalEvent *sEvent = new TRestRawSignalEvent();
viewer->SetEvent(sEvent);

browser->SetViewer( viewer );

browser->OpenFile(fName);

}



