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

#include <TRestEventProcess.h>  // TODO: for some reason TRestG4Metadata is declared here, review this
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

    set<string> energy_generator_types{
        "TH1D",
        "mono",
        "flat",
    };  //!

    set<string> angular_generator_types{
        "TH1D",
        "isotropic",
        "flux",
        "back_to_back",
    };  //!

    string default_generator_type_value = "NOT_ASSIGNED";  //!

    string spatial_generator_type = default_generator_type_value;  //!
    string angular_generator_type = default_generator_type_value;  //!
    string energy_generator_type = default_generator_type_value;   //!

    inline string clean_type_string(string type) {
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        // remove '_'
        string string_to_remove = "_";
        while (type.find(string_to_remove) != string::npos) {
            type.replace(type.find("_"), string_to_remove.length(), "");
        }
        return type;
    }

    Float_t sphere_radius = 0;  //!
    TVector3 generator_origin = TVector3(0,0,0);  //!

    TVector3 particle_position; //!
    TVector3 particle_direction; //!

   public:
    // regarding particle generation
    inline string get_spatial_generator_type() { return spatial_generator_type; }
    void set_spatial_generator_type(string type);
    inline string get_angular_generator_type() { return angular_generator_type; }
    void set_angular_generator_type(string type);

    inline string get_energy_generator_type() { return energy_generator_type; }
    void set_energy_generator_type(string type);
    // TODO: review this section
    Float_t get_sphere_radius() {
        if (get_spatial_generator_type() == "virtual_sphere") {
            return sphere_radius;
        } else {
            cout << "WARNING: radius is not defined for generator of type: " << get_spatial_generator_type()
                 << endl;
        }
    }
    void set_sphere_radius(Float_t radius) {
        if (get_spatial_generator_type() == "virtual_sphere") {
            sphere_radius = radius;
        } else {
            cout << "WARNING: radius is not defined for generator of type: " << get_spatial_generator_type()
                 << endl;
        }
    }

    inline void set_generator_origin(TVector3 origin) { generator_origin = origin; }
    inline TVector3 get_generator_origin(){ return generator_origin;}

    inline void set_particle_position(TVector3 position) { particle_position = position; }
    inline TVector3 get_particle_position(){ return particle_position;}

    inline void set_particle_direction(TVector3 direction) { particle_direction = direction; }
    inline TVector3 get_particle_direction(){ return particle_direction;}

    void initialize_from_metadata(TRestG4Metadata*);
    void sample_spatial_generator();

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
