
Int_t ValidateDetectorParams(string fname) {
    TRestRun* run = new TRestRun(fname);

    TRestDetector* det = (TRestDetector*)run->GetMetadataClass("TRestDetector");

    det->PrintMetadata();

    if (det->GetDataMemberValue("fDetectorName") != "TREXDM_0") return 1;

    if (det->GetDataMemberValue("fDriftVoltage") != "160") return 2;

    if (det->GetDataMemberValue("fAmplificationVoltage") != "250") return 3;

    if (det->GetDataMemberValue("fPressure") != "6") return 4;

    if (det->GetDataMemberValue("fElectronicsSamplingTime") != "0.04") return 5;

    if (det->GetDataMemberValue("fElectronicsGain") != "0x0") return 5;

    if (det->GetDataMemberValue("fElectronicsClock") != "0x4") return 6;

    if (det->GetDataMemberValue("fElectronicsShaping") != "0xF") return 7;

    TRestDetectorHitsToSignalProcess* detSigPcs =
        (TRestDetectorHitsToSignalProcess*)run->GetMetadata("signalToHits");

    if (detSigPcs->GetDataMemberValue("fElectricField") != "960") return 8;
    if (detSigPcs->GetDataMemberValue("fDriftVelocity") != "31.9976") return 9;
    if (detSigPcs->GetDataMemberValue("fGasPressure") != "6") return 11;

    return 0;
}
