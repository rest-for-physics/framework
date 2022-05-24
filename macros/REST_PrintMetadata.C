#include "TRestTask.h"

#ifndef RESTTask_PrintMetadata
#define RESTTask_PrintMetadata

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Metadata(TString fName, TString objName = "") {
    TRestStringOutput RESTcout;
    RESTcout.setorientation(TRestStringOutput::REST_Display_Orientation::kLeft);

    TString fileName = fName;

    RESTcout << "Filename : " << fileName << RESTendl;

    string fname = fileName.Data();

    if (!TRestTools::fileExists(fname)) {
        RESTcout << "WARNING. Input file does not exist" << RESTendl;
        exit(1);
    }

    TFile* f = new TFile(fileName);

    TIter nextkey(f->GetListOfKeys());
    TKey* key;
    int n = 0;
    while ((key = (TKey*)nextkey())) {
        if (((string)(key->GetClassName())).find("TRest") != -1) {
            TObject* obj = f->Get(key->GetName());
            if (obj == NULL) {
                RESTcout << "Cannot Get object with name \"" << key->GetName() << "\"!" << RESTendl;
                RESTcout << "The name may contain illegel characters which was legel in previous version."
                         << RESTendl;
            } else if (obj->InheritsFrom("TRestMetadata")) {
                if (objName == "" || objName == obj->ClassName() || objName == obj->GetName()) {
                    RESTcout << n << "th object : " << RESTendl;
                    ((TRestMetadata*)obj)->PrintMetadata();
                }
            }
        }
        n++;
    }
    /////////////////////////////

    f->Close();

    return 0;
}
#endif
