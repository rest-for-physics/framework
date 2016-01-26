
#include <iostream>
using namespace std;


Int_t RESTSIM_TrackPathMinimization( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    cout << "N processes : " << run->GetNumberOfProcesses() << endl;

    run->SetRunType( "trackPathMinimization" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestTrackPathMinimizationProcess *trackPathMinimizationProcess = new TRestTrackPathMinimizationProcess( );
    
    run->AddProcess( trackPathMinimizationProcess, cfgFilename );

    run->Start( );

    delete run;
}
