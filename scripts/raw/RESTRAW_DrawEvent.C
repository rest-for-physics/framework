




RESTRAW_DrawEvent(TString fName){

run = new TRestRun();

run->OpenInputFile( fName );

TRestSignalEvent *ev = new TRestSignalEvent( );
run->SetInputEvent( ev );

tr = (TTree *) run->GetInputEventTree();

cout << "Total number of entries : " << tr->GetEntries() << endl;

TCanvas *can = new TCanvas("test","test");
TPad *pad;

	for(int i=0;i<tr->GetEntries();i++){
	
	tr->GetEntry( i );
	can->cd();
	pad = ev->DrawEvent( );
	pad->Draw( );
	pad->Update();
	can->Update( );
	
	getchar();
	}


}





