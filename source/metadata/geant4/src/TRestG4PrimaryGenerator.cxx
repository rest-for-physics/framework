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
TRestG4PrimaryGenerator::~TRestG4PrimaryGenerator() {}

void TRestG4PrimaryGenerator::Reset() {
    fNsources = 0;
    fSources.clear();

    RemoveParticleCollections();
}

void TRestG4PrimaryGenerator::RemoveSources() {
    fSources.clear();
    fNsources = 0;
}

void TRestG4PrimaryGenerator::AddSource(TRestParticleSource src) {
    fSources.push_back(src);
    fNsources++;
}

TRestParticleCollection* TRestG4PrimaryGenerator::GetParticleCollection(Int_t n) {
    return fParticleCollections[n];
}

void TRestG4PrimaryGenerator::RemoveParticleCollections() {
    for (auto c : fParticleCollections) {
        delete c;
    }
    fParticleCollections.clear();
    fNCollections = 0;
}

void TRestG4PrimaryGenerator::AddParticleCollection(TRestParticleCollection* collection) {
    fParticleCollections.push_back(collection);
    fNCollections++;
}

void TRestG4PrimaryGenerator::UpdateSourcesFromParticleCollection(Int_t n) {
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
