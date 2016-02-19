
Int_t RESTSIM_TrackLinearization( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestTrackLinearizationProcess *trackLinearizationProcess = new TRestTrackLinearizationProcess( );
    
    run->AddProcess( trackLinearizationProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
