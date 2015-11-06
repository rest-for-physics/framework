

RESTRAW_DrawEvent(TString fName){

TRestAGETToSignalProcess *agetToSignal = new TRestAGETToSignalProcess( );
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
        
        pad = processedEvent->DrawEvent( );
        if(pad==NULL)continue;
        
	pad->Draw( );
	pad->Update();
	can->Update( );
	
	getchar();
        
    }

cout<<"End "<<endl;

}





