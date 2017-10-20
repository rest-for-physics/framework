
int LoadRESTScripts()
{
    gSystem->Load("libGdml.so");
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestEvents.so");
    gSystem->Load("libRestMetadata.so");
    vector <TString> list = TRestTools::GetListOfRESTLibraries( );
    for( unsigned int n = 0; n < list.size(); n++ )
    {
        cout << "Loading library : " << list[n] << endl;
        gSystem->Load( list[n] );
    }

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
