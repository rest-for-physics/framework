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

void TRestVolumeHits::AddHit(Double_t x, Double_t y, Double_t z, Double_t en,
                             Double_t sigmax, Double_t sigmay,
                             Double_t sigmaz) {
  TRestHits::AddHit(x, y, z, en);
  fSigmaX.push_back((Float_t)sigmax);
  fSigmaY.push_back((Float_t)sigmay);
  fSigmaZ.push_back((Float_t)sigmaz);
}

void TRestVolumeHits::AddHit(TVector3 pos, Double_t en, TVector3 sigma) {
  TRestHits::AddHit(pos, en);

  fSigmaX.push_back((Float_t)sigma.X());
  fSigmaY.push_back((Float_t)sigma.Y());
  fSigmaZ.push_back((Float_t)sigma.Z());
}

void TRestVolumeHits::RemoveHits() {
  // cout << "Removing hits" << endl;
  TRestHits::RemoveHits();
  fSigmaX.clear();
  fSigmaY.clear();
  fSigmaZ.clear();
}

void TRestVolumeHits::MergeHits(Int_t n, Int_t m) {
  Double_t totalEnergy = fEnergy[n] + fEnergy[m];

  // TODO : This is wrong but not very important for the moment
  fSigmaX[n] =
      (fSigmaX[n] * fEnergy[n] + fSigmaX[m] * fEnergy[m]) / totalEnergy;
  fSigmaY[n] =
      (fSigmaY[n] * fEnergy[n] + fSigmaY[m] * fEnergy[m]) / totalEnergy;
  fSigmaZ[n] =
      (fSigmaZ[n] * fEnergy[n] + fSigmaZ[m] * fEnergy[m]) / totalEnergy;

  fSigmaX.erase(fSigmaX.begin() + m);
  fSigmaY.erase(fSigmaY.begin() + m);
  fSigmaZ.erase(fSigmaZ.begin() + m);

  TRestHits::MergeHits(n, m);
}

void TRestVolumeHits::RemoveHit(int n) {
  TRestHits::RemoveHit(n);

  fSigmaX.erase(fSigmaX.begin() + n);
  ;
  fSigmaY.erase(fSigmaY.begin() + n);
  ;
  fSigmaZ.erase(fSigmaZ.begin() + n);
  ;
}

void TRestVolumeHits::SortByEnergy() {
  while (!isSortedByEnergy()) {
    for (int i = 0; i < GetNumberOfHits(); i++) {
      for (int j = i + 1; j < GetNumberOfHits(); j++) {
        if (GetEnergy(i) < GetEnergy(j)) SwapHits(i, j);
      }
    }
  }
}

void TRestVolumeHits::SwapHits(Int_t i, Int_t j) {
  iter_swap(fSigmaX.begin() + i, fSigmaX.begin() + j);
  iter_swap(fSigmaY.begin() + i, fSigmaY.begin() + j);
  iter_swap(fSigmaZ.begin() + i, fSigmaZ.begin() + j);

  TRestHits::SwapHits(i, j);
}

TVector3 TRestVolumeHits::GetSigma(int n) {
  return TVector3(((Double_t)fSigmaX[n]), ((Double_t)fSigmaY[n]),
                  ((Double_t)fSigmaZ[n]));
}

void TRestVolumeHits::PrintHits() {
  for (int n = 0; n < GetNumberOfHits(); n++) {
    cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n)
         << " Z: " << GetZ(n) << " sX: " << GetSigmaX(n)
         << " sY: " << GetSigmaY(n) << " sZ: " << GetSigmaZ(n)
         << " Energy: " << GetEnergy(n) << endl;
  }
}
