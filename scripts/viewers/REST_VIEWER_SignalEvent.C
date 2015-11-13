




REST_VIEWER_SignalEvent(TString fName=" "){

TRestBrowser *browser = new TRestBrowser( );

TRestSignalEventViewer *viewer = new TRestSignalEventViewer( );
browser->SetViewer( viewer );

browser->OpenFile(fName);

}





