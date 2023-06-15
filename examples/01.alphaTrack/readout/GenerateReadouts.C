Int_t GenerateReadouts() {
    TRestDetectorReadout* readout = new TRestDetectorReadout("readouts.rml", "readout");
    TRestDetectorReadout* cathode = new TRestDetectorReadout("readouts.rml", "cathode");

    TFile* f = new TFile("readouts.root", "RECREATE");
    readout->Write("readout");
    cathode->Write("cathode");
    f->Close();

    return 0;
}
