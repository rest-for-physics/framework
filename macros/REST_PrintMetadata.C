#include "TRestTask.h"
Int_t REST_Metadata(TString fName, TString objName = "") {
    TRestStringOutput cout;
    cout.setorientation(1);

    TString fileName = fName;

    cout << "Filename : " << fileName << endl;

    string fname = fileName.Data();

    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
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
                cout << "Cannot Get object with name \"" << key->GetName() << "\"!" << endl;
                cout << "The name may contain illegel characters which was legel in previous version."
                     << endl;
            } else if (obj->InheritsFrom("TRestMetadata")) {
                if (objName == "" || objName == obj->ClassName() || objName == obj->GetName()) {
                    cout << n << "th object : " << endl;
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
