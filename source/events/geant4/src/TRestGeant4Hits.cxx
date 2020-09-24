///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Hits.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4Hits.h"

ClassImp(TRestGeant4Hits);

//______________________________________________________________________________
TRestGeant4Hits::TRestGeant4Hits() : TRestHits() {
    // TRestGeant4Hits default constructor
}

//______________________________________________________________________________
TRestGeant4Hits::~TRestGeant4Hits() {
    // TRestGeant4Hits destructor
}

void TRestGeant4Hits::AddG4Hit(TVector3 pos, Double_t en, Double_t hit_global_time, Int_t process,
                               Int_t volume, Double_t eKin, TVector3 momentumDirection) {
    AddHit(pos, en, hit_global_time);

    fProcessID.Set(fNHits);
    fProcessID[fNHits - 1] = process;

    fVolumeID.Set(fNHits);
    fVolumeID[fNHits - 1] = volume;

    fKineticEnergy.Set(fNHits);
    fKineticEnergy[fNHits - 1] = eKin;

    momentumDirection = momentumDirection.Unit();

    Float_t x = momentumDirection.X();
    Float_t y = momentumDirection.Y();
    Float_t z = momentumDirection.Z();

    fMomentumDirectionX.Set(fNHits);
    fMomentumDirectionX[fNHits - 1] = x;

    fMomentumDirectionY.Set(fNHits);
    fMomentumDirectionY[fNHits - 1] = y;

    fMomentumDirectionZ.Set(fNHits);
    fMomentumDirectionZ[fNHits - 1] = z;
}

void TRestGeant4Hits::RemoveG4Hits() {
    RemoveHits();

    fProcessID.Set(0);

    fVolumeID.Set(0);

    fKineticEnergy.Set(0);
}

Double_t TRestGeant4Hits::GetEnergyInVolume(Int_t volID) {
    Double_t en = 0;

    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volID) en += GetEnergy(n);

    return en;
}

TVector3 TRestGeant4Hits::GetMeanPositionInVolume(Int_t volID) {
    TVector3 pos;
    Double_t en = 0;
    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volID) {
            pos += GetPosition(n) * GetEnergy(n);
            en += GetEnergy(n);
        }

    if (en == 0) {
        TVector3 pos;
        Double_t nan = TMath::QuietNaN();
        return TVector3(nan, nan, nan);
    }

    pos = (1. / en) * pos;
    return pos;
}

TVector3 TRestGeant4Hits::GetFirstPositionInVolume(Int_t volID) {
    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volID) return GetPosition(n);

    TVector3 pos;
    Double_t nan = TMath::QuietNaN();
    return TVector3(nan, nan, nan);
}

TVector3 TRestGeant4Hits::GetLastPositionInVolume(Int_t volID) {
    for (int n = fNHits - 1; n >= 0; n--)
        if (fVolumeID[n] == volID) return GetPosition(n);

    TVector3 pos;
    Double_t nan = TMath::QuietNaN();
    return TVector3(nan, nan, nan);
}
