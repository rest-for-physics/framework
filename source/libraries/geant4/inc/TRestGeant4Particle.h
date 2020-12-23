//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Particle.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestGeant4Particle
#define RestCore_TRestGeant4Particle

#include <iostream>

#include <TString.h>
#include <TVector3.h>
#include "TObject.h"

class TRestGeant4Particle : public TObject {
   protected:
    TString fParticleName;
    Double_t fExcitationLevel = 0;
    TVector3 fDirection;
    Double_t fEnergy;
    Int_t fCharge = 0;
    TVector3 fOrigin;

   public:
    TString GetParticleName() { return fParticleName; }
    Double_t GetExcitationLevel() { return fExcitationLevel; }
    Double_t GetEnergy() { return fEnergy; }
    TVector3 GetMomentumDirection() { return fDirection; }
    Int_t GetParticleCharge() { return fCharge; }
    TVector3 GetOrigin() { return fOrigin; }

    void SetParticle(TRestGeant4Particle ptcle) {
        fExcitationLevel = ptcle.GetExcitationLevel();
        fParticleName = ptcle.GetParticleName();
        fEnergy = ptcle.GetEnergy();
        fDirection = ptcle.GetMomentumDirection();
        fOrigin = ptcle.fOrigin;
    }

    void SetParticleName(TString ptcle) { fParticleName = ptcle; }
    void SetExcitationLevel(Double_t eenergy) {
        fExcitationLevel = eenergy;
        if (fExcitationLevel < 0) fExcitationLevel = 0;
    }

    void SetParticleCharge(Int_t charge) { fCharge = charge; }

    void SetDirection(TVector3 dir) { fDirection = dir; }
    void SetEnergy(Double_t en) { fEnergy = en; }
    void SetOrigin(TVector3 pos) { fOrigin = pos; }

    // Constructor
    TRestGeant4Particle();
    // Destructor
    virtual ~TRestGeant4Particle();

    ClassDef(TRestGeant4Particle, 3);
};
#endif
