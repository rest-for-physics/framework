#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <vector>
Int_t ValidateTrees() {
    TFile* f = new TFile("results.root");
    TTree* tr = (TTree*)f->Get("bTree");
    std::vector<double>* vr = 0;
    tr->SetBranchAddress("doubleObservables", &vr);

    TFile* fV = new TFile("validation.root");
    TTree* tV = (TTree*)fV->Get("bTree");
    std::vector<double>* vV = 0;
    tV->SetBranchAddress("doubleObservables", &vV);

    if (tr->GetEntries() != tV->GetEntries()) {
        cout << "Number of entries is not the same!" << endl;
        cout << "result: " << tr->GetEntries() << endl;
        cout << "anticipated: " << tV->GetEntries() << endl;
        return 1;
    }

    for (int n = 0; n < tr->GetEntries(); n++) {
        tr->GetEntry(n);
        tV->GetEntry(n);

        if ((*vr).size() != (*vV).size()) {
            cout << "Vectors are different size at entry " << n << "!!" << endl;
            cout << "results.root vector size : " << (*vr).size() << endl;
            cout << "validation.root vector size : " << (*vV).size() << endl;

            cout << "Validation vector contents: " << endl;
            for (int m = 0; m < (*vV).size(); m++) cout << (*vV)[m] << "\t";
            cout << endl;

            cout << "Results vector contents: " << endl;
            for (int m = 0; m < (*vr).size(); m++) cout << (*vr)[m] << "\t";
            cout << endl;
            return 2;
        }

        for (int m = 0; m < (*vr).size(); m++) {
            if ((*vr)[m] != (*vV)[m]) {
                cout << "Double Observable with index " << m << " in entry " << n
                     << " is not the same value!!" << endl;
                return 3;
            }
        }
    }

    cout << "Tree validation sucess!" << endl;
    return 0;
}
