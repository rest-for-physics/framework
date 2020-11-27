#include <iostream>
#include <vector>
#include "TGeoManager.h"
#include "TRestTask.h"
#include "TSystem.h"
using namespace std;

#ifndef RESTTask_CheckRunFileList
#define RESTTask_CheckRunFileList

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_CheckRunFileList(TString namePattern, Int_t N = 100000) {
    TGeoManager::SetVerboseLevel(0);

    vector<TString> filesNotWellClosed;

    TRestStringOutput cout;

    string a = TRestTools::Execute((string)("ls -d -1 " + namePattern));
    vector<string> b = Split(a, "\n");

    Double_t totalTime = 0;
    int cont = 0;
    for (int i = 0; i < b.size(); i++) {
        string filename = b[i];
        cout << filename << endl;
        cont++;
        TRestRun* run = new TRestRun();

        TFile* f = new TFile(filename.c_str());

        if (!TRestTools::fileExists(filename)) {
            cout << "WARNING. Input file does not exist" << endl;
            exit(1);
        }

        /////////////////////////////
        // Reading metadata classes

        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string className = key->GetClassName();
            if (className == "TRestRun") {
                cout << key->GetName() << endl;
                run = (TRestRun*)f->Get(key->GetName());
            }
        }

        cout << "Run time (hours) : " << run->GetRunLength() / 3600. << endl;
        if (run->GetRunLength() > 0) totalTime += run->GetRunLength() / 3600.;

        if (run->GetEndTimestamp() == 0 || run->GetRunLength() < 0) {
            filesNotWellClosed.push_back(filename);
        }

        delete run;

        f->Close();
    }

    if (filesNotWellClosed.size() > 0) {
        cout << endl;
        cout << "---------------------" << endl;
        cout << "Files not well closed" << endl;
        cout << "---------------------" << endl;
        for (int i = 0; i < filesNotWellClosed.size(); i++) cout << filesNotWellClosed[i] << endl;
    }

    cout << "------------------------------" << endl;
    cout << "Total runs time : " << totalTime << " hours" << endl;

    return 0;
}
#endif
