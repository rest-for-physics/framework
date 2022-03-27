#ifndef RestCore_TRestManager
#define RestCore_TRestManager

#include <TApplication.h>
#include <TObject.h>
#include <TROOT.h>

#include <iostream>

#include "TRestAnalysisPlot.h"
#include "TRestMetadata.h"
#include "TRestProcessRunner.h"
#include "TRestRun.h"

/// Managing applications and executing tasks
class TRestManager : public TRestMetadata {
   private:
    /// app-like metadata objects
    std::vector<TRestMetadata*> fMetaObjects;  //!

   public:
    void Initialize();
    void InitFromConfigFile() {
        if (fElement != nullptr) {
            TiXmlElement* e = fElement->FirstChildElement();
            while (e != nullptr) {
                std::string value = e->Value();
                if (value == "variable" || value == "myParameter" || value == "constant") {
                    e = e->NextSiblingElement();
                    continue;
                }
                ReadConfig((std::string)e->Value(), e);
                e = e->NextSiblingElement();
            }
        }
    }
    int LoadSectionMetadata();
    Int_t ReadConfig(const std::string& keydeclare, TiXmlElement* e);

    void InitFromTask(std::string taskName, std::vector<std::string> arguments);

    void PrintMetadata();

    inline TRestProcessRunner* GetProcessRunner() {
        return (TRestProcessRunner*)GetMetadataClass("TRestProcessRunner");
    }
    inline TRestRun* GetRunInfo() { return (TRestRun*)GetMetadataClass("TRestRun"); }
    inline TRestAnalysisPlot* GetAnaPlot() {
        return (TRestAnalysisPlot*)GetMetadataClass("TRestAnalysisPlot");
    }

    TRestMetadata* GetMetadata(const std::string& name);
    TRestMetadata* GetMetadataClass(const std::string& type);

    TRestManager();
    ~TRestManager();

    /// Call CINT to generate streamers for this class
    ClassDef(TRestManager, 1);
};

#endif
