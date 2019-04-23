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
///
//////////////////////////////////////////////////////////////////////////


#ifndef RestCore_TRestG4PrimaryGenerator
#define RestCore_TRestG4PrimaryGenerator

#include <iostream>

#include "TObject.h"
#include <TString.h>
#include <TVector3.h>

#include <TRestParticleSource.h>
#include <TRestParticleCollection.h>

class TRestG4PrimaryGenerator:public TObject {
    protected:
        Int_t fNsources;  // Number of sources to be generated

		//storing particle information for the use of restG4
		//each entry --> one particle
		//all particles are thrown into one geant4 event
        std::vector <TRestParticleSource> fSources;

        Int_t fNCollections;

		//storing particle generator information
		//each entry --> one event template¡£
		//in restG4, we randomly pick the template from its entries and refresh fSources
        std::vector <TRestParticleCollection*> fParticleCollections;

    public:

        Int_t GetNumberOfCollections() { return fNCollections; }
        Int_t GetNumberOfSources() { return fNsources; }
        TRestParticleSource GetParticleSource( int i ) { return fSources[i]; }

        void Reset()
        {
            fNsources = 0;
            fSources.clear();

            fNCollections = 0;
            fParticleCollections.clear();
        }

        void RemoveSources() { fSources.clear(); fNsources = 0; }
        void AddSource( TRestParticleSource src ) { fSources.push_back ( src ); fNsources++; }

        TRestParticleCollection* GetParticleCollection( Int_t n ) { return fParticleCollections[n]; }
        void RemoveParticleCollections() { fParticleCollections.clear(); fNCollections = 0; }
        void AddParticleCollection ( TRestParticleCollection* collection ) { fParticleCollections.push_back( collection ); fNCollections++; }

        void SetSourcesFromParticleCollection( Int_t n );


        //Construtor
        TRestG4PrimaryGenerator();
        //Destructor
        virtual ~ TRestG4PrimaryGenerator();

        ClassDef(TRestG4PrimaryGenerator, 2); 
};
#endif
