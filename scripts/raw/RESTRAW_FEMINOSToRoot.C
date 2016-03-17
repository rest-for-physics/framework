#include <iostream>
using namespace std;

Int_t RESTRAW_FEMINOSToRoot( TString fName, Int_t firstEvent = 0, Int_t numberOfEventsToProcess = 0, char *cfgFilename = "myConfig.rml" )
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
    
    TRestFEMINOSToSignalProcess *agetToSignal = new TRestFEMINOSToSignalProcess( );

    if(!agetToSignal->OpenInputBinFile(fName))
    {
    	cout << "File " << fName.Data() << " not found" << endl;
    	exit(0);
    }

    run->AddProcess( agetToSignal,  cfgFilename );

    run->ProcessEvents( firstEvent, numberOfEventsToProcess );

    delete run;
}
