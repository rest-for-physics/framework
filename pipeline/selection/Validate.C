#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <vector>
Int_t Validate(TString fname) {
    TFile* f = new TFile(fname);
    TTree* tr = (TTree*)f->Get("AnalysisTree");

    tr->Scan("eventID:g4Ana_totalEdep:g4Ana_energyPrimary");

    if (tr->GetEntries() != 1) {
        cout << "Number of entries is not the same!" << endl;
        cout << "Expected: 1. Obtained: " << tr->GetEntries() << endl;
        return 1;
    }

    // Check IDs.
    std::vector<Int_t> ids = {1};
    for (Int_t i = 0; i < tr->GetEntries(); i++) {
        tr->GetEntry(i);
        if (tr->GetLeaf("eventID")->GetValue(0) != ids[i]) {
            cout << "Same number of entries but different IDs" << endl;
            cout << "Found entry " << i << " with ID: " << tr->GetLeaf("eventID")->GetValue(0) << endl;
            cout << "Expected entry " << i << " with ID: " << ids[i] << endl;
            return 2;
        }
    }

    cout << "Tree validation sucess!" << endl;
    return 0;
}
