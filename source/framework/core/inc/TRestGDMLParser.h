
#ifndef restG4_TRestGDMLParser
#define restG4_TRestGDMLParser

#include <TGeoManager.h>
#include <unistd.h>

#include "TRestMetadata.h"

// we must preprocess gdml file because of a bug in TGDMLParse::Value() in ROOT6

class TRestGDMLParser : public TRestMetadata {
   private:
    TGeoManager* fGeoManager{};

   public:
    TRestGDMLParser() = default;
    ~TRestGDMLParser() = default;

    std::string fFileString = "";
    std::string fPath = "";
    std::string fOutputGdmlDirectory = REST_USER_PATH.empty() ? "/tmp/gdml/" : REST_USER_PATH + "/gdml/";
    std::string fOutputGdmlFilename = "";
    std::string fGdmlVersion = "0.0";
    std::map<std::string, std::string> fEntityVersionMap{};

    std::string GetEntityVersion(const std::string& name) const;

    inline std::string GetGDMLVersion() const { return fGdmlVersion; }

    inline std::string GetOutputGDMLFile() const { return fOutputGdmlFilename; }

    void Load(const std::string& filename);

    inline TGeoManager* GetGeoManager(const std::string& gdmlFilename) {
        Load(gdmlFilename);
        fGeoManager = TGeoManager::Import(GetOutputGDMLFile().c_str());
        return fGeoManager;
    }

    TGeoManager* CreateGeoM();

    void InitFromConfigFile() override {}

    void PrintContent();

    void ReplaceEntity();

    void ReplaceAttributeWithKeyWord(std::string keyword);

    ClassDef(TRestGDMLParser, 2);
};

#endif
