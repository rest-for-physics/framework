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

#include <TObject.h>
#include <TVector3.h>

#include <iostream>

#include "TRestHits.h"

class TRestVolumeHits : public TRestHits {
   protected:
    std::vector<Float_t> fSigmaX;  // [fNHits] Sigma on X axis for each volume hit (units microms)
    std::vector<Float_t> fSigmaY;  // [fNHits] Sigma on Y axis for each volume hit (units microms)
    std::vector<Float_t> fSigmaZ;  // [fNHits] Sigma on Z axis for each volume hit (units microms)

   public:
    void AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t time, REST_HitType type,
                Double_t sigmaX, Double_t sigmaY, Double_t sigmaZ);
    void AddHit(const TVector3& pos, Double_t en, Double_t time, REST_HitType type, const TVector3& sigma);
    void AddHit(const TRestVolumeHits& hits, Int_t n);

    void RemoveHits();
    void MergeHits(Int_t n, Int_t m);

    void RemoveHit(int n);
    void SortByEnergy();
    void SwapHits(Int_t i, Int_t j);

    Bool_t areXY() const;
    Bool_t areXZ() const;
    Bool_t areYZ() const;
    Bool_t areXYZ() const;

    // Setters

    // Getters
    inline Double_t GetSigmaX(int n) const { return fSigmaX[n]; }  // return value in mm
    inline Double_t GetSigmaY(int n) const { return fSigmaY[n]; }  // return value in mm
    inline Double_t GetSigmaZ(int n) const { return fSigmaZ[n]; }  // return value in mm

    TVector3 GetSigma(int n) const;

    void PrintHits() const;

    inline Double_t GetClusterSize(int n) const {
        return TMath::Sqrt(fSigmaX[n] * fSigmaX[n] + fSigmaY[n] * fSigmaY[n] + fSigmaZ[n] * fSigmaZ[n]);
    }
    inline Double_t GetXYSize(int n) const {
        return TMath::Sqrt(fSigmaX[n] * fSigmaX[n] + fSigmaY[n] * fSigmaY[n]);
    }

    // Constructor & Destructor
    TRestVolumeHits();
    ~TRestVolumeHits();

    ClassDef(TRestVolumeHits, 2);
};
#endif
