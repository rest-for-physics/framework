
Int_t LoadGasFromServerAndValidate() {
    gSystem->Load("libRestProcesses.so");
    gSystem->Load("libRestCore.so");
    gSystem->Load("libRestTools.so");
    gSystem->Load("libRestMetadata.so");
    gSystem->Load("libRestEvents.so");

    TRestGas* gas = new TRestGas("server", "Xenon-TMA 1Pct 10-10E3Vcm");
    gas->PrintMetadata();

    if (!gas->GasFileLoaded()) {
        cout << "The gas file was not properly loaded" << endl;
        return 10;
    }

    if (gas->GetGasMixture() != "xe_99.0-n(ch3)3_1.0") {
        cout << "Mixture : " << gas->GetGasMixture() << endl;
        cout << "Gas mixture composition is not good!" << endl;
        return 1;
    }

    if (gas->GetPressure() != 1 || gas->GetElectricField() != 0) {
        cout << "Pressure : " << gas->GetPressure() << endl;
        cout << "Field : " << gas->GetElectricField() << endl;
        cout << "Upon initialization gas pressure should be 1 and electric field should be zero" << endl;
        return 2;
    }

    gas->SetPressure(10);
    gas->SetElectricField(150);

    if (gas->GetPressure() != 10 || gas->GetElectricField() != 150) {
        cout << "Pressure : " << gas->GetPressure() << endl;
        cout << "Field : " << gas->GetElectricField() << endl;
        cout << "Pressure and field have not been properly asssigned" << endl;
        return 3;
    }

    if ((Int_t)(gas->GetDriftVelocity() * 10000) != 45873) {
        cout << "Drift velocity got a wrong validation value!" << endl;
        return 4;
    }

    if ((Int_t)(gas->GetLongitudinalDiffusion() * 10000) != 214) {
        cout << "Longitudinal diffusion got a wrong validation value!" << endl;
        return 5;
    }

    if ((Int_t)(gas->GetTransversalDiffusion() * 10000) != 140) {
        cout << "Transversal diffusion got a wrong validation value!" << endl;
        return 6;
    }

    return 0;
}
