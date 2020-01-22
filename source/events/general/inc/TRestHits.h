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

#include <TVector3.h>
#include "TArrayD.h"
#include "TArrayI.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TObject.h"

//! It let save an event as a set of punctual deposition.
//! It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHits : public TObject {
   private:
    enum REST_HitType { unknown = -1, XZ = 1, YZ = 2, XY = 3, XYZ = 0 };

   public:
    Int_t fNHits;         ///< Number of punctual energy depositions, it is the lenght
                          ///< for all the array
    Double_t fTotEnergy;  ///< Event total energy

    std::vector<Float_t> fX;       // [fNHits] Position on X axis for each punctual
                                   // deposition (units mm)
    std::vector<Float_t> fY;       // [fNHits] Position on Y axis for each punctual
                                   // deposition (units mm)
    std::vector<Float_t> fZ;       // [fNHits] Position on Z axis for each punctual
                                   // deposition (units mm)
    std::vector<Float_t> fT;       // [fNHits] Absolute time information for each punctual deposition
                                   // (units us, 0 is time of decay)
    std::vector<Float_t> fEnergy;  // [fNHits] Energy deposited at each
                                   // 3-coordinate position (units keV)
    REST_HitType fType;            //!
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

    void AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t = 0);
    void AddHit(TVector3 pos, Double_t en, Double_t t = 0);
    void AddHit(TRestHits& hits, Int_t n);

    void RemoveHits();

    Int_t GetMostEnergeticHitInRange(Int_t n, Int_t m);

    Double_t GetMaximumHitDistance();
    Double_t GetMaximumHitDistance2();

    virtual void MergeHits(int n, int m);
    virtual void SwapHits(Int_t i, Int_t j);
    virtual void RemoveHit(int n);

    Bool_t areXY();
    Bool_t areXZ();
    Bool_t areYZ();
    Bool_t areXYZ();

    Bool_t isNaN(Int_t n);

    void GetXArray(Float_t* x);
    void GetYArray(Float_t* y);
    void GetZArray(Float_t* z);

    void InitializeXArray(Float_t x = 0);
    void InitializeYArray(Float_t y = 0);
    void InitializeZArray(Float_t z = 0);

    Double_t GetDistanceToNode(Int_t n);

    Bool_t isSortedByEnergy();

    Int_t GetNumberOfHits() { return fNHits; }

    Double_t GetX(int n) { return ((Double_t)fX[n]); }     // return value in mm
    Double_t GetY(int n) { return ((Double_t)fY[n]); }     // return value in mm
    Double_t GetZ(int n) { return ((Double_t)fZ[n]); }     // return value in mm
    Double_t GetTime(int n) { return ((Double_t)fT[n]); }  // return value in us

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

    Double_t GetEnergyX();
    Double_t GetEnergyY();

    Double_t GetEnergy(int n) { return ((Double_t)fEnergy[n]); }  // return value in keV

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

    virtual void PrintHits(Int_t nHits = -1);

    // Construtor
    TRestHits();
    // Destructor
    ~TRestHits();

    ClassDef(TRestHits, 4);
};
#endif
