
Int_t RESTSIM_GaussianConvolution( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestSignalGaussianConvolutionProcess *eGaussConvProcess = new TRestSignalGaussianConvolutionProcess( );
    
    run->AddProcess( eGaussConvProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
