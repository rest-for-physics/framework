
REST_VIEWER_HitsEvent( TString fName=" ",  TString option = "" )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );

    TRestHitsEvent *hEvent = new TRestHitsEvent();

    viewer->SetOption( option );
    viewer->SetEvent(hEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);
}
