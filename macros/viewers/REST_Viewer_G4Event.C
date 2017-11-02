
Int_t REST_Viewer_G4Event( TString fName = " " )
{
    TRestBrowser *browser = new TRestBrowser( );

    TRestG4EventViewer *viewer = new TRestG4EventViewer( );
    browser->SetViewer( viewer );

    browser->OpenFile(fName);

    return 0;
}

