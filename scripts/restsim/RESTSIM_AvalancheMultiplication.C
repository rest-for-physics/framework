
Int_t RESTSIM_AvalancheMultiplication( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestAvalancheProcess *eAvalancheProcess = new TRestAvalancheProcess( );
    
    run->AddProcess( eAvalancheProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
