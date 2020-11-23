
#ifndef restG4_GdmlPreprocessor
#define restG4_GdmlPreprocessor

#include "TGeoManager.h"
#include "TRestMetadata.h"
#include "unistd.h"

///////////////////////////////////////////
// we must preprocess gdml file because of a bug in TGDMLParse::Value() in ROOT6
//

class GdmlPreprocessor : public TRestMetadata {
   private:
    TGeoManager* fGeo;

   public:
    GdmlPreprocessor() {}
    ~GdmlPreprocessor() {}
    string filestr = "";
    string path = "";
    string outPath = REST_USER_PATH + "/gdml/";
    string outfilename = "";
    string gdmlVersion = "0.0";
    map<string, string> entityVersion;

    string GetEntityVersion(string name);

    string GetGDMLVersion();

    string GetOutputGDMLFile();

    void Load(string file);

    TGeoManager* GetGeoManager(string gdmlfile) {
        Load(gdmlfile);
        fGeo = TGeoManager::Import(GetOutputGDMLFile().c_str());
        return fGeo;
    }

    TGeoManager* CreateGeoM();

    void InitFromConfigFile() {}

    void PrintContent();

    void ReplaceEntity();

    void ReplaceAttributeWithKeyWord(string keyword);

    ClassDef(GdmlPreprocessor, 1);
};

#endif
