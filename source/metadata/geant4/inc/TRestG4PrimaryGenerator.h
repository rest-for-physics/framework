//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4PrimaryGenerator.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///             jul 2019:   Significant modifications
///                 Implemented many methods previously implemented in restG4 PrimaryGeneratorAction
///                 Such as spatial, angular and energy generators.
///                 L. Obis
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestG4PrimaryGenerator
#define RestCore_TRestG4PrimaryGenerator

#include <iostream>

#include <set>
#include <string>

#include <TString.h>
#include <TVector3.h>
#include "TObject.h"

#include <TRestParticleCollection.h>
#include <TRestParticleSource.h>

using namespace std;

class TRestG4PrimaryGenerator : public TObject {
   protected:
    Int_t fNsources;  // Number of sources to be generated

    // storing particle information for the use of restG4
    // each entry --> one particle
    // all particles are thrown into one geant4 event
    std::vector<TRestParticleSource> fSources;

    Int_t fNCollections;

    // storing particle generator information
    // each entry --> one event template
    // in restG4, we randomly pick the template from its entries and refresh
    // fSources
    std::vector<TRestParticleCollection*> fParticleCollections;

    // regarding particle generation
    set<string> spatial_generator_types{
        "file",
        "volume",
        "surface",
        "point",
        "virtual_wall",
        "virtual_box",
        "virtual_sphere",
        "virtual_circle_wall",
        "virtual_cylinder",
    };  //!

    string default_generator_type_value = "NOT_ASSIGNED";  //!

    string spatial_generator_type = default_generator_type_value;  //!
    string angular_generator_type = default_generator_type_value;  //!
    string energy_generator_type = default_generator_type_value;   //!

    inline string clean_type_string(string type) {
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        // remove '_'
        string string_to_remove = "_";
        while (type.find(string_to_remove) != string::npos){
            type.replace(type.find("_"), string_to_remove.length(), "");
        }
        return type;
    }

   public:
    // regarding particle generation
    inline string get_spatial_generator_type() { return spatial_generator_type; }
    void set_spatial_generator_type(string type);
    inline string get_angular_generator_type() { return angular_generator_type; }
    inline void set_angular_generator_type(string type) {
        angular_generator_type = clean_type_string((string)type);
    }
    inline string get_energy_generator_type() { return energy_generator_type; }
    inline void set_energy_generator_type(string type) {
        energy_generator_type = clean_type_string((string)type);
    }

    Int_t GetNumberOfCollections() { return fNCollections; }
    Int_t GetNumberOfSources() { return fNsources; }
    TRestParticleSource GetParticleSource(int i) { return fSources[i]; }

    void Reset() {
        fNsources = 0;
        fSources.clear();

        fNCollections = 0;
        fParticleCollections.clear();
    }

    void RemoveSources() {
        fSources.clear();
        fNsources = 0;
    }
    void AddSource(TRestParticleSource src) {
        fSources.push_back(src);
        fNsources++;
    }

    TRestParticleCollection* GetParticleCollection(Int_t n) { return fParticleCollections[n]; }
    void RemoveParticleCollections() {
        for (auto c : fParticleCollections) {
            delete c;
        }
        fParticleCollections.clear();
        fNCollections = 0;
    }
    void AddParticleCollection(TRestParticleCollection* collection) {
        fParticleCollections.push_back(collection);
        fNCollections++;
    }

    void SetSourcesFromParticleCollection(Int_t n);

    // Constructor
    TRestG4PrimaryGenerator();
    // Destructor
    virtual ~TRestG4PrimaryGenerator();

    ClassDef(TRestG4PrimaryGenerator, 3);
};
#endif
