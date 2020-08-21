// This macro will extract the double observables and will fill a tree with std::vector<double>
Int_t MakeBasicTree(string fname) {
    TRestRun* run = new TRestRun(fname);

    TRestAnalysisTree* aTree = run->GetAnalysisTree();

    std::vector<double> obsValues;
    for (int n = 0; n < aTree->GetNumberOfObservables(); n++) {
        if (aTree->GetObservableType(n) == "double") obsValues.push_back(0);
    }

    TFile* outFile = new TFile("results.root", "RECREATE");
    TTree* myTree = new TTree("bTree", "basicTree");

    myTree->Branch("doubleObservables", &obsValues);

    for (int n = 0; n < run->GetEntries(); n++) {
        run->GetEntry(n);

        obsValues.clear();
        for (int m = 0; m < aTree->GetNumberOfObservables(); m++) {
            if (aTree->GetObservableType(m) == "double") {
                obsValues.push_back(aTree->GetObservableValue<Double_t>(m));
            }
        }
        myTree->Fill();
    }

    myTree->Write();
    outFile->Close();
    return 0;
}
