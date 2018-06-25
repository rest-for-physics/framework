

REST_VIEWER_LinearTrackEvent(TString fName=" "){

    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
    TRestLinearTrackEvent *tEvent = new TRestLinearTrackEvent();
    viewer->SetEvent(tEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);

}



