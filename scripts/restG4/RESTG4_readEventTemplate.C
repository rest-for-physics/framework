
Int_t RESTG4_readEventTemplate( TString fName )
{
    TRestRun *run = new TRestRun();

    string fname = fName.Data();
    if( !run->fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }
    else run->OpenInputFile( fName );

    run->OpenInputFile( fName );

    run->PrintInfo();

    /////////////////////////////
    
    /////////////////////////////
    // Reading event
    TRestG4Event *ev = new TRestG4Event();

    run->SetInputEvent( ev );

    run->GetEntry ( 0 );

    ev->PrintEvent();


    // Do what ever you want with g4Event 

    /////////////////////////////

    delete run;
    delete ev;
}
