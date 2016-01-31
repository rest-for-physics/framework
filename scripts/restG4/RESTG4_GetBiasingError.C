
Double_t RESTG4_GetBiasingError( TString fName, Int_t finalEvents = 0 )
{
    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun();
    TRestG4Metadata *metadata = new TRestG4Metadata();

    string fname = fName.Data();
    if( !run->fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }

    TFile *f = new TFile( fName );

    /////////////////////////////
    // Reading metadata classes

    TIter nextkey(f->GetListOfKeys());
    TKey *key;
    while (key = (TKey*)nextkey()) {
        string className = key->GetClassName();
        if ( className == "TRestG4Metadata" )
        {
            metadata = (TRestG4Metadata *) f->Get( key->GetName() );
        }
        if ( className == "TRestRun" )
        {
            run = (TRestRun *) f->Get( key->GetName() );
        }
    }

    if( metadata == NULL ) { cout << "WARNING no TRestG4Metadata class was found" << endl; exit(1); }
    if( run == NULL ) { cout << "WARNING no TRestRun class was found" << endl; exit(1); }

    /////////////////////////////
    run->PrintMetadata();
    metadata->PrintMetadata();
    /////////////////////////////
    
    // Reading event
    TRestG4Event *ev = new TRestG4Event();
    TTree *tr = (TTree *) f->Get("TRestG4Event Tree");
    TBranch *br = tr->GetBranch( "eventBranch" );

    br->SetAddress( &ev );

    cout << "Number of biasing volumes : " << metadata->GetNumberOfBiasingVolumes() << endl;


    Double_t sum = 0;
    TH1D *h;
    for( int i = 0; i < metadata->GetNumberOfBiasingVolumes(); i++ )
    {

        TString index;
        index = index.Form("%d", i );

        TString biasSpctName = "Bias_Spectrum_" + index;
        h = (TH1D *) f->Get( biasSpctName );
        cout << "Number of counts in bias " << i << " " << h->Integral() << endl;
        sum += 1./h->Integral();
    }

    h = (TH1D *) f->Get( "Total energy deposited (ROI)" );
    sum += 1./h->Integral();

    Double_t roiEvents = 0;
    if( finalEvents > 0 ) roiEvents = finalEvents;
    else roiEvents = h->Integral();

    sum += 1./roiEvents;

    cout << "Number of events in ROI : " << roiEvents << endl;

    Double_t error = roiEvents * TMath::Sqrt( sum );

    cout << "The error is : " << error << endl;


    delete run;
    delete metadata;

    delete ev;

    f->Close();

    return error;

}
