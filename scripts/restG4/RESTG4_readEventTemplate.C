
Int_t RESTG4_readEventTemplate( TString fName )
{
    cout << "Filename : " << fName << endl;

    gSystem->Load("librestcore.so");

    TRestRun *run = new TRestRun();

    string fname = fName.Data();
    if( !run->fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }
    else run->OpenInputFile( fName );

    if( run == NULL ) { cout << "WARNING no TRestG4Run class was found" << endl; exit(1); }

    run->PrintInfo();

    /////////////////////////////
    
    /////////////////////////////
    // Reading event
    TRestG4Event *ev = new TRestG4Event();

    run->SetInputEvent( ev );

    TTree *tr = (TTree *) run->GetInputEventTree();

    cout << "Total number of entries : " << tr->GetEntries() << endl;

    // Printing the first event
    if( tr->GetEntries() > 0 )
    {
        tr->GetEntry(3);
        ev->PrintEvent();
    }
    /////////////////////////////

    delete run;
    delete ev;
}
