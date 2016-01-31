
Int_t RESTSIM_SignalToHits( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
{

    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestSignalToHitsProcess *signalToHitsProcess = new TRestSignalToHitsProcess( );
    
    run->AddProcess( signalToHitsProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
