
REST_VIEWER_TrackEvent(TString fName=" "){

TRestBrowser *browser = new TRestBrowser( );

TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
TRestTrackEvent *tEvent = new TRestTrackEvent();
viewer->SetEvent(tEvent);

browser->SetViewer( viewer );

browser->OpenFile(fName);

}
