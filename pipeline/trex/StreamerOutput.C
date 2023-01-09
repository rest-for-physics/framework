Int_t StreamerOutput(std::string fname) {
    TFile* f = TFile::Open((TString)fname);
    f->ShowStreamerInfo();

    f->Close();

    return 0;
}
