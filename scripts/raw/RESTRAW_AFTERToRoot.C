#include <iostream>
using namespace std;

Int_t RESTRAW_AFTERToRoot( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "template/config.rml" )
{
    cout << "Filename : " << fName << endl;

    TRestRun *run = new TRestRun( cfgFilename );

    int runNumber, runIndex;
    int size=fName.Sizeof();
    TString fN(fName(size-20,size-1));
    sscanf(fN.Data(),"RUN_%d.%d.acq",&runNumber,&runIndex);
    cout<<"Run# "<<runNumber<<" index "<<runIndex<<endl;

    run->SetRunNumber( runNumber );
    run->SetRunType( "rawSignal" );
    run->ResetRunTimes( );
    run->PrintInfo();
    
    TRestAFTERToSignalProcess *afterToSignal = new TRestAFTERToSignalProcess( );

    if(!afterToSignal->OpenInputBinFile(fName))
    {
    	cout<<"File "<<fName.Data()<<" not found"<<endl;
    	exit(0);
    }

    run->AddProcess( afterToSignal, cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );
   // run->Start( );

    delete run;
}
