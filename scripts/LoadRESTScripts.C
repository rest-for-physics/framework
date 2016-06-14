
int LoadRESTScripts()
{
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestEvents.so");
    gSystem->Load("libRestTools.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestViewers.so");
    gSystem->Load("libRestExternal.so");

    char command[256];
    sprintf( command, "find \$REST_PATH/scripts/ |grep .C | grep -v \"LoadRESTScripts.C\" | grep -v \"swo\" | grep -v \"swp\"  | grep -v \"svn\"> /tmp/macros.list" );

    system( command );

    FILE *f = fopen( "/tmp/macros.list", "r" );

    char str[256];
    while ( fscanf ( f, "%s\n", str ) != EOF )
            {
                sprintf( str, ".L %s", str );
                gROOT->ProcessLine( str );
            }

    fclose( f );

    system( "rm /tmp/macros.list" );
}
