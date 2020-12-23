///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleCollection.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4ParticleCollection.h"
#include "TClass.h"
#include "TRestStringHelper.h"

ClassImp(TRestGeant4ParticleCollection)
    //______________________________________________________________________________
    TRestGeant4ParticleCollection::TRestGeant4ParticleCollection() {
    // TRestGeant4ParticleCollection default constructor
}

//______________________________________________________________________________
TRestGeant4ParticleCollection::~TRestGeant4ParticleCollection() {
    // TRestGeant4ParticleCollection destructor
}

TRestGeant4ParticleCollection* TRestGeant4ParticleCollection::instantiate(std::string model) {
    if (model == "" || model == "geant4") {
        // use default generator
        return new TRestGeant4ParticleCollection();
    } else {
        // use specific generator
        // naming convension: TRestGeant4ParticleCollectionXXX
        // currently supported generator: decay0
        // in future we may add wrapper of generators: cry(for muon), pythia(for HEP), etc.
        model[0] = *REST_StringHelper::ToUpper(std::string(&model[0], 1)).c_str();
        TClass* c = TClass::GetClass(("TRestGeant4ParticleCollection" + model).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            return (TRestGeant4ParticleCollection*)c->New();
        } else {
            std::cout << "REST ERROR! generator wrapper \"" << ("TRestGeant4ParticleCollection" + model)
                      << "\" not found!" << std::endl;
        }
    }
    return NULL;
}
