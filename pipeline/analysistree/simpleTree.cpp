void simpleTree() {
    TFile* f = new TFile("simpleTree.root", "recreate");
    TRestAnalysisTree* tree = new TRestAnalysisTree();

    for (int i = 0; i < 100; i++) {
        tree->SetObservableValue("a", i);
        tree->SetObservableValue("vec", vector<int>{i, i * 10});
        tree->SetObservableValue("rand", gRandom->Gaus(1, 2));
        tree->Fill();
    }
    tree->Write();
    f->Close();
}
