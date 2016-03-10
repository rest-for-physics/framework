
Int_t REST_UTILS_TranslateHitsEvent( TString fName, Double_t deltax, Double_t deltay, Double_t deltaz )
{
    TRestRun *run = new TRestRun();

    TTree *fOutputEventTree = new TTree();

    run->OpenInputFile( fName );

    run->PrintInfo();

    run->SetRunType( "hitsTranslation" );
    run->OpenOutputFile( );

    TRestHitsEvent *evt = new TRestHitsEvent();
    TRestHitsEvent *outEvt = new TRestHitsEvent();

    run->SetInputEvent( evt );
    run->SetOutputEvent( outEvt );

    Int_t nEntries = run->GetInputEventTree()->GetEntries();

    TRestHitsEvent *processedEvent = new TRestHitsEvent();

    for (int i = 0; i < nEntries; i++)
    {
	run->GetInputEventTree()->GetEntry( i );
	
	processedEvent = evt;

	//processedEvent->PrintEvent();

	processedEvent->fHits->Translate (deltax, deltay, deltaz);
	outEvt = processedEvent;

	//outEvt->PrintEvent(); 

	outEvt->SetEventID( evt->GetEventID() );
	outEvt->SetEventTime( evt->GetEventTime() );

	fOutputEventTree->Fill();
    }

cout << fOutputEventTree->GetEntries() << " processed events" << endl;

    delete run;
}
