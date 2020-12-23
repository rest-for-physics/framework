#ifndef RestCore_TRestGeant4ParticleCollectionD0
#define RestCore_TRestGeant4ParticleCollectionD0

#include <TRestGeant4Particle.h>
#include <iostream>
#include "TObject.h"

#include "TRestGeant4ParticleCollection.h"
#include "TRestMetadata.h"

#include <bxdecay0/decay0_generator.h>
#include <bxdecay0/event.h>
#include <bxdecay0/std_random.h>

using namespace std;

class TRestParticleCollectionDecay0 : public TRestGeant4ParticleCollection, public TRestMetadata {
   protected:
    bxdecay0::decay0_generator fDecay0Model;  //!
    bxdecay0::std_random prng;                //!
    std::default_random_engine generator;     //!

    string fParentName;
    string fDecayType;

    int fDaugherLevel;

   public:
    void SetParticleModel(std::string modelstring);
    void VirtualUpdate();
    void InitFromConfigFile();
    virtual Int_t GetNumberOfParticles() { return fParticles.size(); }
    virtual TRestGeant4Particle GetParticle(int i) { return fParticles[i]; }

    virtual void RemoveParticles() { fParticles.clear(); }
    virtual void AddParticle(TRestGeant4Particle ptcle) { fParticles.push_back(ptcle); }

    TRestParticleCollectionDecay0() : generator((uintptr_t) this), prng(generator) {}

    ClassDef(TRestParticleCollectionDecay0, 1);
};
#endif
