
Int_t REST_UTILS_Rotate3DHitsEvent( TString fName, Double_t alpha, Double_t beta, Double_t gamma )
{

    Double_t alpha_rad = alpha * TMath::Pi() / 180. ;  // rotation angle around Z-axis
    Double_t beta_rad  = beta  * TMath::Pi() / 180. ;  // rotation angle around Y-axis
    Double_t gamma_rad = gamma * TMath::Pi() / 180. ;  // rotation angle around X-axis


    cout << "Rotation of "<< endl;
    cout <<alpha_rad << " radians around Z-axis." << endl;
    cout <<beta_rad << " radians around Y-axis." << endl;
    cout <<gamma_rad << " radians around X-axis." << endl;

    TRestRun *run = new TRestRun();

    TTree *fOutputEventTree = new TTree();

    run->OpenInputFile( fName );

    run->PrintInfo();

    run->SetRunType( "hitsRotation" );
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

	processedEvent->PrintEvent(); getchar();

	processedEvent->fHits->RotateIn3D( alpha_rad , beta_rad , gamma_rad );
	outEvt = processedEvent;

	outEvt->PrintEvent(); getchar();

	outEvt->SetEventID( evt->GetEventID() );
	outEvt->SetEventTime( evt->GetEventTime() );

	fOutputEventTree->Fill();
    }

cout << fOutputEventTree->GetEntries() << " processed events" << endl;

    delete run;
}
