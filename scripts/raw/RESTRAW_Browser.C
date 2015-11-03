




RESTRAW_Browser(TString fName=" "){

TRestBrowser *browser = new TRestBrowser( );

TRestSignalEvent *ev = new TRestSignalEvent( );
browser->SetInputEvent( ev );

browser->OpenFile(fName);

}





