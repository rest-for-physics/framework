#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TRestRun.h>


char varName[256];
char iFile[256];
Double_t start = -1;
Double_t endVal = -1;
Int_t bins = 100;

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << "This program does a gausian fit of an existing variable inside TRestAnalysisTree" << endl;
    cout << "The fit range must be given by argument" << endl;
    cout << "The number of bins of the histogram can be defined using --b. Default value : 100" << endl;
    cout << endl;
    cout << "Usage : ./restFit --v VAR_NAME --s START --e END [--B BINS] --f INPUT_FILE" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << " INPUT_FILE : Input file name. " << endl;
    cout << endl;
    cout << " You can also specify a file input range using the shell *,? characters as in ls." << endl;
    cout << " For example : \"Run_simulation_*.root\". " << endl;
    cout << endl;
    cout << " IMPORTANT : You should then write the filename range between quotes!! \"\"" << endl;
    cout << endl;
    cout << " ==================================================================================" << endl;
}

int main( int argc, char *argv[] )
{
	int argRint = 1;
	char *argVRint[3];

	char batch[64], quit[64], appName[64];
	sprintf ( appName, "restFit" );
	sprintf( batch, "%s", "-b" );
	sprintf( quit, "%s", "-q" );

	argVRint[0] = appName;
	argVRint[1] = batch;
	argVRint[2] = quit;

	for( int i = 1; i < argc; i++ )
		if ( strstr( argv[i], "--batch") != NULL  )
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
						case 'v' : sprintf( varName, "%s", argv[i+1] ); break;
						case 'B' : bins = atoi ( argv[i+1] ); break;
						case 's' : start = atof ( argv[i+1] ); break;
						case 'e' : endVal = atof ( argv[i+1] ); break;
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

    cout << "Variable name : " << varName << endl;
    cout << "Fit range : ( " << start << " , " << endVal << " ) " << endl;
    if( start == -1 || endVal == -1 )
    {
        cout << "Start or End integration values not properly defined!!!" << endl;
        PrintHelp();
        exit(1);
    }

	std::vector <TString> inputFilesNew;
	for( unsigned int n = 0; n < inputFiles.size(); n++ )
	{
		if( inputFiles[n].First( "*" ) >= 0 || inputFiles[n].First( "?" ) >= 0  )
		{
			char command[256];
			sprintf( command, "find %s > /tmp/fileList.tmp", inputFiles[n].Data() );

			system( command );

			FILE *fin = fopen( "/tmp/fileList.tmp", "r" );
			char str[256];
			while ( fscanf ( fin, "%s\n", str ) != EOF )
			{
				TString newFile = str;
				inputFilesNew.push_back( newFile );
			}
			fclose( fin );

			system ( "rm /tmp/fileList.tmp" );
		}
		else
		{
			inputFilesNew.push_back( inputFiles[n] );
		}
	}

    TH1D *h = new TH1D( "FitHisto", "FitHisto", bins, start, endVal );

	for( unsigned int n = 0; n < inputFilesNew.size(); n++ )
	{
        TRestRun *run = new TRestRun();

        run->OpenInputFile( inputFilesNew[n] );

        run->SkipEventTree();
        run->PrintInfo();

        Int_t obsID = run->GetAnalysisTree( )->GetObservableID( varName );
        for( int i = 0; i < run->GetEntries( ); i++ )
        {
            run->GetEntry(i);
            Double_t value = run->GetAnalysisTree()->GetObservableValue( obsID );
            if( value >= start && value < endVal )
            {
                h->Fill( value );
            }
        }

        delete run;
	}

    h->Fit("gaus");
    h->Draw();

	theApp.Run();

	return 0;
}

