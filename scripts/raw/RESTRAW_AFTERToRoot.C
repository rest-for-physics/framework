//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;

Int_t RESTRAW_AFTERToRoot( TString fName, TString cfgFilename="myConfig.rml" )
{
    cout << "Filename : " << fName << endl;

 //   TRestElectronDiffusionRun *run = new TRestElectronDiffusionRun();
    TRestRun *run = new TRestRun(cfgFilename.Data());

    // We load the information from the input file on the new run
    //run->OpenInputFile( fName );

    run->SetRunType( "rawSignal" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestAFTERToSignalProcess *afterToSignal = new TRestAFTERToSignalProcess( );
    	if(!afterToSignal->OpenInputBinFile(fName)){
    	cout<<"File "<<fName.Data()<<" not found"<<endl;
    	exit(0);
    	}
    run->AddProcess( afterToSignal );

    run->Start( );

    delete run;
}
