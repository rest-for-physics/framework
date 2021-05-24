#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <vector>
Int_t Validate(TString fname) {
    TFile* f = new TFile(fname);
    TTree* tr = (TTree*)f->Get("AnalysisTree");

    if (tr->GetEntries() != 3) {
        cout << "Number of entries is not the same!" << endl;
        cout << "Expected: 3. Obtained: " << tr->GetEntries() << endl;
        return 1;
    }

    // Check IDs.
    std::vector<Int_t> ids = {0, 3, 6};
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
