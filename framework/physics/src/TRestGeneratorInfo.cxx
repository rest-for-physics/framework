///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeneratorInfo.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeneratorInfo.h"
#include "TClass.h"
#include "TRestStringHelper.h"

ClassImp(TRestGeneratorInfo)
    //______________________________________________________________________________
    TRestGeneratorInfo::TRestGeneratorInfo() {
    // TRestGeneratorInfo default constructor
}

//______________________________________________________________________________
TRestGeneratorInfo::~TRestGeneratorInfo() {
    // TRestGeneratorInfo destructor
}

TRestGeneratorInfo* TRestGeneratorInfo::instantiate(std::string model) {
    if (model == "" || model == "geant4") {
        // use default generator
        return new TRestGeneratorInfo();
    } else {
        // use specific generator
        // naming convension: TRestGeneratorInfoXXX
        // currently supported generator: decay0
        // in future we may add wrapper of generators: cry(for muon), pythia(for HEP), etc.
        model[0] = *REST_StringHelper::ToUpper(std::string(&model[0], 1)).c_str();
        TClass* c = TClass::GetClass(("TRestGeneratorInfo" + model).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            return (TRestGeneratorInfo*)c->New();
        } else {
            std::cout << "REST ERROR! generator wrapper \"" << ("TRestGeneratorInfo" + model)
                      << "\" not found!" << std::endl;
        }
    }
    return NULL;
}
