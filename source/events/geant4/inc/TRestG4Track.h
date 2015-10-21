///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrack.h
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestG4Track
#define RestCore_TRestG4Track

#include <vector>
#include <iostream>
using namespace std;

#include "TObject.h"
#include <TString.h>
#include <TArrayI.h>
#include <TVector3.h>
#include <TRestG4Hits.h>

// Perhaps there might be need for a mother class TRestTrack (if there is future need)
class TRestG4Track:public TObject {
    protected:
        Int_t fTrack_ID;
        Int_t fParent_ID;

        Int_t fSubEventId;

        // We must change this to save space! (Might be not needed afterall)
        //Int_t fParticle_ID;
        TString fParticleName;

        Double_t fGlobalTimestamp; // in seconds precision
        Double_t fTrackTimestamp; // in ns precision (seconds have been removed)
        Double_t fKineticEnergy;

        TRestG4Hits fHits;

        TVector3 fTrackOrigin;

    public:

        TRestG4Hits *GetHits( ) { return &fHits; }

        Int_t GetNumberOfHits() { return fHits.GetNumberOfHits(); }
        Int_t GetTrackID() { return fTrack_ID; }
        Int_t GetParentID() { return fParent_ID; }
        TString GetParticleName() { return fParticleName; }
        Double_t GetGlobalTrackTime() { return fGlobalTimestamp; }
        Double_t GetTrackTimeLength() { return fTrackTimestamp; }
        Double_t GetKineticEnergy() { return fKineticEnergy; }
        Double_t GetTotalDepositedEnergy( ) { return fHits.GetTotalDepositedEnergy(); }
        TVector3 GetTrackOrigin() { return fTrackOrigin; }
        Int_t GetSubEventID() { return fSubEventId; }

        void Initialize() { RemoveHits(); fSubEventId = 0.; }
        void PrintTrack();
        void SetSubEventID( Int_t id ) { fSubEventId = id; }


        void SetTrackID( Int_t id ) { fTrack_ID = id; }
        void SetParentID ( Int_t id ) { fParent_ID = id; }
 //       void SetParticleID( Int_t id ) { fParticle_ID = id; }
        void SetParticleName( TString ptcleName ) { fParticleName = ptcleName; }
        void SetGlobalTrackTime( Double_t time ) { fGlobalTimestamp = time; }
        void SetTrackTimeLength( Double_t time ) { fTrackTimestamp = time; }
        void SetKineticEnergy( Double_t en ) { fKineticEnergy = en; }
        void SetTrackOrigin( TVector3 pos ) { fTrackOrigin = pos; }
        void SetTrackOrigin( Double_t x, Double_t y, Double_t z ) { fTrackOrigin.SetXYZ( x, y, z ); }

        void AddG4Hit( TVector3 pos, Double_t en, Int_t pcs, Int_t vol )
        {
            fHits.AddG4Hit( pos, en, pcs, vol );
        }

        Double_t GetTrackLength();

        Double_t GetDistance( TVector3 v1, TVector3 v2 ) {
            return TMath::Sqrt( (v1.X()-v2.X()) * (v1.X()-v2.X()) +  (v1.Y()-v2.Y()) * (v1.Y()-v2.Y()) +  (v1.Z()-v2.Z()) * (v1.Z()-v2.Z()) );
        }


        void RemoveHits( ) { fHits.RemoveHits ( ); }

        Int_t GetProcessID( TString pcsName );
        TString GetProcessName( Int_t id );
        //    Int_t GetElement( Int_t n ) { return X.At(n); }

        //    Int_t GetParticleID();
        //Construtor
        TRestG4Track();
        //Destructor
        virtual ~ TRestG4Track();

        ClassDef(TRestG4Track, 1);     // REST event superclass
};


#endif
