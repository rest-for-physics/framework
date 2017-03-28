#include <TSystem.h>
#include <TRint.h>
#include <TApplication.h>
#include <TRestRun.h>
#include <TRestG4Event.h>

char iFile[256];

std::vector <TString> inputFiles;

void PrintHelp( )
{
    cout << endl;
    cout << "Usage : ./restG4PrintParticles --f INPUT_FILE" << endl;
    cout << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " INPUT_FILE : Input file name. It can be also specified from the analysisPlot " << endl;
    cout << " section using addFile key. " << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
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

    std::vector <TString> listOfParticles;
	for( unsigned int n = 0; n < inputFilesNew.size(); n++ )
    {
        cout << "Processing file : " << inputFilesNew[n] << endl;

        TString fName = inputFilesNew[n];
        TRestRun *run = new TRestRun();

        string fname = fName.Data();
        if( !run->fileExists( fname ) ) { cout << "WARNING. Input file does not exist" << endl; exit(1); }
        else run->OpenInputFile( fName );

        /////////////////////////////
        // Reading event
        TRestG4Event *ev = new TRestG4Event();

        run->SetInputEvent( ev );

        for( int n = 0; n < run->GetEntries(); n++ )
        {
            run->GetEntry ( n );

            for( int t = 0; t < ev->GetNumberOfTracks(); t++ )
            {
                Int_t notFound = 1;
                for( unsigned int i = 0; i < listOfParticles.size(); i++ )
                {
                    if( listOfParticles[i] ==  ev->GetTrack(t)->GetParticleName() )
                        notFound = 0;
                }

                if( notFound ) 
                    listOfParticles.push_back( ev->GetTrack(t)->GetParticleName() );

            }
        }
        /////////////////////////////

        delete run;
        delete ev;
    }

    for( unsigned int i = 0; i < listOfParticles.size(); i++ )
        cout << "Particle " << i << " : " << listOfParticles[i] << endl;

	return 0;
}

