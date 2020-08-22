
Int_t ValidateG4(string fname) {
    TRestRun* run = new TRestRun(fname);

    if (run->GetParentRunNumber() != 0) {
        cout << "Parent run number value : " << run->GetParentRunNumber() << endl;
        cout << "The parent run number from restG4 generated file should be 0" << endl;
        return 1;
    }

    if (run->GetRunNumber() != 1) {
        cout << "Run number value : " << run->GetRunNumber() << endl;
        cout << "The run number on the validation chain should be 1 by default!" << endl;
        return 2;
    }

    if (run->GetRunType() != "restG4") {
        cout << "Run type : " << run->GetRunType() << endl;
        cout << "The run type of restG4 generated data should be 'restG4'!" << endl;
        return 3;
    }

    if (run->GetRunTag() != "SimulationofXe136bb0ndecayfromgas.") {
        cout << "Run tag : " << run->GetRunTag() << endl;
        cout << "The run tag of the basic validation test should be 'NLDBD'!" << endl;
        return 4;
    }

    if (run->GetEntries() != 500) {
        cout << "Run entries : " << run->GetEntries() << endl;
        cout << "The NLDBD simulation is launched from gas. It should always generate 50000 events." << endl;
        return 5;
    }

    cout << "Testing reading of Geant4 metadata class" << endl;
    TRestG4Metadata* g4Md = (TRestG4Metadata*)run->GetMetadataClass("TRestG4Metadata");
    if (!g4Md) {
        cout << "Problem reading Geant4 metadata class!" << endl;
        return 6;
    }
    g4Md->PrintMetadata();

    if (g4Md->GetNumberOfActiveVolumes() != 1) {
        cout << "The number of registered volumes is not 1!" << endl;
        return 7;
    }

    // Other tests like opening other metadata classes. Detector TGeoManager, etc.

    cout << "G4 file good" << endl;
    return 0;
}
