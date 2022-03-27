
#ifndef restG4_TRestGDMLParser
#define restG4_TRestGDMLParser

#include <TGeoManager.h>
#include <unistd.h>

#include "TRestMetadata.h"
#include "TRestTools.h"

///////////////////////////////////////////
// we must preprocess gdml file because of a bug in TGDMLParse::Value() in ROOT6
//

class TRestGDMLParser : public TRestMetadata {
   private:
    TGeoManager* fGeo;

   public:
    TRestGDMLParser() {}
    ~TRestGDMLParser() {}
    std::string filestr = "";
    std::string path = "";
    std::string outPath = REST_USER_PATH + "/gdml/";
    std::string outfilename = "";
    std::string gdmlVersion = "0.0";
    std::map<std::string, std::string> entityVersion;

    std::string GetEntityVersion(const std::string& name);

    std::string GetGDMLVersion();

    std::string GetOutputGDMLFile();

    void Load(std::string file);

    TGeoManager* GetGeoManager(std::string gdmlFile) {
        Load(gdmlFile);
        fGeo = TGeoManager::Import(GetOutputGDMLFile().c_str());
        return fGeo;
    }

    TGeoManager* CreateGeoM();

    void InitFromConfigFile() {}

    void PrintContent();

    void ReplaceEntity();

    void ReplaceAttributeWithKeyWord(const std::string& keyword);

    ClassDef(TRestGDMLParser, 2);
};

#endif
