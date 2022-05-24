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

    TRestStringOutput RESTcout;

    string a = TRestTools::Execute((string)("ls -d -1 " + namePattern));
    vector<string> b = Split(a, "\n");

    Double_t totalTime = 0;
    int cont = 0;
    for (int i = 0; i < b.size(); i++) {
        string filename = b[i];
        RESTcout << filename << RESTendl;
        cont++;
        TRestRun* run = new TRestRun();

        TFile* f = new TFile(filename.c_str());

        if (!TRestTools::fileExists(filename)) {
            RESTcout << "WARNING. Input file does not exist" << RESTendl;
            exit(1);
        }

        /////////////////////////////
        // Reading metadata classes

        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string className = key->GetClassName();
            if (className == "TRestRun") {
                RESTcout << key->GetName() << RESTendl;
                run = (TRestRun*)f->Get(key->GetName());
            }
        }

        RESTcout << "Run time (hours) : " << run->GetRunLength() / 3600. << RESTendl;
        if (run->GetRunLength() > 0) totalTime += run->GetRunLength() / 3600.;

        if (run->GetEndTimestamp() == 0 || run->GetRunLength() < 0) {
            filesNotWellClosed.push_back(filename);
        }

        delete run;

        f->Close();
    }

    if (filesNotWellClosed.size() > 0) {
        RESTcout << RESTendl;
        RESTcout << "---------------------" << RESTendl;
        RESTcout << "Files not well closed" << RESTendl;
        RESTcout << "---------------------" << RESTendl;
        for (int i = 0; i < filesNotWellClosed.size(); i++) RESTcout << filesNotWellClosed[i] << RESTendl;
    }

    RESTcout << "------------------------------" << RESTendl;
    RESTcout << "Total runs time : " << totalTime << " hours" << RESTendl;

    return 0;
}
#endif
