{
    gErrorIgnoreLevel = kWarning;
    gROOT->ProcessLine(".x $REST_PATH/scripts/LoadRESTScripts.C");
    //gSystem->Load("$REST_PATH/lib/libRestCore.so");
    //gSystem->Load("$REST_PATH/lib/libRestEvents.so");
    gSystem->AddIncludePath(" -I$REST_PATH/source/restcore/inc");
    gSystem->AddIncludePath(" -I$REST_PATH/source/events/geant4/inc");
    gSystem->AddIncludePath(" -I$REST_PATH/source/events/general/inc");
}

