
REST_VIEWER_TrackEvent(TString fName=" ", TString option = "", Int_t canvasWidth = 800, Int_t canvasHeight = 600 )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( canvasWidth, canvasHeight );
    TRestTrackEvent *tEvent = new TRestTrackEvent();
    viewer->SetEvent(tEvent);

    viewer->SetOption( option );

    browser->SetViewer( viewer );

    browser->OpenFile(fName);
}
