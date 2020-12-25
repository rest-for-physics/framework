void validate() {
    TFile* f = new TFile("inheritTree.root", "open");
    TRestAnalysisTree* tree = (TRestAnalysisTree*)f->Get("AnalysisTree");
    if (tree->GetEntries() != 24) {
        cout << "Entry is not 24!" << endl;
        return 1;
    }

    tree->GetEntry(3);
    if (tree->GetObservableValue<double>("b") != 1.5) {
        cout << "branch \"b\" in entry 3 is not 1.5!" << endl;
        return 2;
    }

}
