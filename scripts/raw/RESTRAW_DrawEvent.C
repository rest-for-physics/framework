

RESTRAW_DrawEvent(TString fName, char *cfgFileName){

TRestFEMINOSToSignalProcess *femToSignal = new TRestFEMINOSToSignalProcess(cfgFileName);
    	if(!agetToSignal->OpenInputBinFile(fName)){
    	cout<<"File "<<fName.Data()<<" not found"<<endl;
    	exit(0);
    	}

//agetToSignal->SetVerboseLevel(3);
    	
TRestEvent *processedEvent = new TRestSignalEvent();

TCanvas *can = new TCanvas("test","test");
TPad *pad;

agetToSignal->InitProcess();

while( processedEvent!=NULL )
    {
        agetToSignal->BeginOfEventProcess();
        processedEvent = agetToSignal->ProcessEvent( NULL );
        agetToSignal->EndOfEventProcess();
        
        if(processedEvent==NULL)break;
        
        pad = processedEvent->DrawEvent( );
        if(pad==NULL)continue;
        
	pad->Draw( );
	pad->Update();
	can->Update( );
	
	getchar();
        
    }

cout<<"End "<<endl;

}





