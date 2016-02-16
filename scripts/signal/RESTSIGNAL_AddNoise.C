
#include <iostream>
using namespace std;


Int_t RESTSIGNAL_AddNoise( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
{
    cout << "Filename : " << fName << endl;
    cout << "Config file : " << cfgFilename << endl;

    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestAddSignalNoiseProcess *sNoiseProcess = new TRestAddSignalNoiseProcess( cfgFilename );
    
    run->AddProcess( sNoiseProcess, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
