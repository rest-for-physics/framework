// Example showing how to use TRestElectronDiffusionProcess

Int_t RESTSIM_AllProcesses( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    TString str = TString( gSystem->Getenv("REST_PATH") );// + "inputData/definitions/readouts.root" << endl;

    TString readoutsFile = str + "/inputData/definitions/pandaXIII.root";
    cout << readoutsFile << endl;

    TString readoutName = "Readout-PANDA_3MM_Jinping";
    run->ImportMetadata( readoutsFile, readoutName );

    run->PrintInfo();

    TRestGeant4AnalysisProcess *g4AnalysisProcess = new TRestGeant4AnalysisProcess( );
    
    TRestElectronDiffusionProcess *eDiffProcess = new TRestElectronDiffusionProcess( );

    TRestHitsToSignalProcess *hToSignalProcess = new TRestHitsToSignalProcess( );

    TRestSignalAnalysisProcess *signalAnalysisProcess = new TRestSignalAnalysisProcess( );

    run->AddProcess( g4AnalysisProcess, cfgFilename );
    run->AddProcess( eDiffProcess, cfgFilename );
    run->AddProcess( hToSignalProcess, cfgFilename );
    run->AddProcess( signalAnalysisProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;

    return 0;
   
}
