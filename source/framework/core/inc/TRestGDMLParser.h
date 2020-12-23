
#ifndef restG4_TRestGDMLParser
#define restG4_TRestGDMLParser

#include "TGeoManager.h"
#include "TRestMetadata.h"
#include "unistd.h"

///////////////////////////////////////////
// we must preprocess gdml file because of a bug in TGDMLParse::Value() in ROOT6
//

class TRestGDMLParser : public TRestMetadata {
   private:
    TGeoManager* fGeo;

   public:
    TRestGDMLParser() {}
    ~TRestGDMLParser() {}
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

    ClassDef(TRestGDMLParser, 1);
};

#endif
