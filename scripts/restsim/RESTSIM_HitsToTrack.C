
Int_t RESTSIM_HitsToTrack( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{

    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestHitsToTrackProcess *hitsToTrackProcess = new TRestHitsToTrackProcess( );
    
    run->AddProcess( hitsToTrackProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
