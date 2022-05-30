#include <TApplication.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTools.h>
#include <TRint.h>
#include <TSystem.h>

#include "TRestTask.h"

#ifndef RESTTask_PrintFileContents
#define RESTTask_PrintFileContents

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_FileContents(TString fName) {
    TRestStringOutput RESTfout;
    TFile* fFile = new TFile(fName);
    if (fFile == NULL) {
        RESTfout << "I could not open file : " << fName << RESTendl;
        return 0;
    }
    TIter nextkey(fFile->GetListOfKeys());
    TKey* key;
    Int_t c = 0;
    RESTfout << "==========================================" << RESTendl;
    while ((key = (TKey*)nextkey()) != NULL) {
        if (c > 0) RESTfout << " ------------------------------------- " << RESTendl;

        RESTfout << "ClassName : " << key->GetClassName() << RESTendl;
        RESTfout << "Name : " << key->GetName() << RESTendl;
        RESTfout << "Title : " << key->GetTitle() << RESTendl;

        c++;
    }
    RESTfout << "==========================================" << RESTendl;

    return 0;
}

#endif
