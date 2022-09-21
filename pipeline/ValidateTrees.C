#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <vector>

Int_t ValidateTrees(const char* validationFilename, const char* inputFilename = "results.root") {
    TFile* inputFile = TFile::Open(inputFilename);
    if (inputFile == nullptr) {
        cout << "Input file '" << inputFilename << " does not exist" << endl;
        exit(1);
    }
    TTree* inputTree = inputFile->Get<TTree>("bTree");
    std::vector<double>* inputObservableVector = 0;
    inputTree->SetBranchAddress("doubleObservables", &inputObservableVector);

    TFile* validationFile = TFile::Open(validationFilename);
    if (validationFile == nullptr) {
        cout << "Validation file '" << validationFilename << " does not exist" << endl;
        exit(1);
    }
    TTree* validationTree = validationFile->Get<TTree>("bTree");
    std::vector<double>* validationObservableVector = 0;
    validationTree->SetBranchAddress("doubleObservables", &validationObservableVector);

    if (inputTree->GetEntries() != validationTree->GetEntries()) {
        cout << "Number of entries is not the same!" << endl;
        cout << "result: " << inputTree->GetEntries() << endl;
        cout << "anticipated: " << validationTree->GetEntries() << endl;
        return 1;
    }

    bool obsValErr = false;
    for (int n = 0; n < inputTree->GetEntries(); n++) {
        inputTree->GetEntry(n);
        validationTree->GetEntry(n);

        if ((*inputObservableVector).size() != (*validationObservableVector).size()) {
            cout << "Vectors are different size at entry " << n << "!!" << endl;
            cout << "results.root vector size : " << (*inputObservableVector).size() << endl;
            cout << "validation.root vector size : " << (*validationObservableVector).size() << endl;

            cout << "Validation vector contents: " << endl;
            for (int m = 0; m < (*validationObservableVector).size(); m++)
                cout << (*validationObservableVector)[m] << "\t";
            cout << endl;

            cout << "Results vector contents: " << endl;
            for (int m = 0; m < (*inputObservableVector).size(); m++)
                cout << (*inputObservableVector)[m] << "\t";
            cout << endl;
            return 2;
        }

        for (int m = 0; m < (*inputObservableVector).size(); m++) {
            if (std::round(100. * (*inputObservableVector)[m]) !=
                std::round(100. * (*validationObservableVector)[m])) {
                cout << "Double Observable with index " << m << " in entry " << n
                     << " is not the same value!!" << endl;
                printf("  value: %.15f, should be: %.15f\n", (*inputObservableVector)[m],
                       (*validationObservableVector)[m]);

                if (!TMath::IsNaN((*inputObservableVector)[m] - (*validationObservableVector)[m]) &&
                    abs(((*inputObservableVector)[m] - (*validationObservableVector)[m]) /
                        (*inputObservableVector)[m]) < 1e-15) {
                    cout << "  (ignored)" << endl;
                } else {
                    obsValErr = true;
                }
            }
        }
    }
    if (obsValErr) return 3;

    cout << "Tree validation success!" << endl;
    return 0;
}
