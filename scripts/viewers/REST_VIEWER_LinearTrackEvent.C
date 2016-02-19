

REST_VIEWER_LinearTrackEvent(TString fName=" "){

    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
    TRestLinearizedTrackEvent *tEvent = new TRestLinearizedTrackEvent();
    viewer->SetEvent(tEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);

}



