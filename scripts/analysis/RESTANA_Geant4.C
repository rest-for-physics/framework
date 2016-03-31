
Int_t RESTANA_Geant4( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{
    TRestRun *run = new TRestRun( cfgFilename );

    run->OpenInputFile( fName );

    run->PrintInfo();
    
    TRestGeant4AnalysisProcess *g4AnalysisProcess = new TRestGeant4AnalysisProcess( );
    
    run->AddProcess( g4AnalysisProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
