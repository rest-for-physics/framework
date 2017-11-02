
Int_t REST_Viewer_SignalEvent( TString fName=" " )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestGenericEventViewer *viewer = new TRestGenericEventViewer( );
    TRestSignalEvent *sEvent = new TRestSignalEvent();
    viewer->SetEvent(sEvent);

    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}

