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

#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TVector3.h>

#include <iostream>

#include "TArrayD.h"
#include "TArrayI.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TObject.h"

enum REST_HitType { unknown = -1, X = 2, Y = 3, Z = 5, XY = 6, XZ = 10, YZ = 15, XYZ = 30 };
//! It let save an event as a set of punctual deposition.
//! It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHits : public TObject {
   public:
    Int_t fNHits;         ///< Number of punctual energy depositions, it is the length
                          ///< for all the array
    Double_t fTotEnergy;  ///< Event total energy

    std::vector<Float_t> fX;          // [fNHits] Position on X axis for each punctual
                                      // deposition (units mm)
    std::vector<Float_t> fY;          // [fNHits] Position on Y axis for each punctual
                                      // deposition (units mm)
    std::vector<Float_t> fZ;          // [fNHits] Position on Z axis for each punctual
                                      // deposition (units mm)
    std::vector<Float_t> fT;          // [fNHits] Absolute time information for each punctual deposition
                                      // (units us, 0 is time of decay)
    std::vector<Float_t> fEnergy;     // [fNHits] Energy deposited at each
                                      // 3-coordinate position (units keV)
    std::vector<REST_HitType> fType;  //

   public:
    //! Changes the origin of the Cartesian coordinate system
    void Translate(Int_t n, Double_t x, Double_t y, Double_t z);
    /// Event is rotated in XYZ.
    void RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma,
                    const TVector3& vMean);  // vMean is the mean position of the event from GetMeanPosition()
    /// Rotation around an arbitrary axis vAxis
    void Rotate(Int_t n, Double_t alpha, const TVector3& vAxis,
                const TVector3& vMean);  // vMean is the mean position of the event from GetMeanPosition()

    void AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t = 0, REST_HitType type = XYZ);
    void AddHit(const TVector3& pos, Double_t en, Double_t t = 0, REST_HitType type = XYZ);
    void AddHit(TRestHits& hits, Int_t n);

    void RemoveHits();

    Int_t GetMostEnergeticHitInRange(Int_t n, Int_t m) const;

    Double_t GetMaximumHitDistance() const;
    Double_t GetMaximumHitDistance2() const;

    virtual void MergeHits(int n, int m);
    virtual void SwapHits(Int_t i, Int_t j);
    virtual void RemoveHit(int n);

    virtual Bool_t areXY() const;
    virtual Bool_t areXZ() const;
    virtual Bool_t areYZ() const;
    virtual Bool_t areXYZ() const;

    Bool_t isNaN(Int_t n) const;

    void GetXArray(Float_t* x) const;
    void GetYArray(Float_t* y) const;
    void GetZArray(Float_t* z) const;

    void InitializeXArray(Float_t x = 0);
    void InitializeYArray(Float_t y = 0);
    void InitializeZArray(Float_t z = 0);

    Double_t GetDistanceToNode(Int_t n) const;

    Bool_t isSortedByEnergy() const;

    inline Int_t GetNumberOfHits() const { return fNHits; }

    inline Double_t GetX(int n) const { return ((Double_t)fX[n]); }     // return value in mm
    inline Double_t GetY(int n) const { return ((Double_t)fY[n]); }     // return value in mm
    inline Double_t GetZ(int n) const { return ((Double_t)fZ[n]); }     // return value in mm
    inline Double_t GetTime(int n) const { return ((Double_t)fT[n]); }  // return value in us
    inline REST_HitType GetType(int n) const { return fType[n]; }

    TVector3 GetPosition(int n) const;
    TVector3 GetVector(int i, int j) const;

    Int_t GetNumberOfHitsX() const;
    Int_t GetNumberOfHitsY() const;

    Double_t GetMeanPositionX() const;
    Double_t GetMeanPositionY() const;
    Double_t GetMeanPositionZ() const;
    TVector3 GetMeanPosition() const;

    Double_t GetSigmaXY2() const;
    Double_t GetSigmaX() const;
    Double_t GetSigmaY() const;
    Double_t GetSigmaZ2() const;
    Double_t GetSkewXY() const;
    Double_t GetSkewZ() const;

    Double_t GetGaussSigmaX();
    Double_t GetGaussSigmaY();
    Double_t GetGaussSigmaZ();

    Double_t GetEnergyX() const;
    Double_t GetEnergyY() const;

    inline Double_t GetEnergy(int n) const { return ((Double_t)fEnergy[n]); }  // return value in keV

    Bool_t isHitNInsidePrism(Int_t n, const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                             Double_t theta) const;
    Int_t GetNumberOfHitsInsidePrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta) const;
    Double_t GetEnergyInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                              Double_t theta) const;
    Double_t GetMeanPositionXInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta) const;
    Double_t GetMeanPositionYInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta) const;
    Double_t GetMeanPositionZInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta) const;
    TVector3 GetMeanPositionInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                                    Double_t theta) const;

    Bool_t isHitNInsideCylinder(Int_t n, const TVector3& x0, const TVector3& x1, Double_t radius) const;

    Int_t GetNumberOfHitsInsideCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const;
    Int_t GetNumberOfHitsInsideCylinder(Int_t i, Int_t j, Double_t radius) const;

    Double_t GetEnergyInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const;
    Double_t GetEnergyInCylinder(Int_t i, Int_t j, Double_t radius) const;
    Double_t GetMeanPositionXInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const;
    Double_t GetMeanPositionYInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const;
    Double_t GetMeanPositionZInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const;
    TVector3 GetMeanPositionInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const;

    Bool_t isHitNInsideSphere(Int_t n, const TVector3& pos0, Double_t radius) const;
    Bool_t isHitNInsideSphere(Int_t n, Double_t x0, Double_t y0, Double_t z0, Double_t radius) const;

    Double_t GetEnergyInSphere(const TVector3& pos0, Double_t radius) const;
    Double_t GetEnergyInSphere(Double_t x, Double_t y, Double_t z, Double_t radius) const;

    Double_t GetMaximumHitEnergy() const;
    Double_t GetMinimumHitEnergy() const;
    Double_t GetMeanHitEnergy() const;

    Double_t GetEnergyIntegral() const;
    inline Double_t GetTotalDepositedEnergy() const { return fTotEnergy; }
    inline Double_t GetTotalEnergy() const { return fTotEnergy; }
    inline Double_t GetEnergy() const { return fTotEnergy; }
    Double_t GetDistance2(int n, int m) const;
    inline Double_t GetDistance(int N, int M) const { return TMath::Sqrt(GetDistance2(N, M)); }
    Double_t GetTotalDistance() const;

    Double_t GetHitsPathLength(Int_t n = 0, Int_t m = 0) const;

    Double_t GetHitsTwist(Int_t n, Int_t m) const;
    Double_t GetHitsTwistWeighted(Int_t n, Int_t m) const;

    Int_t GetClosestHit(const TVector3& position) const;

    TVector2 GetProjection(Int_t n, Int_t m, const TVector3& position) const;

    Double_t GetTransversalProjection(const TVector3& p0, const TVector3& direction,
                                      const TVector3& position) const;

    void WriteHitsToTextFile(TString filename);

    virtual void PrintHits(Int_t nHits = -1) const;

    class TRestHits_Iterator : public std::iterator<std::random_access_iterator_tag, TRestHits_Iterator> {
       private:
        int maxindex = 0;
        int index = 0;
        TRestHits* fHits = nullptr;
        bool isaccessor = false;
        float _x;
        float _y;
        float _z;
        float _t;
        float _e;
        REST_HitType _type;

       public:
        float& x() { return isaccessor ? _x : fHits->fX[index]; }
        float& y() { return isaccessor ? _y : fHits->fY[index]; }
        float& z() { return isaccessor ? _z : fHits->fZ[index]; }
        float& t() { return isaccessor ? _t : fHits->fT[index]; }
        float& e() { return isaccessor ? _e : fHits->fEnergy[index]; }
        REST_HitType& type() { return isaccessor ? _type : fHits->fType[index]; }

        float x() const { return isaccessor ? _x : fHits->fX[index]; }
        float y() const { return isaccessor ? _y : fHits->fY[index]; }
        float z() const { return isaccessor ? _z : fHits->fZ[index]; }
        float t() const { return isaccessor ? _t : fHits->fT[index]; }
        float e() const { return isaccessor ? _e : fHits->fEnergy[index]; }
        REST_HitType type() const { return isaccessor ? _type : fHits->fType[index]; }

        void toaccessor();

        TRestHits_Iterator operator*();
        TRestHits_Iterator& operator++();
        TRestHits_Iterator& operator+=(const int& n);
        TRestHits_Iterator operator+(const int& n);
        TRestHits_Iterator& operator--();
        TRestHits_Iterator& operator-=(const int& n);
        TRestHits_Iterator operator-(const int& n);
        TRestHits_Iterator& operator=(const TRestHits_Iterator& iter);

        friend int operator-(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            return i1.index - i2.index;
        }
        friend bool operator==(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            return i1.fHits == i2.fHits && i1.index == i2.index;
        }
        friend bool operator!=(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            return i1.fHits != i2.fHits || i1.index != i2.index;
        }
        friend bool operator>(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            // default comparsion logic
            return i1.index > i2.index;
        }
        friend bool operator>=(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            // default comparison logic
            return i1.index >= i2.index;
        }
        friend bool operator<(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            return i1.index < i2.index;
        }
        friend bool operator<=(const TRestHits_Iterator& i1, const TRestHits_Iterator& i2) {
            return i1.index <= i2.index;
        }
        friend void swap(TRestHits::TRestHits_Iterator i1, TRestHits::TRestHits_Iterator i2) {
            if (i1.fHits == i2.fHits) {
                i1.fHits->SwapHits(i1.index, i2.index);
            }
        }

        TRestHits_Iterator(TRestHits* h, int _index);
    };
    inline TRestHits_Iterator begin() { return TRestHits_Iterator(this, 0); }
    inline TRestHits_Iterator end() { return TRestHits_Iterator(this, fNHits); }
    typedef TRestHits_Iterator iterator;

    // Constructor
    TRestHits();
    // Destructor
    ~TRestHits();

    ClassDef(TRestHits, 5);
};

#endif
