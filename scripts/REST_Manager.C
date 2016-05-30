// //////////////////////////////////////////////
// REST macro.
// Author : J. Galan
// Date : 29-Apr-2016
// Description : Script to launch the RESTManager
// //////////////////////////////////////////////

Int_t REST_Manager(TString inputFile = "", char *managerSectionName = "", string cfgFilename = "" )
{
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestEvents.so");
    gSystem->Load("libRestTools.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestViewers.so");
    gSystem->Load("libRestExternal.so");

    // We assume here that REST_INPUTFILE is defined in the manager configuration file
    // So that we use it to define the input file
    // If we do not provide an input file it will be used the one defined in the configuration
    TString cfgFile = cfgFilename;
    if( cfgFile == "" )
    {
        cfgFile = gSystem->Getenv( "REST_CONFIGFILE" );

        if( cfgFile == "" )
        {
            TString restPath = gSystem->Getenv( "REST_PATH" );
            cfgFile = restPath + "/config/template/config.rml";
        }
    }

    if( inputFile != "" )
        gSystem->Setenv( "REST_INPUTFILE", inputFile.Data() );

    TRestManager *manager = new TRestManager( cfgFile.Data(), managerSectionName );

    manager->ProcessEvents( );

    delete manager;

    return 0;
   
}
