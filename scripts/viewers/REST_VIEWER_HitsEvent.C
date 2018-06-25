
REST_VIEWER_HitsEvent(TString fName=" ", TString option = "", Int_t canvasWidth = 800, Int_t canvasHeight = 600 )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( canvasWidth, canvasHeight );

    TRestHitsEvent *hEvent = new TRestHitsEvent();

    viewer->SetOption( option );
    viewer->SetEvent(hEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);
}
