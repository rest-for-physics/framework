///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestVolumeHits.h
///
///             Event class to store signals form simulation and acquisition
///             events
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________

#include "TRestVolumeHits.h"
using namespace std;

ClassImp(TRestVolumeHits)
    //______________________________________________________________________________
    TRestVolumeHits::TRestVolumeHits() {
    // TRestVolumeHits default constructor
}

//______________________________________________________________________________
TRestVolumeHits::~TRestVolumeHits() {
    // TRestVolumeHits destructor
}

void TRestVolumeHits::AddHit(const Double_t &x, const Double_t &y, const Double_t &z, const Double_t &en, const Double_t &time, const TString &type, const Double_t &sigmax, const Double_t &sigmay, const Double_t &sigmaz) {
    if ( type != fHits[0].fType) {
        cout << "Error! Cannot add different typed hits into TRestVolumeHits!" << endl;
        return;
    }

    TRestHits::AddHit(x, y, z, en, time, type);
    fSigmaX.push_back((Float_t)sigmax);
    fSigmaY.push_back((Float_t)sigmay);
    fSigmaZ.push_back((Float_t)sigmaz);
}

void TRestVolumeHits::AddHit(const TVector3 &pos, const Double_t &en, const Double_t &time, const TString &type, const TVector3 &sigma) {
    if (type != fHits[0].fType) {
        cout << "Error! Cannot add different typed hits into TRestVolumeHits!" << endl;
        return;
    }

    TRestHits::AddHit(pos, en, time, type);
    fSigmaX.push_back((Float_t)sigma.X());
    fSigmaY.push_back((Float_t)sigma.Y());
    fSigmaZ.push_back((Float_t)sigma.Z());
}

void TRestVolumeHits::AddHit(TRestVolumeHits& hits, Int_t n) {
    Double_t x = hits.GetX(n);
    Double_t y = hits.GetY(n);
    Double_t z = hits.GetZ(n);
    Double_t t = hits.GetTime(n);
    Double_t en = hits.GetEnergy(n);
    TString type = hits.GetType(n);

    Double_t sx = hits.GetSigmaX(n);
    Double_t sy = hits.GetSigmaY(n);
    Double_t sz = hits.GetSigmaZ(n);

    if (type != fHits[0].fType) {
        cout << "Error! Cannot add different typed hits into TRestVolumeHits!" << endl;
        return;
    }

    AddHit(x, y, z, en, t, type, sx, sy, sz);
}

void TRestVolumeHits::RemoveHits() {
    // cout << "Removing hits" << endl;
    TRestHits::RemoveHits();
    fSigmaX.clear();
    fSigmaY.clear();
    fSigmaZ.clear();
}

void TRestVolumeHits::MergeHits(Int_t n, Int_t m) {
    Double_t totalEnergy = fHits.at(n).fEnergy + fHits.at(m).fEnergy;

    // TODO : This is wrong but not very important for the moment
    fSigmaX[n] = (fSigmaX[n] * fHits.at(n).fEnergy + fSigmaX[m] * fHits.at(m).fEnergy) / totalEnergy;
    fSigmaY[n] = (fSigmaY[n] * fHits.at(n).fEnergy + fSigmaY[m] * fHits.at(m).fEnergy) / totalEnergy;
    fSigmaZ[n] = (fSigmaZ[n] * fHits.at(n).fEnergy + fSigmaZ[m] * fHits.at(m).fEnergy) / totalEnergy;

    fSigmaX.erase(fSigmaX.begin() + m);
    fSigmaY.erase(fSigmaY.begin() + m);
    fSigmaZ.erase(fSigmaZ.begin() + m);

    TRestHits::MergeHits(n, m);
}

void TRestVolumeHits::RemoveHit(int n) {
    TRestHits::RemoveHit(n);

    fSigmaX.erase(fSigmaX.begin() + n);
    
    fSigmaY.erase(fSigmaY.begin() + n);
    
    fSigmaZ.erase(fSigmaZ.begin() + n);
    
}

void TRestVolumeHits::SortByEnergy() {
    sort(fHits.begin(),fHits.end(),TRestHit::sortByEn);
}

void TRestVolumeHits::SwapHits(Int_t i, Int_t j) {
    swap(fSigmaX[i], fSigmaX[j]);
    swap(fSigmaY[i], fSigmaY[j]);
    swap(fSigmaZ[i], fSigmaZ[j]);

    TRestHits::SwapHits(i, j);
}

TVector3 TRestVolumeHits::GetSigma(int n) {
    return TVector3(((Double_t)fSigmaX[n]), ((Double_t)fSigmaY[n]), ((Double_t)fSigmaZ[n]));
}

void TRestVolumeHits::PrintHits() {
    for (int n = 0; n < GetNumberOfHits(); n++) {
        cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n)
             << " sX: " << GetSigmaX(n) << " sY: " << GetSigmaY(n) << " sZ: " << GetSigmaZ(n)
             << " Energy: " << GetEnergy(n) << endl;
    }
}
