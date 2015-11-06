//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;




Int_t RESTRAW_AGETToRoot( TString fName, TString cfgFilename="myConfig.rml" )
{
    cout << "Filename : " << fName << endl;

 //   TRestElectronDiffusionRun *run = new TRestElectronDiffusionRun();
    TRestRun *run = new TRestRun(cfgFilename.Data());

    // We load the information from the input file on the new run
    //run->OpenInputFile( fName );

    run->SetRunType( "analysis" );
    run->ResetRunTimes( );

    run->PrintInfo();
    
    TRestAGETToSignalProcess *agetToSignal = new TRestAGETToSignalProcess( );
    	if(!agetToSignal->OpenInputBinFile(fName)){
    	cout<<"File "<<fName.Data()<<" not found"<<endl;
    	exit(0);
    	}
    run->AddProcess( agetToSignal );

    run->Start( );

    delete run;
}
