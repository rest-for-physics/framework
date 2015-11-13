
int LoadRESTScripts()
{
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestEvents.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestViewers.so");

    char command[256];
    sprintf( command, "find \$REST_PATH/scripts/ |grep .C | grep -v \"LoadRESTScripts.C\" | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"> macros.list" );

    system( command );

    FILE *f = fopen( "macros.list", "r" );

    char str[256];
    while ( fscanf ( f, "%s\n", str ) != EOF )
            {
                sprintf( str, ".L %s", str );
                gROOT->ProcessLine( str );
            }

    fclose( f );

    system( "rm macros.list" );
}
