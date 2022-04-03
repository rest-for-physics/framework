// This macro will extract the double observables and will fill a tree with std::vector<double>
Int_t MakeBasicTree(const char* inputFilename, const char* outputFilename = "results.root",
                    Bool_t check = true) {
    TRestRun run(inputFilename);

    TRestAnalysisTree* aTree = run.GetAnalysisTree();

    std::vector<double> obsValues;
    for (int n = 0; n < aTree->GetNumberOfObservables(); n++) {
        if (aTree->GetObservableType(n) == "double") obsValues.push_back(0);
    }

    TFile* outFile = new TFile(outputFilename, "RECREATE");
    TTree* myTree = new TTree("bTree", "basicTree");

    myTree->Branch("doubleObservables", &obsValues);

    for (int n = 0; n < run.GetEntries(); n++) {
        run.GetEntry(n);
        // cout << "Entry : " << n << endl;

        obsValues.clear();
        for (int m = 0; m < aTree->GetNumberOfObservables(); m++) {
            if (aTree->GetObservableType(m) == "double") {
                if (n == 0) {
                    cout << "index: " << obsValues.size() << ", obsName: " << aTree->GetObservableName(m)
                         << " value: " << aTree->GetObservableValue<Double_t>(m) << endl;
                }
                obsValues.push_back(aTree->GetObservableValue<Double_t>(m));
            }
        }
        myTree->Fill();
    }

    // check all trees have the same number of elements
    if (run.GetEntries() != aTree->GetEntries() || run.GetEntries() != myTree->GetEntries()) {
        cout << "ERROR: mismatch in number of tree entries" << endl;
        return 1;
    }
    myTree->Write();
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

    return 0;
}
