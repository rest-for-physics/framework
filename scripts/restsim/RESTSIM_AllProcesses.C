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

    TRestSignalToHitsProcess *signalToHitsProcess = new TRestSignalToHitsProcess( );

    TRestFastHitsToTrackProcess *hitsToTrackProcess = new TRestFastHitsToTrackProcess( );

    TRestTrackReductionProcess *trackReductionProcess = new TRestTrackReductionProcess( );

    TRestTrackPathMinimizationProcess *trackPathMinimizationProcess = new TRestTrackPathMinimizationProcess( );

    // TRestG4Event
    // ============
    // This process fills the analysis tree taking values from a TRestG4Event
    run->AddProcess( g4AnalysisProcess, cfgFilename );

    // This process produces the electron generation and diffusion of the hits inside a TRestG4Event
    // It uses the readout to define the drift volume and the gas properties.
    // TRestG4Event will be transformed into a TRestHitsEvent
    run->AddProcess( eDiffProcess, cfgFilename );

    // TRestHitsEvent
    // ============
    // This process will take the hits in a TRestHitsEvent.
    // It will translate them to signals following the description of the readout provided, and the properties of the gas.
    // TRestHitsEvent will be transformed into a TRestSignalEvent
    run->AddProcess( hToSignalProcess, cfgFilename );

    // TRestSignalEvent
    // ================
    // This process extracts parameter from a TRestSignalEvent and will fill the analysis tree.
    run->AddProcess( signalAnalysisProcess, cfgFilename );

    // This process will do the hit reconstruction, recovering the hits from signal daq channels.
    // It will use the readout and the gas properties
    run->AddProcess( signalToHitsProcess, cfgFilename );

    // This process will do clusterization of neightbouring hits grouping them into tracks
    run->AddProcess( hitsToTrackProcess, cfgFilename );

    // TRestTrackEvent
    // ================
    // This process will do hit reduction, it will merge the closest hits up to a defined distance,
    // or until the number of nodes below a given value.
    run->AddProcess( trackReductionProcess, cfgFilename );

    // This process will find the minimum path between the hits of each track. 
    // This process does a very basic blob finding (for testing pourposes), this needs to be debugged and optimized.
    // The blobs are used to define the fixed extremes in the path minimization.
    run->AddProcess( trackPathMinimizationProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;

    return 0;
   
}
