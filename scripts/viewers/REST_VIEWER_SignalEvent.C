

REST_VIEWER_SignalEvent(TString fName=" "){

TRestBrowser *browser = new TRestBrowser( );

TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
TRestSignalEvent *sEvent = new TRestSignalEvent();
viewer->SetEvent(sEvent);

browser->SetViewer( viewer );

browser->OpenFile(fName);

}



