
Int_t RESTSIM_ElectronDiffusion( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestElectronDiffusionProcess *eDiffProcess = new TRestElectronDiffusionProcess( );
    
    run->AddProcess( eDiffProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
