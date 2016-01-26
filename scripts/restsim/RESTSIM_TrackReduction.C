
#include <iostream>
using namespace std;


Int_t RESTSIM_TrackReduction( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    cout << "N processes : " << run->GetNumberOfProcesses() << endl;

    run->SetRunType( "trackReduction" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestTrackReductionProcess *trackReductionProcess = new TRestTrackReductionProcess( );
    
    run->AddProcess( trackReductionProcess, cfgFilename );

    run->Start( );

    delete run;
}
