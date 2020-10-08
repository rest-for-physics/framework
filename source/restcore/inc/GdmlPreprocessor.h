
#ifndef restG4_GdmlPreprocessor
#define restG4_GdmlPreprocessor

#include "TGeoManager.h"
#include "TRestMetadata.h"
#include "unistd.h"

///////////////////////////////////////////
// we must preprocess gdml file because of a bug in TGDMLParse::Value() in ROOT6
//

class GdmlPreprocessor : public TRestMetadata {
   public:
    GdmlPreprocessor() {}
    ~GdmlPreprocessor() {}
    string filestr = "";
    string path = "";
    // outPath might be associated in future to a temporal REST_USER_PATH directory
    // string outPath = REST_USER_PATH + "/gdml/";
    string outPath = "/tmp/";
    string outfilename = "";
    string gdmlVersion = "0.0";
    map<string, string> entityVersion;

    string GetEntityVersion(string name);

    string GetGDMLVersion();

    string GetOutputGDMLFile();

    void Load(string file);

    TGeoManager* CreateGeoM();

    void InitFromConfigFile() {}

    void PrintContent();

    void ReplaceEntity();

    void ReplaceAttributeWithKeyWord(string keyword);

    ClassDef(GdmlPreprocessor, 1);
};

#endif
