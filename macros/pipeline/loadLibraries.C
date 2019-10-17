
Int_t loadLibraries() {
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestTools.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestEvents.so");
    return 0;
}
