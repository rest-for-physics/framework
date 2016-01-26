
#include <iostream>
using namespace std;


Int_t RESTSIM_SignalToHits( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    cout << "N processes : " << run->GetNumberOfProcesses() << endl;

    run->SetRunType( "signalToHits" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestSignalToHitsProcess *signalToHitsProcess = new TRestSignalToHitsProcess( );
    
    run->AddProcess( signalToHitsProcess, cfgFilename );

    run->Start( );

    delete run;
}
