//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHits.h
///
///             Event class to store hits
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///
///		        nov 2015:
///		            Changed vectors fX fY fZ and fEnergy from <Int_t> to
///< Float_t> 	                JuanAn Garcia
///
//////////////////////////////////////////////////////////////////////////

#ifndef TRestSoft_TRestHits
#define TRestSoft_TRestHits

#include <iostream>

#include "TVector3.h"

#include "TRestHit.h"

class TRestHits {
   public:

    Double_t fTotEnergy;  ///< Event total energy
    std::vector<TRestHit> fHits;

   public:
    //! Changes the origin of the Cartesian coordinate system
    void Translate(Int_t n, Double_t x, Double_t y, Double_t z);
    /// Event is rotated in XYZ.
    void RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma,
                    TVector3 vMean);  // vMean is the mean position of the event
                                      // from GetMeanPosition()
    /// Rotation around an arbitrary axis vAxis
    void Rotate(Int_t n, Double_t alpha, TVector3 vAxis,
                TVector3 vMean);  // vMean is the mean position of the event from
                                  // GetMeanPosition()

    void AddHit(const Double_t &x, const Double_t &y, const Double_t &z, const Double_t &en, Double_t t = 0, TString = "XYZ");
    void AddHit(const TVector3 &pos, const Double_t &en, Double_t t = 0, TString type = "XYZ");
    void AddHit(const TRestHit &hit);
    void AddHit(TRestHits& hits, Int_t n);

    TRestHit GetHit(Int_t n){return fHits.at(n);}

    void RemoveHits();

    Int_t GetMostEnergeticHitInRange(Int_t n, Int_t m);

    Double_t GetMaximumHitDistance();
    Double_t GetMaximumHitDistance2();

    virtual void MergeHits(int n, int m);
    virtual void SwapHits(Int_t i, Int_t j);
    virtual void RemoveHit(int n);

    virtual Bool_t areXY();
    virtual Bool_t areXZ();
    virtual Bool_t areYZ();
    virtual Bool_t areXYZ();

    Bool_t isNaN(Int_t n);

    Double_t GetDistanceToNode(Int_t n);

    Bool_t isSortedByEnergy();

    Int_t GetNumberOfHits() { return fHits.size(); }

    Double_t GetX(int n) { return fHits.at(n).fX; }     // return value in mm
    Double_t GetY(int n) { return fHits.at(n).fY; }     // return value in mm
    Double_t GetZ(int n) { return fHits.at(n).fZ; }     // return value in mm
    Double_t GetTime(int n) { return fHits.at(n).fT; }  // return value in us
    TString GetType(int n) { return fHits.at(n).fType; }

    TVector3 GetPosition(int n);
    TVector3 GetVector(int i, int j);

    Int_t GetNumberOfHitsX();
    Int_t GetNumberOfHitsY();

    Double_t GetMeanPositionX();
    Double_t GetMeanPositionY();
    Double_t GetMeanPositionZ();
    TVector3 GetMeanPosition();

    Double_t GetSigmaXY2();
    Double_t GetSigmaX();
    Double_t GetSigmaY();
    Double_t GetSigmaZ2();
    Double_t GetSkewXY();
    Double_t GetSkewZ();

    Double_t GetGaussSigmaX();
    Double_t GetGaussSigmaY();
    Double_t GetGaussSigmaZ();

    Double_t GetEnergyX();
    Double_t GetEnergyY();

    Double_t GetEnergy(int n) { return ((Double_t)fHits.at(n).fEnergy); }  // return value in keV

    Bool_t isHitNInsidePrism(Int_t n, TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                             Double_t theta);
    Int_t GetNumberOfHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta);
    Double_t GetEnergyInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);
    Double_t GetMeanPositionXInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta);
    Double_t GetMeanPositionYInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta);
    Double_t GetMeanPositionZInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta);
    TVector3 GetMeanPositionInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta);

    Bool_t isHitNInsideCylinder(Int_t n, TVector3 x0, TVector3 x1, Double_t radius);

    Int_t GetNumberOfHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Int_t GetNumberOfHitsInsideCylinder(Int_t i, Int_t j, Double_t radius);

    Double_t GetEnergyInCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetEnergyInCylinder(Int_t i, Int_t j, Double_t radius);
    Double_t GetMeanPositionXInCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetMeanPositionYInCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    Double_t GetMeanPositionZInCylinder(TVector3 x0, TVector3 x1, Double_t radius);
    TVector3 GetMeanPositionInCylinder(TVector3 x0, TVector3 x1, Double_t radius);

    Bool_t isHitNInsideSphere(Int_t n, TVector3 pos0, Double_t radius);
    Bool_t isHitNInsideSphere(Int_t n, Double_t x0, Double_t y0, Double_t z0, Double_t radius);

    Double_t GetEnergyInSphere(TVector3 pos0, Double_t radius);
    Double_t GetEnergyInSphere(Double_t x, Double_t y, Double_t z, Double_t radius);

    Double_t GetMaximumHitEnergy();
    Double_t GetMinimumHitEnergy();
    Double_t GetMeanHitEnergy();

    Double_t GetEnergyIntegral();
    Double_t GetTotalDepositedEnergy() { return fTotEnergy; }
    Double_t GetTotalEnergy() { return fTotEnergy; }
    Double_t GetEnergy() { return fTotEnergy; }
    Double_t GetDistance2(int n, int m);
    Double_t GetDistance(int N, int M) { return TMath::Sqrt(GetDistance2(N, M)); }
    Double_t GetTotalDistance();

    Double_t GetHitsPathLength(Int_t n = 0, Int_t m = 0);

    Double_t GetHitsTwist(Int_t n, Int_t m);
    Double_t GetHitsTwistWeighted(Int_t n, Int_t m);

    Int_t GetClosestHit(TVector3 position);

    TVector2 GetProjection(Int_t n, Int_t m, TVector3 position);

    Double_t GetTransversalProjection(TVector3 p0, TVector3 direction, TVector3 position);

    void WriteHitsToTextFile(TString filename);

    virtual void PrintHits(Int_t nHits = -1);

    // Construtor
    TRestHits();
    // Destructor
    ~TRestHits();

    ClassDef(TRestHits, 6);
};

#endif
