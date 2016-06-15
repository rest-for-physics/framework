#include <TSystem.h>
#include <TApplication.h>
#include <TRestManager.h>

char cfgFileName[256];
char sectionName[256];
char iFile[256];

void PrintHelp( )
{
    cout << endl;
    cout << "Usage : ./restManager --c CONFIG_FILE --n SECTION_NAME --f INPUT_FILE" << endl;
    cout << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " CONFIG_FILE : RML configuration file containing at least one manager section." << endl;
    cout << " If config file is not provided the configuration file will be taken from " << endl;
    cout << " the environment variable REST_CONFIGFILE. If this last is not defined " << endl;
    cout << " the config file will be taken from REST_PATH/config/template/config.rml" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " SECTION_NAME : Name of the manager section. If not defined, the first " << endl;
    cout << " section inside the config file will be taken. " << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    cout << " INPUT_FILE : Input file name. It can be also specified from the manager " << endl;
    cout << " section using inputFile parameter. In order to give as argument here the input " << endl;
    cout << " filename it is necessary to define the parameter inputData to REST_INPUT_FILE  " << endl;
    cout << " ==================================================================================" << endl;
}

int main( int argc, char *argv[] )
{

    TApplication theApp("App", 0, 0 );

    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestEvents.so");


    if( argc <= 1 ) { PrintHelp(); exit(1); }

    if( argc >= 2 )
    {
        for(int i = 1; i < argc; i++)
            if( *argv[i] == '-')
            {
                argv[i]++;
                if( *argv[i] == '-') argv[i]++;
                {
                    switch ( *argv[i] )
                    {
                        case 'c' : sprintf( cfgFileName, "%s", argv[i+1] ); break;
                        case 'n' : sprintf( sectionName, "%s", argv[i+1] ); break;
                        case 'f' : sprintf( iFile, "%s", argv[i+1] ); break;
                        case 'h' : PrintHelp(); exit(1);
                        default : return 0;
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
            cfgFile = restPath + "/config/template/config.rml";
        }
    }

    TString inputFile = iFile;
    if( inputFile != "" )
        gSystem->Setenv( "REST_INPUTFILE", inputFile.Data() );

    cout << "Input file : " << inputFile.Data() << endl;

    TRestManager *manager = new TRestManager( cfgFile.Data(), sectionName );

    manager->ProcessEvents( );


    return 0;
}

