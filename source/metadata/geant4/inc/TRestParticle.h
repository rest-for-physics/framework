//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestParticle.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestParticle
#define RestCore_TRestParticle

#include <iostream>

#include <TString.h>
#include <TVector3.h>
#include "TObject.h"

class TRestParticle : public TObject {
 protected:
  TString fParticleName;
  Double_t fExcitationLevel = 0;
  TVector3 fDirection;
  Double_t fEnergy;
  Int_t fCharge = 0;

 public:
  TString GetParticleName() { return fParticleName; }
  Double_t GetExcitationLevel() { return fExcitationLevel; }
  Double_t GetEnergy() { return fEnergy; }
  TVector3 GetMomentumDirection() { return fDirection; }
  Int_t GetParticleCharge() { return fCharge; }

  void SetParticle(TRestParticle ptcle) {
    fExcitationLevel = ptcle.GetExcitationLevel();
    fParticleName = ptcle.GetParticleName();
    fEnergy = ptcle.GetEnergy();
    fDirection = ptcle.GetMomentumDirection();
  }

  void SetParticleName(TString ptcle) { fParticleName = ptcle; }
  void SetExcitationLevel(Double_t eenergy) {
    fExcitationLevel = eenergy;
    if (fExcitationLevel < 0) fExcitationLevel = 0;
  }

  void SetParticleCharge(Int_t charge) { fCharge = charge; }

  void SetDirection(TVector3 dir) { fDirection = dir; }
  void SetEnergy(Double_t en) { fEnergy = en; }

  // Construtor
  TRestParticle();
  // Destructor
  virtual ~TRestParticle();

  ClassDef(TRestParticle, 2);
};
#endif
