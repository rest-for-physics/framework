




REST_VIEWER_G4Event(TString fName=" ", TString option = "", Int_t canvasWidth = 800, Int_t canvasHeight = 600 ){

    TRestBrowser *browser = new TRestBrowser( );

    if( option == "eveViewer" )
    {
        TRestG4EventViewer *viewer = new TRestG4EventViewer( );
        browser->SetViewer( viewer );
    }
    else
    {

        TRestGenericEventViewer *viewer = new TRestGenericEventViewer( canvasWidth, canvasHeight );
        TRestG4Event *tEvent = new TRestG4Event();
        viewer->SetEvent(tEvent);

        viewer->SetOption( option );

        browser->SetViewer( viewer );
    }

    browser->OpenFile(fName);

}





