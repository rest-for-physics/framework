//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t RESTSIM_ElectronDiffusion( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

 //   TRestElectronDiffusionRun *run = new TRestElectronDiffusionRun();
    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    run->SetRunType( "diffusion" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestElectronDiffusionProcess *eDiffProcess = new TRestElectronDiffusionProcess( cfgFilename );
    
    run->AddProcess( eDiffProcess );

    run->Start( );

    delete run;
}
