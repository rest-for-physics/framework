///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4PrimaryGenerator.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestG4PrimaryGenerator.h"
using namespace std;

ClassImp(TRestG4PrimaryGenerator)
    //______________________________________________________________________________
    TRestG4PrimaryGenerator::TRestG4PrimaryGenerator() {
  // TRestG4PrimaryGenerator default constructor
  Reset();
}

//______________________________________________________________________________
TRestG4PrimaryGenerator::~TRestG4PrimaryGenerator() {
  // TRestG4PrimaryGenerator destructor
}

void TRestG4PrimaryGenerator::SetSourcesFromParticleCollection(Int_t n) {
  RemoveSources();

  Int_t pCollectionID = n % fNCollections;

  TRestParticleCollection* pCollection = GetParticleCollection(pCollectionID);

  pCollection->VirtualUpdate();

  TRestParticleSource src;
  src.SetAngularDistType("flux");
  src.SetEnergyDistType("mono");

  // cout << pCollectionID << " ";
  for (int i = 0; i < pCollection->GetNumberOfParticles(); i++) {
    src.SetParticle(pCollection->GetParticle(i));

    // cout << pCollection->GetParticle(i).GetEnergy()<<" ";

    AddSource(src);
  }
  // cout << endl;
}
