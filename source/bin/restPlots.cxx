#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TRestAnalysisPlot.h>

char cfgFileName[256];
char sectionName[256];
TString saveToPDFFile = "";
TString saveToROOTFile = "";
char iFile[256];

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << endl;
    cout << "Usage : ./restPlots --c CONFIG_FILE [--n SECTION_NAME] --f INPUT_FILE [--r HISTOS_FILE.root] [--p PDF_PLOTS.pdf]" << endl;
    cout << endl;
    cout << " Fields between [] are optional. " << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " CONFIG_FILE : RML configuration file containing at least one analysisPlot section." << endl;
    cout << " If config file is not provided the configuration file will be taken from " << endl;
    cout << " the environment variable REST_CONFIGFILE. If this last is not defined " << endl;
    cout << " the config file will be taken from REST_PATH/config/template/plots.rml" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " SECTION_NAME : Name of the manager section. If not defined, the first " << endl;
    cout << " section inside the config file will be taken. " << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " INPUT_FILE : Input file name. It can be also specified from the analysisPlot " << endl;
    cout << " section using addFile key. " << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " HISTOS_FILE : The ROOT filename where histograms will be saved. " << endl;
    cout << " Histograms are by default saved at /tmp/histos.root. " << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " PDF_PLOTS : The PDF filename where the generated canvas will be saved. " << endl;
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
	sprintf ( appName, "restPlots" );
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
	//				printf( "arg : %s\n", argv[i+1] );
					switch ( *argv[i] )
					{
						case 'c' : sprintf( cfgFileName, "%s", argv[i+1] ); break;
						case 'n' : sprintf( sectionName, "%s", argv[i+1] ); break;
						case 'p' : saveToPDFFile = argv[i+1]; break;
						case 'r' : saveToROOTFile = argv[i+1]; break;
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

	TString cfgFile = cfgFileName;
	if( cfgFile == "" )
	{
		cfgFile = getenv( "REST_CONFIGFILE" );

		if( cfgFile == "" )
		{
			TString restPath = getenv( "REST_PATH" );
			cfgFile = restPath + "/config/template/plots.rml";
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

	TRestAnalysisPlot *anPlot = new TRestAnalysisPlot( );
	anPlot->LoadConfigFromFile(cfgFileName);
	if( saveToPDFFile != "" )
		anPlot->SetOutputPlotsFilename( saveToPDFFile );

	if( saveToROOTFile  != "" )
		anPlot->SetOutputHistosFilename( saveToROOTFile );

	for( unsigned int n = 0; n < inputFilesNew.size(); n++ )
	{
		cout << "Adding file : " << inputFilesNew[n] << endl;
		anPlot->AddFile( inputFilesNew[n] );
	}

	anPlot->PlotCombinedCanvas( );

	theApp.Run();

	return 0;
}

