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

#ifndef RestCore_TRestGeant4Track
#define RestCore_TRestGeant4Track

#include <TArrayI.h>
#include <TColor.h>
#include <TRestGeant4Hits.h>
#include <TString.h>
#include <TVector3.h>

#include <iostream>
#include <vector>

#include "TObject.h"

// Perhaps there might be need for a mother class TRestTrack (if there is future
// need)
class TRestGeant4Track : public TObject {
   protected:
    Int_t fTrack_ID;
    Int_t fParent_ID;

    Int_t fSubEventId;

    // We must change this to save space! (Might be not needed afterall)
    // Int_t fParticle_ID;
    TString fParticleName;

    Double_t fGlobalTimestamp;  // in seconds precision
    Double_t fTrackTimestamp;   // in ns precision (seconds have been removed)
    Double_t fKineticEnergy;

    TRestGeant4Hits fHits;

    TVector3 fTrackOrigin;

   public:
    void Initialize() {
        RemoveHits();
        fSubEventId = 0.;
    }

    TRestGeant4Hits* GetHits() { return &fHits; }

    Double_t GetEnergy() { return GetHits()->GetEnergy(); }

    Int_t GetNumberOfHits() { return fHits.GetNumberOfHits(); }
    Int_t GetTrackID() { return fTrack_ID; }
    Int_t GetParentID() { return fParent_ID; }

    TString GetParticleName() { return fParticleName; }
    EColor GetParticleColor();

    Double_t GetGlobalTime() { return fGlobalTimestamp; }
    Double_t GetTrackTimeLength() { return fTrackTimestamp; }
    Double_t GetKineticEnergy() { return fKineticEnergy; }
    Double_t GetTotalDepositedEnergy() { return fHits.GetTotalDepositedEnergy(); }
    TVector3 GetTrackOrigin() { return fTrackOrigin; }
    Int_t GetSubEventID() { return fSubEventId; }

    Double_t GetEnergyInVolume(Int_t volID) { return GetHits()->GetEnergyInVolume(volID); }
    TVector3 GetMeanPositionInVolume(Int_t volID) { return GetHits()->GetMeanPositionInVolume(volID); }
    TVector3 GetFirstPositionInVolume(Int_t volID) { return GetHits()->GetFirstPositionInVolume(volID); }
    TVector3 GetLastPositionInVolume(Int_t volID) { return GetHits()->GetLastPositionInVolume(volID); }

    void SetSubEventID(Int_t id) { fSubEventId = id; }

    void SetTrackID(Int_t id) { fTrack_ID = id; }
    void SetParentID(Int_t id) { fParent_ID = id; }
    //       void SetParticleID( Int_t id ) { fParticle_ID = id; }
    void SetParticleName(TString ptcleName) { fParticleName = ptcleName; }
    void SetGlobalTrackTime(Double_t time) { fGlobalTimestamp = time; }
    void SetTrackTimeLength(Double_t time) { fTrackTimestamp = time; }
    void SetKineticEnergy(Double_t en) { fKineticEnergy = en; }
    void SetTrackOrigin(TVector3 pos) { fTrackOrigin = pos; }
    void SetTrackOrigin(Double_t x, Double_t y, Double_t z) { fTrackOrigin.SetXYZ(x, y, z); }

    void AddG4Hit(TVector3 pos, Double_t en, Double_t hit_global_time, Int_t pcs, Int_t vol, Double_t eKin,
                  TVector3 momentumDirection) {
        fHits.AddG4Hit(pos, en, hit_global_time, pcs, vol, eKin, momentumDirection);
    }

    Double_t GetTrackLength();

    Double_t GetDistance(TVector3 v1, TVector3 v2) {
        return TMath::Sqrt((v1.X() - v2.X()) * (v1.X() - v2.X()) + (v1.Y() - v2.Y()) * (v1.Y() - v2.Y()) +
                           (v1.Z() - v2.Z()) * (v1.Z() - v2.Z()));
    }

    void RemoveHits() { fHits.RemoveHits(); }

    // TODO move this to a namespace header??
    Int_t GetProcessID(TString pcsName);
    TString GetProcessName(Int_t id);

    Bool_t isRadiactiveDecay() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 11) return true;
        return false;
    }
    Bool_t isPhotoElectric() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 3) return true;
        return false;
    }
    Bool_t isCompton() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 7) return true;
        return false;
    }
    Bool_t isBremstralung() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 5) return true;
        return false;
    }

    Bool_t ishadElastic() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 36) return true;
        return false;
    }
    Bool_t isneutronInelastic() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 37) return true;
        return false;
    }
    Bool_t isnCapture() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 38) return true;
        return false;
    }

    Bool_t ishIoni() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 33) return true;
        return false;
    }
    Bool_t isphotonNuclear() {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if (GetHits()->GetHitProcess(n) == 42) return true;
        return false;
    }
    // Processes in active volume
    Bool_t isRadiactiveDecayInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 11) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }
    Bool_t isPhotoElectricInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 3) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }
    Bool_t isPhotonNuclearInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 42) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }

    Bool_t isComptonInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 7) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }
    Bool_t isBremstralungInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 5) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }

    Bool_t isHadElasticInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 36) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }
    Bool_t isNeutronInelasticInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 37) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }

    Bool_t isNCaptureInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 38) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }

    Bool_t isHIoniInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitProcess(n) == 33) && (GetHits()->GetHitVolume(n)) == volID) return true;
        return false;
    }

    Bool_t isAlphaInVolume(Int_t volID) {
        if (GetParticleName() == "alpha") {
            for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
                if ((GetHits()->GetHitVolume(n)) == volID) return true;
        }
        return false;
    }

    Bool_t isNeutronInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitVolume(n) == volID) && (GetParticleName() == "neutron")) return true;
        return false;
    }

    Bool_t isArgonInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitVolume(n) == volID) && (GetParticleName().Contains("Ar"))) return true;
        return false;
    }

    Bool_t isNeonInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitVolume(n) == volID) && (GetParticleName().Contains("Ne"))) return true;
        return false;
    }

    Bool_t isXenonInVolume(Int_t volID) {
        for (int n = 0; n < GetHits()->GetNumberOfHits(); n++)
            if ((GetHits()->GetHitVolume(n) == volID) && (GetParticleName().Contains("Xe"))) return true;
        return false;
    }
    /////////////////////////////////

    /// Prints the track information. N number of hits to print, 0 = all
    void PrintTrack(int maxHits = 0);

    //    Int_t GetElement( Int_t n ) { return X.At(n); }

    //    Int_t GetParticleID();
    // Construtor
    TRestGeant4Track();
    // Destructor
    virtual ~TRestGeant4Track();

    ClassDef(TRestGeant4Track, 2);  // REST event superclass
};

#endif
