
REST_VIEWER_TrackEvent(TString fName=" ", TString option = "" ){

TRestBrowser *browser = new TRestBrowser( );

TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
TRestTrackEvent *tEvent = new TRestTrackEvent();
viewer->SetEvent(tEvent);

viewer->SetOption( option );

browser->SetViewer( viewer );

browser->OpenFile(fName);

}
