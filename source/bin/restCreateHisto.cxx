#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRestRun.h>


char varName[256];
char iFile[256];
char rootFileName[256];
char histoName[256];

int startVal = 0;
int endVal = 1000;
int bins = 1000;

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " This program will produce a histogram using VAR_NAME, " << endl;
    cout << " which should be defined inside TRestAnalysisTree" << endl;
    cout << endl;
    cout << " The histogram will be added to an existing root file (or will be created)" << endl;
    cout << endl;
    cout << " Usage : ./restCreateHisto --v VAR_NAME --o ROOT_FILE --n HISTO_NAME --b BINS --s START --e END --f INPUT_FILEs" << endl;
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
	sprintf ( appName, "restCreateHisto" );
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

    sprintf( rootFileName, "%s", "default.root" );
    sprintf( histoName, "%s", "default" );

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
                        case 'o' : sprintf( rootFileName, "%s", argv[i+1] ); break;
                        case 'n' : sprintf( histoName, "%s", argv[i+1] ); break;
                        case 's' : startVal = atof( argv[i+1] ); break;
                        case 'e' : endVal = atof( argv[i+1] ); break;
                        case 'b' : bins = atoi( argv[i+1] ); break;
						case 'v' : sprintf( varName, "%s", argv[i+1] ); break;
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

    TH1D *h = new TH1D( histoName, histoName, bins, startVal, endVal );

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
            Double_t val = run->GetAnalysisTree()->GetObservableValue( obsID );
            if( val >= startVal && val <= endVal )
                h->Fill(val);
        }

        delete run;
	}

    TFile *f = new TFile( rootFileName, "update" );
    h->Write( histoName );
    f->Close();

    cout << "Written histogram " << histoName << " into " << rootFileName << endl;

	theApp.Run();

	return 0;
}

