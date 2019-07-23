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
#include <TRestG4Metadata.h>

#include <TMath.h>

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

void TRestG4PrimaryGenerator::set_angular_generator_type(string type) {
    // first we need to check we are passing a valid type value

    bool is_valid_type = false;
    for (auto type_name : angular_generator_types) {
        if (TRestG4PrimaryGenerator::clean_type_string(type) ==
            TRestG4PrimaryGenerator::clean_type_string(type_name)) {
            is_valid_type = true;
            angular_generator_type = type_name;
            break;
        }
    }
    if (!is_valid_type) {
        // not a valid type
        cout << "ERROR: not a valid type passed for angular_generator: " << type << endl;
        cout << "valid types are:";
        for (auto type_name : angular_generator_types) {
            cout << " \"" << type_name << "\",";
        }
        cout << endl;
        throw;
    }
}

void TRestG4PrimaryGenerator::set_energy_generator_type(string type) {
    // first we need to check we are passing a valid type value

    // const bool is_valid_type = spatial_generator_types.find(type) != spatial_generator_types.end();
    bool is_valid_type = false;
    for (auto type_name : energy_generator_types) {
        if (TRestG4PrimaryGenerator::clean_type_string(type) ==
            TRestG4PrimaryGenerator::clean_type_string(type_name)) {
            is_valid_type = true;
            energy_generator_type = type_name;
            break;
        }
    }
    if (!is_valid_type) {
        // not a valid type
        cout << "ERROR: not a valid type passed for spatial_generator: " << type << endl;
        cout << "valid types are:";
        for (auto type_name : energy_generator_types) {
            cout << " \"" << type_name << "\",";
        }
        cout << endl;
        throw;
    }
}

void TRestG4PrimaryGenerator::initialize_from_metadata(TRestG4Metadata* restG4_metadata) {
    // first initialize spatial, angular and energy generator types
    set_spatial_generator_type((string)(restG4_metadata->GetGeneratorType()));
}

void TRestG4PrimaryGenerator::sample_spatial_generator() {
    // sample the generator type (with its own seed, separate from GEANT4 seed
    if (get_spatial_generator_type() == "virtual_sphere") {
        Float_t radius = get_sphere_radius();  // this has to be defined
        if (radius == 0) {
            cout << "WARNING: radius needs to be defined before sampling!" << endl;
        }
        Float_t theta = TMath::Pi() * random.Rndm();
        Float_t phi = TMath::ACos(1 - 2 * random.Rndm());

        TVector3 position(radius * TMath::Sin(theta) * TMath::Cos(phi),
                          radius * TMath::Sin(theta) * TMath::Sin(phi), radius * TMath::Cos(theta));
        TVector3 direction = -1 / radius * position;

        set_particle_position(position + get_generator_origin());
        set_particle_direction(direction);

    }
    // not a valid type or valid type but not implemented yet
    else {
        cout << "sampling this distribution has not been implemented yet or is invalid: "
             << get_spatial_generator_type() << endl;
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
