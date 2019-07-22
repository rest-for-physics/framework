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

// regarding particle generation
void TRestG4PrimaryGenerator::set_spatial_generator_type(string type) {
    // first we need to check we are passing a valid type value

    // const bool is_valid_type = spatial_generator_types.find(type) != spatial_generator_types.end();
    bool is_valid_type = false;
    for (auto type_name : spatial_generator_types) {
        if (TRestG4PrimaryGenerator::clean_type_string(type) ==
            TRestG4PrimaryGenerator::clean_type_string(type_name)) {
            is_valid_type = true;
            spatial_generator_type = type_name;
            break;
        }
    }
    if (!is_valid_type) {
        // not a valid type
        cout << "ERROR: not a valid type passed for spatial_generator: " << type << endl;
        cout << "valid types are:";
        for (auto type_name : spatial_generator_types) {
            cout << " \"" << type_name << "\",";
        }
        cout << endl;
        throw;
    }
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
