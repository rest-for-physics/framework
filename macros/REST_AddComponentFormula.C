#include "TRestComponent.h"
#include "TRestTask.h"
#include "TRestTools.h"

#ifndef RestTask_AddComponentFormula
#define RestTask_AddComponentFormula

//*******************************************************************************************************
//*** Description: This macro will load from an RML the component chosen in the arguments and it
//*** will write it inside the file given as outputFile
//***
//*** --------------
//*** Usage: restManager AddComponentFormula components.rml sectionName [outputFile] [componentName] [update]
//***
//*** Arguments description:
//***
//*** - cfgFile: The RML configuration file where the component definition can be found.
//*** - sectionName: The section name used to select a component inside the RML file.
//*** - outputFile: The file where the component is written, by default is components.root.
//*** - componentName: This argument allows to change the component name stored in the output file.
//***                  By default it will take the same value as section name.
//*** - update: If disabled it will create a new file erasing any other previously added components.
//***           It is enabled by default.
//***
//*******************************************************************************************************

Int_t REST_AddComponentFormula(std::string cfgFile, std::string sectionName,
                               std::string outputFile = "components.root", std::string componentName = "",
                               Bool_t update = true) {
    TRestComponentFormula comp(cfgFile.c_str(), sectionName.c_str());
    comp.Initialize();

    auto file = TRestRootFileHandle::Open(outputFile,
                                          update ? TRestRootFileMode::Update : TRestRootFileMode::Recreate);
    if (!file) {
        RESTError << file.Error() << RESTendl;
        return -1;
    }

    if (componentName == "") componentName = sectionName;

    comp.Write(componentName.c_str());

    return file.Close() ? 0 : -1;
}
#endif
