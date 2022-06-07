#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <vector>

Int_t Validate(const char* filename) {
    TFile* file = TFile::Open(filename);
    TTree* analysisTree = file->Get<TTree>("AnalysisTree");

    analysisTree->Scan("eventID:g4Ana_totalEdep:g4Ana_energyPrimary");

    constexpr size_t expectedNumberOfEntries = 16;
    if (analysisTree->GetEntries() != expectedNumberOfEntries) {
        cout << "Number of entries is not the same!" << endl;
        cout << "Expected: " << expectedNumberOfEntries << ". Obtained: " << analysisTree->GetEntries()
             << endl;
        return 1;
    }

    // Check IDs.
    bool success = true;
    std::vector<Int_t> ids = {1};
    for (Int_t i = 0; i < analysisTree->GetEntries(); i++) {
        analysisTree->GetEntry(i);
        if (analysisTree->GetLeaf("eventID")->GetValue(0) != ids[i]) {
            cout << "Same number of entries but different IDs" << endl;
            cout << "Found entry " << i << " with ID: " << analysisTree->GetLeaf("eventID")->GetValue(0)
                 << endl;
            cout << "Expected entry " << i << " with ID: " << ids[i] << endl;
            success = false;
        }
    }
    if (!success) {
        return 2;
    }

    cout << "Tree validation success!" << endl;
    return 0;
}
