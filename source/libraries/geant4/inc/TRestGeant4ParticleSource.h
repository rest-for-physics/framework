//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleSource.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestGeant4ParticleSource
#define RestCore_TRestGeant4ParticleSource

#include <iostream>

#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>
#include "TObject.h"
//
#include <TRestGeant4Particle.h>

class TRestGeant4ParticleSource : public TRestGeant4Particle {
   protected:
    TString fAngularDistType;
    TString fEnergyDistType;
    TVector2 fEnergyRange;

    TString fSpectrumFilename;
    TString fSpectrumName;

    TString fAngularFilename;
    TString fAngularName;

   public:
    TString GetParticle() { return fParticleName; }
    TString GetAngularDistType() { return fAngularDistType; }
    TVector3 GetDirection() { return fDirection; }
    TString GetEnergyDistType() { return fEnergyDistType; }
    TVector2 GetEnergyRange() { return fEnergyRange; }
    Double_t GetMinEnergy() { return fEnergyRange.X(); }
    Double_t GetMaxEnergy() { return fEnergyRange.Y(); }

    TString GetSpectrumFilename() { return fSpectrumFilename; }
    TString GetSpectrumName() { return fSpectrumName; }

    TString GetAngularFilename() { return fAngularFilename; }
    TString GetAngularName() { return fAngularName; }

    void SetAngularDistType(TString type) { fAngularDistType = type; }
    void SetEnergyDistType(TString type) { fEnergyDistType = type; }
    void SetEnergyRange(TVector2 range) { fEnergyRange = range; }

    void SetSpectrumFilename(TString spctFilename) { fSpectrumFilename = spctFilename; }
    void SetSpectrumName(TString spctName) { fSpectrumName = spctName; }

    void SetAngularFilename(TString angFilename) { fAngularFilename = angFilename; }
    void SetAngularName(TString angName) { fAngularName = angName; }

    void PrintParticleSource();

    // Construtor
    TRestGeant4ParticleSource();
    // Destructor
    virtual ~TRestGeant4ParticleSource();

    ClassDef(TRestGeant4ParticleSource, 2);
};
#endif
