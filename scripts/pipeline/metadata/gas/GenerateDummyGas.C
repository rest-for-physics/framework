// A basic test to generate a gas file and test uploading it to the gas server

Int_t GenerateDummyGas() {
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestTools.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestEvents.so");

    TRestTools::Execute("scp -P 22 gasUser@sultan.unizar.es:gasFiles/gases.rml gases.bkp");

    if (TRestTools::isValidFile("gases.bkp") == 0) {
        cout << "GenerateDummyGas validation script. Failed to copy gases.rml file locally" << endl;
        return 1;
    }

    // Launching gas generation in test mode
    TRestGas* gas = new TRestGas("dummy.rml", "dummyGas", true, true);

    if (!gas->GasFileLoaded()) {
        cout << "The gas file was not properly loaded" << endl;
        return 10;
    }

    if (!TRestTools::isValidFile("xe_0.1-n(ch3)3_99.0-E_vs_P_50.0_100.0_nodes_02-nCol_03-maxE_400.gas")) {
        cout << "Problem with the generation of gas file locally" << endl;
        return 20;
    } else {
        TRestTools::Execute("rm xe_0.1-n(ch3)3_99.0-E_vs_P_50.0_100.0_nodes_02-nCol_03-maxE_400.gas");
    }

    cout << "Trying to load the generated dummyGas from server" << endl;
    cout << "-----" << endl;
    // Checking if now we are able to read the gas from server
    TRestGas* gas2 = new TRestGas("server", "dummyGas");
    gas2->PrintMetadata();

    if (!gas2->GasFileLoaded()) {
        cout << "The gas file was not properly loaded from server" << endl;
        return 30;
    }

    // Everything went fine. Placing back the original gases.rml
    TRestTools::Execute("scp -P 22 gases.bkp gasUser@sultan.unizar.es:gasFiles/gases.rml");

    return 0;
}
