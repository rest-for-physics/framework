#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <vector>

Int_t Validate(const char* filename, const bool isReferenceGeant4Version = false) {
    TFile* file = TFile::Open(filename);
    TTree* analysisTree = file->Get<TTree>("AnalysisTree");

    analysisTree->Scan("eventID:g4Ana_totalEdep:g4Ana_energyPrimary");

    const size_t expectedNumberOfEntries = (!isReferenceGeant4Version) ? 10 : 19;

    if (analysisTree->GetEntries() != expectedNumberOfEntries) {
        cout << "Number of entries is not the same!" << endl;
        cout << "Expected: " << expectedNumberOfEntries << ". Obtained: " << analysisTree->GetEntries()
             << endl;
        return 1;
    }

    // Check IDs.
    bool success = true;
    const std::vector<size_t> ids =
        (!isReferenceGeant4Version)
            ? std::vector<size_t>{5, 16, 18, 28, 32, 40, 42, 48, 53, 68}
            : std::vector<size_t>{7, 8, 10, 13, 14, 15, 17, 19, 25, 27, 37, 44, 55, 58, 77, 84, 90, 92, 94};
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
