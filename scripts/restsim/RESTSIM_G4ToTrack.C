
Int_t RESTSIM_G4ToTrack( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestElectronDiffusionProcess *eDiffProcess = new TRestElectronDiffusionProcess( );
    
    run->AddProcess( eDiffProcess, cfgFilename );

    TRestHitsToSignalProcess *hitsToSignalProcess = new TRestHitsToSignalProcess( );

    run->AddProcess( hitsToSignalProcess, cfgFilename );

    TRestSignalToHitsProcess *signalToHitsProcess = new TRestSignalToHitsProcess( );
    
    run->AddProcess( signalToHitsProcess, cfgFilename );

    TRestFastHitsToTrackProcess *hitsToTrackProcess = new TRestFastHitsToTrackProcess( );
    
    run->AddProcess( hitsToTrackProcess, cfgFilename );

    TRestTrackReductionProcess *trackReductionProcess = new TRestTrackReductionProcess( );
    
    run->AddProcess( trackReductionProcess, cfgFilename );

    TRestTrackPathMinimizationProcess *trackPathMinimizationProcess = new TRestTrackPathMinimizationProcess( );
    
    run->AddProcess( trackPathMinimizationProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
