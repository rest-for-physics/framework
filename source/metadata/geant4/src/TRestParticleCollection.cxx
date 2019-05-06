///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestParticleCollection.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestParticleCollection.h"
#include "TClass.h"

ClassImp(TRestParticleCollection)
    //______________________________________________________________________________
    TRestParticleCollection::TRestParticleCollection() {
  // TRestParticleCollection default constructor
}

//______________________________________________________________________________
TRestParticleCollection::~TRestParticleCollection() {
  // TRestParticleCollection destructor
}

TRestParticleCollection* TRestParticleCollection::instantiate(
    std::string model) {
  if (model == "decay0") {
    TClass* c = TClass::GetClass("TRestParticleCollectionDecay0");
    if (c != NULL)  // this means we have the package installed
    {
      return (TRestParticleCollection*)c->New();
    }
  }
  // in future we may add more classes to use other particle generation
  // software, e.g. CRY,
  else if (model == "") {
    return new TRestParticleCollection();
  }
  return NULL;
}
