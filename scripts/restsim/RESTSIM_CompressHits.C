//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t RESTSIM_CompressHits( TString fName, char *cfgFilename )
{
    cout << "Filename : " << fName << endl;

 //   TRestElectronDiffusionRun *run = new TRestElectronDiffusionRun();
    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    cout << "N processes : " << run->GetNumberOfProcesses() << endl;

    run->SetRunType( "compressed" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestCompressHitsProcess *compressHitsProcess = new TRestCompressHitsProcess( cfgFilename );
    
    run->AddProcess( compressHitsProcess );

    run->Start( );

    delete run;
}
