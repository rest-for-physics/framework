#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <vector>
Int_t Validate(TString validationFile) {
    TFile* f = new TFile("${CI_PROJECT_DIR}/install/examples/01.NLDBD/Run00001_NLDBD_Test_EvSelection.root");
    TTree* tr = (TTree*)f->Get("AnalysisTree");
      
    TFile* fV = new TFile(validationFile);
    TTree* tV = (TTree*)fV->Get("AnalysisTree");
    
    // Check number of entries.
    if (tr->GetEntries() != tV->GetEntries()) {
        cout << "Number of entries is not the same!" << endl;
        cout << "Expected: " << tV->GetEntries() << endl;
        cout << "Obtained: " << tr->GetEntries() << endl;
        return 1;
    }
    
    // Check IDs.
    for (Int_t i = 0; i < tr->GetEntries(); i++){     
        tr->GetEntry(i);
        tV->GetEntry(i);
        if (tr->GetLeaf("eventID")->GetValue(0) != tV->GetLeaf("eventID")->GetValue(0)){
            cout << "Same number of entries but different IDs" << endl;
            cout << "Found entry " << i << " with ID: " <<  tr->GetLeaf("eventID")->GetValue(0) << endl;
            cout << "Expected entry " << i << " with ID: " << tV->GetLeaf("eventID")->GetValue(0) << endl;
            return 2;
        }
    }

    cout << "Tree validation sucess!" << endl;
    return 0;
}