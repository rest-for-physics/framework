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
///             events. We should make sure that all the hit points in it is
///             with same type. e.g. all XZ hits
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________

#ifndef RestCore_TRestVolumeHits
#define RestCore_TRestVolumeHits


#include <iostream>

#include "TRestHits.h"

class TRestVolumeHits : public TRestHits {
   protected:
    std::vector<Float_t> fSigmaX;  // [fNHits] Sigma on X axis for each volume hit (units microms)
    std::vector<Float_t> fSigmaY;  // [fNHits] Sigma on Y axis for each volume hit (units microms)
    std::vector<Float_t> fSigmaZ;  // [fNHits] Sigma on Z axis for each volume hit (units microms)

   public:
    void AddHit(const Double_t &x, const Double_t &y, const Double_t &z, const Double_t &en, const Double_t &time, const TString &type, const Double_t &sigmax, const Double_t &sigmay, const Double_t &sigmaz);
    void AddHit(const TVector3 &pos, const Double_t &en, const Double_t &time, const TString &type, const TVector3 &sigma);
    void AddHit(TRestVolumeHits& hits, Int_t n);

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
        return TMath::Sqrt(fSigmaX[n] * fSigmaX[n] + fSigmaY[n] * fSigmaY[n] + fSigmaZ[n] * fSigmaZ[n]);
    }
    Double_t GetXYSize(int n) { return TMath::Sqrt(fSigmaX[n] * fSigmaX[n] + fSigmaY[n] * fSigmaY[n]); }

    // Constructor & Destructor
    TRestVolumeHits();
    ~TRestVolumeHits();

    ClassDef(TRestVolumeHits, 2);
};
#endif
