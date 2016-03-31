
Int_t RESTSIM_TrackPathMinimization( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestTrackPathMinimizationProcess *trackPathMinimizationProcess = new TRestTrackPathMinimizationProcess( );
    
    run->AddProcess( trackPathMinimizationProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
