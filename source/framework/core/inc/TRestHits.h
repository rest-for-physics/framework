/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef TRestSoft_TRestHits
#define TRestSoft_TRestHits

#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TVector3.h>

#include <iostream>

enum REST_HitType { unknown = -1, X = 2, Y = 3, Z = 5, XY = 6, XZ = 10, YZ = 15, XYZ = 30, VETO = 100 };

/// It saves a 3-coordinate position and an energy for each punctual deposition.
class TRestHits {
   protected:
    // TODO: This is no longer used, it should be removed
    size_t fNHits = 0;  ///< Number of punctual energy depositions, it is the length for all the arrays
    // TODO: This is no longer used, it should be removed
    Double_t fTotalEnergy = 0;  ///< Event total energy

    /// Position on X axis for each punctual deposition (units mm)
    std::vector<Float_t> fX;  // [fNHits]

    /// Position on Y axis for each punctual deposition (units mm)
    std::vector<Float_t> fY;  // [fNHits]

    /// Position on Z axis for each punctual deposition (units mm)
    std::vector<Float_t> fZ;  // [fNHits]

    /// Absolute time information for each punctual deposition (units us, 0 is time of decay)
    std::vector<Float_t> fTime;  // [fNHits]

    /// Energy deposited at each 3-coordinate position (units keV)
    std::vector<Float_t> fEnergy;  // [fNHits]

    /// The type of hit X,Y,XY,XYZ, ...
    std::vector<REST_HitType> fType;

   public:
    void Translate(Int_t n, Double_t x, Double_t y, Double_t z);
    void RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma, const TVector3& center);
    void Rotate(Int_t n, Double_t alpha, const TVector3& vAxis, const TVector3& vMean);

    void AddHit(const TVector3& position, Double_t energy, Double_t time = 0, REST_HitType type = XYZ);
    void AddHit(TRestHits& hits, Int_t n);

    Int_t GetMostEnergeticHitInRange(Int_t n, Int_t m) const;

    Double_t GetMaximumHitDistance() const;
    Double_t GetMaximumHitDistance2() const;

    virtual void RemoveHits();
    virtual void MergeHits(int n, int m);
    virtual void SwapHits(Int_t i, Int_t j);
    virtual void RemoveHit(int n);

    virtual Bool_t areXY() const;
    virtual Bool_t areXZ() const;
    virtual Bool_t areYZ() const;
    virtual Bool_t areXYZ() const;

    Bool_t isNaN(Int_t n) const;

    Double_t GetDistanceToNode(Int_t n) const;

    Bool_t isSortedByEnergy() const;

    inline size_t GetNumberOfHits() const { return fEnergy.size(); }

    inline const std::vector<Float_t>& GetX() const { return fX; }
    inline const std::vector<Float_t>& GetY() const { return fY; }
    inline const std::vector<Float_t>& GetZ() const { return fZ; }
    inline const std::vector<Float_t>& GetTime() const { return fTime; }
    inline const std::vector<Float_t>& GetEnergyVector() const { return fEnergy; }

    inline Double_t GetX(int n) const { return fX[n]; }            // return value in mm
    inline Double_t GetY(int n) const { return fY[n]; }            // return value in mm
    inline Double_t GetZ(int n) const { return fZ[n]; }            // return value in mm
    inline Double_t GetTime(int n) const { return fTime[n]; }      // return value in us
    inline Double_t GetEnergy(int n) const { return fEnergy[n]; }  // return value in keV

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

    Double_t GetGaussSigmaX(Double_t error = 150.0, Int_t nHitsMin = 100000);
    Double_t GetGaussSigmaY(Double_t error = 150.0, Int_t nHitsMin = 100000);
    Double_t GetGaussSigmaZ(Double_t error = 150.0, Int_t nHitsMin = 100000);

    Double_t GetEnergyX() const;
    Double_t GetEnergyY() const;

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

    Double_t GetTotalEnergy() const;
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

    static void GetBoundaries(std::vector<double>& val, double& max, double& min, int& nBins,
                              double offset = 10);

    virtual void PrintHits(Int_t nHits = -1) const;

    class TRestHits_Iterator {
       public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = TRestHits_Iterator;
        using difference_type = int;
        using pointer = float*;
        using reference = float&;

        reference operator*() {
            return isAccessor ? x() : fHits->fX[index]; // Replace with the appropriate member access
        }

       private:
        int maxIndex = 0;
        int index = 0;
        TRestHits* fHits = nullptr;
        bool isAccessor = false;
        float _x;
        float _y;
        float _z;
        float _t;
        float _e;
        REST_HitType _type;

       public:
        float& x() { return isAccessor ? _x : fHits->fX[index]; }
        float& y() { return isAccessor ? _y : fHits->fY[index]; }
        float& z() { return isAccessor ? _z : fHits->fZ[index]; }
        float& t() { return isAccessor ? _t : fHits->fTime[index]; }
        float& e() { return isAccessor ? _e : fHits->fEnergy[index]; }
        REST_HitType& type() { return isAccessor ? _type : fHits->fType[index]; }

        float x() const { return isAccessor ? _x : fHits->fX[index]; }
        float y() const { return isAccessor ? _y : fHits->fY[index]; }
        float z() const { return isAccessor ? _z : fHits->fZ[index]; }
        float t() const { return isAccessor ? _t : fHits->fTime[index]; }
        float e() const { return isAccessor ? _e : fHits->fEnergy[index]; }
        REST_HitType type() const { return isAccessor ? _type : fHits->fType[index]; }

        // accessor: hit data is copied to self. The class acts like "TRestHit"
        void toaccessor();

        TRestHits_Iterator operator*() const;
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
            // default comparison logic
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
    inline TRestHits_Iterator begin() { return {this, 0}; }
    inline TRestHits_Iterator end() { return {this, static_cast<int>(GetNumberOfHits())}; }
    typedef TRestHits_Iterator iterator;

    TRestHits();
    ~TRestHits();

    ClassDef(TRestHits, 6);
};

#endif
