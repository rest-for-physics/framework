//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t RESTSIGNAL_Deconvolution( TString fName, char *cfgFilename = "myConfig.rml" )
{
    cout << "Filename : " << fName << endl;
    cout << "Config file : " << cfgFilename << endl;

 //   TRestElectronDiffusionRun *run = new TRestElectronDiffusionRun();
    TRestRun *run = new TRestRun();

    // We load the information from the input file on the new run
    run->OpenInputFile( fName );

    run->SetRunType( "deconvolution" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestSignalDeconvolutionProcess *sConvProcess = new TRestSignalDeconvolutionProcess( cfgFilename );
    
    run->AddProcess( sConvProcess );

    run->Start( );

    delete run;
}
