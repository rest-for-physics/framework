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
    TRestStringOutput RESTLog;
    RESTLog.setorientation(TRestStringOutput::REST_Display_Orientation::kLeft);

    TString fileName = fName;

    RESTLog << "Filename : " << fileName << RESTendl;

    string fname = fileName.Data();

    if (!TRestTools::fileExists(fname)) {
        RESTLog << "WARNING. Input file does not exist" << RESTendl;
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
                RESTLog << "Cannot Get object with name \"" << key->GetName() << "\"!" << RESTendl;
                RESTLog << "The name may contain illegel characters which was legel in previous version."
                        << RESTendl;
            } else if (obj->InheritsFrom("TRestMetadata")) {
                if (objName == "" || objName == obj->ClassName() || objName == obj->GetName()) {
                    RESTLog << n << "th object : " << RESTendl;
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
