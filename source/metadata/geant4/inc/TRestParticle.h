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

#include "TObject.h"
#include <TString.h>
#include <TVector3.h>

class TRestParticle:public TObject {
    protected:
        TString fParticleName;
        Double_t fExcitationLevel;
        TVector3 fDirection;
        Double_t fEnergy;

    public:

        TString GetParticleName() { return fParticleName; }
        Double_t GetExcitationLevel() { return fExcitationLevel; }
        Double_t GetEnergy() { return fEnergy; }
        TVector3 GetMomentumDirection() { return fDirection; }

        void SetParticle ( TRestParticle ptcle ) 
        { 
            fExcitationLevel=ptcle.GetExcitationLevel();
            fParticleName = ptcle.GetParticleName();
            fEnergy = ptcle.GetEnergy() ;
            fDirection = ptcle.GetMomentumDirection();
        }

        void SetParticleName( TString ptcle ) { fParticleName = ptcle; }
        void SetExcitationLevel( Double_t eenergy )
        {
            fExcitationLevel = eenergy;
            if( fExcitationLevel < 0 ) fExcitationLevel = 0;
        }

        void SetDirection( TVector3 dir ) { fDirection = dir; }
        void SetEnergy( Double_t en ) { fEnergy = en; }

        //Construtor
        TRestParticle();
        //Destructor
        virtual ~ TRestParticle();

        ClassDef(TRestParticle, 1); 
};
#endif
