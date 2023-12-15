#include <filesystem>
#include <iostream>
#include <vector>

#include "TGeoManager.h"
#include "TRestTask.h"
#include "TSystem.h"
using namespace std;
namespace fs = std::filesystem;

#ifndef RESTTask_CheckValidRuns
#define RESTTask_CheckValidRuns

//*******************************************************************************************************
//***
//*** Description: This macro will identify run files that were not properly closed.
//***
//*** --------------
//*** The first and mandatory argument must provide a full data and pattern to filter the files that
//*** will be checked. E.g. to add all the Hits files from run number 123 the first argument would be
//*** pattern = "/path/to/data/Run00123*Hits*root".
//***
//*** IMPORTANT: The pattern must be given using double quotes ""
//***
//*** --------------
//*** Usage: restManager CheckValidRuns "/full/path/file_*pattern*.root" [purge]
//*** --------------
//***
//*** An optional parameter `purge` can be made true. In that case, this macro will not just provide
//*** a list of the files not properly closed but it will also remove them!
//***
//*** The following command will remove the non-valid runs
//*** --------------
//*** Usage: restManager CheckValidRuns "/full/path/file_*pattern*.root" 1
//*** --------------
//***
//*** CAUTION: Be aware that any non-REST file in the list will be removed if you use purge=1
//***
//*******************************************************************************************************
Int_t REST_CheckValidRuns(TString namePattern, Bool_t purge = false) {
    TGeoManager::SetVerboseLevel(0);

    vector<std::string> filesNotWellClosed;

    TRestStringOutput RESTLog;

    string a = TRestTools::Execute((string)("ls -d -1 " + namePattern));
    vector<string> b = Split(a, "\n");

    Double_t totalTime = 0;
    int cont = 0;
    for (int i = 0; i < b.size(); i++) {
        string filename = b[i];
        RESTLog << filename << RESTendl;
        cont++;
        TRestRun* run = new TRestRun();

        TFile* f = new TFile(filename.c_str());

        if (!TRestTools::fileExists(filename)) {
            RESTLog << "WARNING. Input file does not exist" << RESTendl;
            exit(1);
        }

        /////////////////////////////
        // Reading metadata classes

        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string className = key->GetClassName();
            if (className == "TRestRun") {
                RESTLog << key->GetName() << RESTendl;
                run = (TRestRun*)f->Get(key->GetName());
            }
        }

        RESTLog << "Run time (hours) : " << run->GetRunLength() / 3600. << RESTendl;
        if (run->GetRunLength() > 0) totalTime += run->GetRunLength() / 3600.;

        if (run->GetEndTimestamp() == 0 || run->GetRunLength() < 0) {
            filesNotWellClosed.push_back(filename);
        }

        delete run;

        f->Close();
    }

    if (filesNotWellClosed.size() > 0) {
        RESTLog << RESTendl;
        RESTLog << "---------------------" << RESTendl;
        RESTLog << "Files not well closed" << RESTendl;
        RESTLog << "---------------------" << RESTendl;
        for (int i = 0; i < filesNotWellClosed.size(); i++) {
            RESTLog << filesNotWellClosed[i] << RESTendl;
            if (purge) fs::remove(filesNotWellClosed[i]);
        }

        if (purge) {
            RESTLog << "---------------------------" << RESTendl;
            RESTLog << "The files have been removed" << RESTendl;
            RESTLog << "---------------------------" << RESTendl;
        }
    }

    RESTLog << "------------------------------" << RESTendl;
    RESTLog << "Total runs time : " << totalTime << " hours" << RESTendl;

    return 0;
}
#endif
