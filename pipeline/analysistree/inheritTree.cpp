void inheritTree() {
    TFile* fin = new TFile("simpleTree.root", "open");
    TRestAnalysisTree* treein = (TRestAnalysisTree*)fin->Get("AnalysisTree");
    treein->GetEntry(0);

    TFile* fout = new TFile("inheritTree.root", "recreate");
    TRestAnalysisTree* tmp_tree = (TRestAnalysisTree*)treein->CopyTree("a<=23");
    TRestAnalysisTree* treeout = (TRestAnalysisTree*)tmp_tree->CloneTree(0);

    int a_old = 0;
    int nEntries = tmp_tree->GetEntries();
    for (int iEntry = 0; iEntry < nEntries; iEntry++) {
        tmp_tree->GetEntry(iEntry);

        double a = tmp_tree->GetObservableValue<int>("a");
        double b = a / a_old;
        a_old = a;
        cout << b << endl;
        treeout->SetObservableValue("b", b);

        any c = tmp_tree->GetObservable("vec");
        treeout->SetObservable("interitedvec", c);

        /*if(iEntry % 100 == 0)cout << sAna_PeakAmplitudeIntegral << endl;
        energy = sAna_PeakAmplitudeIntegral * gain[GetX(rA_MeanX + 95.25)][GetY(rA_MeanY + 97.125)];
*/
        treeout->Fill();
    }

    treeout->Write();
    fin->Close();
    fout->Close();
}
