#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>

#include <TRestRun.h>
#include <TRestG4Event.h>


char varName[256];
char iFile[256];
Double_t start = 0;
Double_t endVal = 0;

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << "This program prints on screen the existing variables inside TRestAnalysisTree" << endl;
    cout << endl;
    cout << "Usage : ./restPrintAnaTreeVariables INPUT_FILE" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << endl;
}

int main( int argc, char *argv[] )
{
	int argRint = 1;
	char *argVRint[3];

	char batch[64], quit[64], appName[64];
	sprintf ( appName, "restPrintAnaTreeVariables" );
	sprintf( batch, "%s", "-b" );
	sprintf( quit, "%s", "-q" );

	argVRint[0] = appName;
	argVRint[1] = batch;
	argVRint[2] = quit;

    argRint = 3;

	TRint theApp("App", &argRint, argVRint );

	gSystem->Load("libRestCore.so");
	gSystem->Load("libRestMetadata.so");
	gSystem->Load("libRestEvents.so");
	gSystem->Load("libRestProcesses.so");

	if( argc <= 1 || argc > 2 ) 
    {
        cout << "REST Error : Wrong number of arguments given" << endl;
        PrintHelp(); 
        exit(1); 
    }

    sprintf( iFile, "%s", argv[1] );

    TRestRun *run = new TRestRun();

    run->OpenInputFile( iFile );

    TRestAnalysisTree *anaTree = run->GetAnalysisTree();

    cout << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestAnalysis tree observables description" << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << endl;

 //   run->SkipEventTree();

   TRestG4Event *ev = new TRestG4Event();

	run->SetInputEvent( ev );
    for( int i = 0; i < anaTree->GetNumberOfObservables( ); i++ )
    {
        if( anaTree->GetObservableDescription( i ) != "Not defined" && anaTree->GetObservableDescription( i ) != "" )
        {
            cout << " - " << i << ". " << anaTree->GetObservableName( i ) << endl;
            cout << endl;
            cout << "    Description : " << anaTree->GetObservableDescription( i ) << endl;
            cout << endl;
        }
    }

	Int_t id = anaTree->GetObservableID( "g4Ana.photoelectric" );
/*
	for( int n = 0; run->GetEntries(); n++ )
	{
		//run->GetEntry(n);
		cout << "Event id : " << anaTree->GetEventID() << " value : " << anaTree->GetObservableValue( id ) << endl;
	}
*/

    cout << endl;

    cout << endl;
    cout << "++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestAnalysis tree observable index" << endl;
    cout << "++++++++++++++++++++++++++++++++++++++" << endl;
    cout << endl;

    for( int i = 0; i < anaTree->GetNumberOfObservables( ); i++ )
    {
        cout << " - " << i << ". " << anaTree->GetObservableName( i ) << endl;
    }
    cout << endl;

    cout << "----------------------------" << endl;
    cout << endl;


	return 0;
}
