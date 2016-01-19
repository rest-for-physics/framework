//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t RESTSIM_HitsToTrack( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    cout << "N processes : " << run->GetNumberOfProcesses() << endl;

    run->SetRunType( "clusterization" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestHitsToTrackProcess *hitsToTrackProcess = new TRestHitsToTrackProcess( cfgFilename );
    
    run->AddProcess( hitsToTrackProcess );

    run->Start( );

    delete run;
}
