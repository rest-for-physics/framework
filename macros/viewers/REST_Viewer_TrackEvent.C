
Int_t REST_Viewer_TrackEvent(TString fName=" ")
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
    TRestTrackEvent *tEvent = new TRestTrackEvent();
    viewer->SetEvent(tEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}
