#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRestRun.h>
#include <TRestG4Metadata.h>

TRestEvent *ev;

char varName[256];
char iFile[256];
Double_t start = 0;
Double_t endVal = 0;

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " This program will visualize the events inside a REST file using the TRestEvent::Draw" << endl;
    cout << " method implemented in each specific event data type." << endl;
    cout << endl;
    cout << "Usage : ./restDrawEvents --f INPUT_FILE" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << " INPUT_FILE : Input file name. " << endl;
    cout << endl;
    cout << " ==================================================================================" << endl;
}

int main( int argc, char *argv[] )
{
	int argRint = 1;
	char *argVRint[3];

	char batch[64], quit[64], appName[64];
	sprintf ( appName, "restDrawEvents" );
	sprintf( batch, "%s", "" );
	sprintf( quit, "%s", "-q" );

	argVRint[0] = appName;
	argVRint[1] = batch;
	argVRint[2] = quit;

    // We only run in batch mode
    argRint = 3;

	TRint theApp("App", &argRint, argVRint );

	gSystem->Load("libRestCore.so");
	gSystem->Load("libRestMetadata.so");
	gSystem->Load("libRestEvents.so");
	gSystem->Load("libRestProcesses.so");

	if( argc <= 1 ) { PrintHelp(); exit(1); }

	if( argc >= 2 )
	{
		for(int i = 1; i < argc; i++)
			if( *argv[i] == '-')
			{
				argv[i]++;
				if( *argv[i] == '-')
				{
					argv[i]++;
					switch ( *argv[i] )
					{
						case 'f' : 
							   {
								   sprintf( iFile, "%s", argv[i+1] );
								   TString iFileStr = iFile;
								   inputFiles.push_back( iFileStr );
								   break;
							   }
						case 'h' : PrintHelp(); exit(1);
						default : ;
					}
				}
			}
	}

    TCanvas *canvas = new TCanvas( "EventCanvas", "EventCanvas", 600, 400 );

    // We do only the first file in the list
    if( inputFiles.size() > 0 ) 
	{
        TRestRun *run = new TRestRun();

        run->OpenInputFile( inputFiles[0] );

        TClass *cl = TClass::GetClass( run->GetInputEventName() );
        TRestEvent *ev = (TRestEvent *) cl->New();
        run->SetInputEvent( ev );

        for( int i = 0; i < run->GetEntries( ); i++ )
        {
            run->GetEntry(i);
            ev->PrintEvent();
            canvas->cd();
            ev->DrawEvent();
            canvas->Update();
            run->GetAnalysisTree()->PrintObservables();
            cout << "Press a KEY to print next event ... " << endl;
            cout << "Press q to exit " << endl;

            char key[1];
            scanf( "%c", key );

            if( key[0] == 'q' ) return 0;
        }

        delete run;
	}

	theApp.Run();

    cout << "END" << endl;

	return 0;
}

