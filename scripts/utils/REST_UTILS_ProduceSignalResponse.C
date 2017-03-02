
Int_t REST_UTILS_ProduceSignalResponse( TString inputFileName, TString outputFileName, Int_t nPoints = 512, Double_t threshold = 1 )
{
    TRestRun *run = new TRestRun();
    run->OpenInputFile( inputFileName );

    TRestSignalEvent *evt = new TRestSignalEvent();
    run->PrintAllMetadata();
    run->SetInputEvent( evt );

    TRestSignal *averageSignal = new TRestSignal();
    averageSignal->Initialize();
    for( int i = 0; i < nPoints; i++ )
        averageSignal->NewPoint( i, 0 );

    Int_t n = 0;
    for( int i = 0; i < run->GetEntries(); i++ )
    {
        run->GetEntry(i);
        if( evt->GetNumberOfSignals() == 1 )
        {
            TRestSignal *s = evt->GetSignal(0);
            if ( s->GetMaxPeakValue() > 2500 )
            {
                averageSignal->SignalAddition( s );
                n++;
            }

        }
    }
    Double_t factor = 1. / ((double) n);
    averageSignal->MultiplySignalBy( factor );

    Double_t baseline = averageSignal->SubstractBaseline( 20, 100 );

    Double_t peakTime = averageSignal->GetMaxPeakTime();
    cout << "Peak time : " << peakTime << endl;

    Int_t continueLoop = 1;
    Int_t startTime = 0;
    for( int i = peakTime; i > 0 && continueLoop; i-- )
    {
        if( averageSignal->GetData( i ) <= threshold )
        {
            startTime = i;
            continueLoop = 0;
        }
    }

    continueLoop = 1;
    Int_t endTime = 0;
    for( int i = peakTime; i < nPoints && continueLoop; i++ )
    {
        if( averageSignal->GetData( i ) <= threshold )
        {
            endTime = i;
            continueLoop = 0;
        }
    }

    TRestSignal *responseSignal = new TRestSignal();
    responseSignal->Initialize();
    for( int i = startTime; i <= endTime; i++ )
        responseSignal->NewPoint( i, averageSignal->GetData(i) );

    Double_t integral = responseSignal->GetIntegral();
    responseSignal->MultiplySignalBy( 1./integral );

    cout << "Normalized integral : " << responseSignal->GetIntegral() << endl;

    TFile *f = new TFile( outputFileName, "RECREATE" );
    f->cd();
    responseSignal->Write("signal Response");
    responseSignal->GetGraph()->Draw();

    f->Close();

    cout << "Number of entries : " << run->GetEntries() << endl;
    cout << "Number of good signals : " << n << endl;
}
