//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4PrimaryGenerator.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestGeant4PrimaryGenerator
#define RestCore_TRestGeant4PrimaryGenerator

#include <TRestGeant4ParticleCollection.h>
#include <TRestGeant4ParticleSource.h>
#include <TString.h>
#include <TVector3.h>

#include <iostream>

#include "TObject.h"

class TRestGeant4PrimaryGenerator : public TObject {
   protected:
    Int_t fNsources;  // Number of sources to be generated

    // storing particle information for the use of restG4
    // each entry --> one particle
    // all particles are thrown into one geant4 event
    std::vector<TRestGeant4ParticleSource> fSources;

    Int_t fNCollections;

    // storing particle generator information
    // each entry --> one event template¡£
    // in restG4, we randomly pick the template from its entries and refresh
    // fSources
    std::vector<TRestGeant4ParticleCollection*> fParticleCollections;  //!

   public:
    Int_t GetNumberOfCollections() { return fNCollections; }
    Int_t GetNumberOfSources() { return fNsources; }
    TRestGeant4ParticleSource GetParticleSource(int i) { return fSources[i]; }
    TRestGeant4ParticleCollection* GetParticleCollection(Int_t n);

    void Reset();
    void RemoveSources();
    void RemoveParticleCollections();
    void AddSource(TRestGeant4ParticleSource src);
    void AddParticleCollection(TRestGeant4ParticleCollection* collection);

    /// Read n-th collection in fParticleCollections, and set fSources
    /// accordingly. Used in TRestGeant4Metadata::ReadParticleCollection
    void UpdateSourcesFromParticleCollection(Int_t n);

    // Construtor
    TRestGeant4PrimaryGenerator();
    // Destructor
    virtual ~TRestGeant4PrimaryGenerator();

    ClassDef(TRestGeant4PrimaryGenerator, 3);
};
#endif
