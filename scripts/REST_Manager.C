// //////////////////////////////////////////////
// REST macro.
// Author : J. Galan
// Date : 29-Apr-2016
// Description : Script to launch the RESTManager
// //////////////////////////////////////////////

Int_t REST_Manager( char *cfgFilename, TString inputFile = "" )
{
    // We assume here that REST_INPUTFILE is defined in the manager configuration file
    // So that we use it to define the input file
    // If we do not provide an input file it will be used the one defined in the configuration
    if( inputFile != "" )
        gSystem->Setenv( "REST_INPUTFILE", inputFile.Data() );

    TRestManager *manager = new TRestManager( cfgFilename );

    manager->ProcessEvents( );

    delete manager;

    return 0;
   
}
