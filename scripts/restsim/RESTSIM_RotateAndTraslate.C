
Int_t RESTSIM_RotateAndTraslate( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestHitsRotateAndTraslateProcess *eRotateProcess = new TRestHitsRotateAndTraslateProcess( );
    
    run->AddProcess( eRotateProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
