
Int_t RESTSIM_HitsToSignal( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{

    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();

    TRestHitsToSignalProcess *hitsToSignalProcess = new TRestHitsToSignalProcess( );

    run->AddProcess( hitsToSignalProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
