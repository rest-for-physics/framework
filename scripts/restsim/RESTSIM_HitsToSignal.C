//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t RESTSIM_HitsToSignal( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

 //   TRestElectronDiffusionRun *run = new TRestElectronDiffusionRun();
    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    cout << "N processes : " << run->GetNumberOfProcesses() << endl;

    run->SetRunType( "segmentation" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestHitsToSignalProcess *hitsToSignalProcess = new TRestHitsToSignalProcess( cfgFilename );
    
    run->AddProcess( hitsToSignalProcess );

    run->Start( );

    delete run;
}
