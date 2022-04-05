// This macro will extract the double observables and will fill a tree with std::vector<double>
Int_t MakeBasicTree(const char* inputFilename, const char* outputFilename = "results.root",
                    Bool_t check = true) {
    TRestRun run(inputFilename);

    TRestAnalysisTree* analysisTree = run.GetAnalysisTree();

    std::vector<double> obsValues;
    for (int n = 0; n < analysisTree->GetNumberOfObservables(); n++) {
        if (analysisTree->GetObservableType(n) == "double") obsValues.push_back(0);
    }

    TFile* outFile = new TFile(outputFilename, "RECREATE");
    TTree* outTree = new TTree("bTree", "basicTree");

    outTree->Branch("doubleObservables", &obsValues);

    for (int n = 0; n < run.GetEntries(); n++) {
        run.GetEntry(n);
        // cout << "Entry : " << n << endl;

        obsValues.clear();
        for (int m = 0; m < analysisTree->GetNumberOfObservables(); m++) {
            if (analysisTree->GetObservableType(m) == "double") {
                if (n == 0) {
                    cout << "index: " << obsValues.size()
                         << ", obsName: " << analysisTree->GetObservableName(m)
                         << " value: " << analysisTree->GetObservableValue<Double_t>(m) << endl;
                }
                obsValues.push_back(analysisTree->GetObservableValue<Double_t>(m));
            }
        }
        outTree->Fill();
    }

    // check all trees have the same number of elements
    if (run.GetEntries() != analysisTree->GetEntries() || run.GetEntries() != outTree->GetEntries()) {
        cout << "ERROR: mismatch in number of tree entries" << endl;
        return 1;
    }
    outTree->Write();
    outFile->Close();

    // check output file tree has the correct number of entries
    if (check) {
        TFile outFileCheck(outputFilename, "READ");
        TTree* outTreeCheck = outFileCheck.Get<TTree>("bTree");
        if (outTreeCheck == nullptr) {
            cout << "ERROR: tree was not correctly written into file" << endl;
            return 1;
        }

        if (outTreeCheck->GetEntries() != run.GetEntries()) {
            cout << "ERROR: output file check - mismatch in number of tree entries" << endl;
            return 1;
        }
    }

    if (run.GetEntries() == 0) {
        cout << "ERROR: Macro finished without issues but number of final entries is 0" << endl;
        return 1;
    }

    cout << "Finished `MakeBasicTree.C` macro. Number of entries: " << run.GetEntries() << endl;

    return 0;
}
