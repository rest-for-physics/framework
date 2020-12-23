///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4PrimaryGenerator.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4PrimaryGenerator.h"
using namespace std;

ClassImp(TRestGeant4PrimaryGenerator)
    //______________________________________________________________________________
    TRestGeant4PrimaryGenerator::TRestGeant4PrimaryGenerator() {
    // TRestGeant4PrimaryGenerator default constructor
    Reset();
}

//______________________________________________________________________________
TRestGeant4PrimaryGenerator::~TRestGeant4PrimaryGenerator() {}

void TRestGeant4PrimaryGenerator::Reset() {
    fNsources = 0;
    fSources.clear();

    RemoveParticleCollections();
}

void TRestGeant4PrimaryGenerator::RemoveSources() {
    fSources.clear();
    fNsources = 0;
}

void TRestGeant4PrimaryGenerator::AddSource(TRestGeant4ParticleSource src) {
    fSources.push_back(src);
    fNsources++;
}

TRestGeant4ParticleCollection* TRestGeant4PrimaryGenerator::GetParticleCollection(Int_t n) {
    return fParticleCollections[n];
}

void TRestGeant4PrimaryGenerator::RemoveParticleCollections() {
    for (auto c : fParticleCollections) {
        delete c;
    }
    fParticleCollections.clear();
    fNCollections = 0;
}

void TRestGeant4PrimaryGenerator::AddParticleCollection(TRestGeant4ParticleCollection* collection) {
    fParticleCollections.push_back(collection);
    fNCollections++;
}

void TRestGeant4PrimaryGenerator::UpdateSourcesFromParticleCollection(Int_t n) {
    RemoveSources();

    Int_t pCollectionID = n % fNCollections;

    TRestGeant4ParticleCollection* pCollection = GetParticleCollection(pCollectionID);

    pCollection->VirtualUpdate();

    TRestGeant4ParticleSource src;
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
