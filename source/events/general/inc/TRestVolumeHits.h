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

#ifndef RestCore_TRestVolumeHits
#define RestCore_TRestVolumeHits

#include <iostream>

#include <TObject.h>
#include <TRestHits.h>
#include <TVector3.h>

class TRestVolumeHits : public TRestHits {
 protected:
  std::vector<Float_t>
      fSigmaX;  // [fNHits] Sigma on X axis for each volume hit (units microms)
  std::vector<Float_t>
      fSigmaY;  // [fNHits] Sigma on Y axis for each volume hit (units microms)
  std::vector<Float_t>
      fSigmaZ;  // [fNHits] Sigma on Z axis for each volume hit (units microms)

 public:
  void AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t sigmax,
              Double_t sigmay, Double_t sigmaz);
  void AddHit(TVector3 pos, Double_t en, TVector3 sigma);
  void AddHit(TRestVolumeHits& hits, Int_t n) {
    Double_t x = hits.GetX(n);
    Double_t y = hits.GetY(n);
    Double_t z = hits.GetZ(n);
    Double_t en = hits.GetEnergy(n);
    Double_t sx = hits.GetSigmaX(n);
    Double_t sy = hits.GetSigmaY(n);
    Double_t sz = hits.GetSigmaZ(n);

    AddHit(x, y, z, en, sx, sy, sz);
  }

  void RemoveHits();
  void MergeHits(Int_t n, Int_t m);

  void RemoveHit(int n);
  void SortByEnergy();
  void SwapHits(Int_t i, Int_t j);
  // Setters

  // Getters
  Double_t GetSigmaX(int n) { return fSigmaX[n]; }  // return value in mm
  Double_t GetSigmaY(int n) { return fSigmaY[n]; }  // return value in mm
  Double_t GetSigmaZ(int n) { return fSigmaZ[n]; }  // return value in mm

  TVector3 GetSigma(int n);

  void PrintHits();

  Double_t GetClusterSize(int n) {
    return TMath::Sqrt(fSigmaX[n] * fSigmaX[n] + fSigmaY[n] * fSigmaY[n] +
                       fSigmaZ[n] * fSigmaZ[n]);
  }
  Double_t GetXYSize(int n) {
    return TMath::Sqrt(fSigmaX[n] * fSigmaX[n] + fSigmaY[n] * fSigmaY[n]);
  }

  // Construtor
  TRestVolumeHits();
  // Destructor
  ~TRestVolumeHits();

  ClassDef(TRestVolumeHits, 1);
};
#endif
