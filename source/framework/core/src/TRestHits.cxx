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

//////////////////////////////////////////////////////////////////////////
/// TRestHits is a generic data holder that defines an arbitrary physical quantity
/// (usually the energy) in a 3-dimensional space (x,y,z). However, it also defines
/// an optional time member that might be used to add additional information to the
/// spatial event, such as it could be the drift time in a TPC. However, the final
/// meaning of that member must be interpreted in the context of the event data
/// processing algorithms, and/or the data type using the hits, such as:
/// TRestGeant4Hits, TRestDetectorHits, ...
///
/// On top of that, we may also define the hit type in particular scenarios where
/// one of the spatial coordinates remains unknown, and we may have a REST_HitType
/// that defines a XY, XZ, XYZ, etc, hit type.
///
/// This class defines typical transformations required by spatially defined
/// physical quantities such as rotation or translation, basic hit distance
/// calculation, and hit operations such as merging, adding/removing or swaping
/// hits.
///
/// It contains also more sophisticated methods to perform physical calculations
/// and parameterize the properties of a group of hits or cluster such as
/// performing a gaussian fit to the hit distribution, such as TRestHits::GetGaussSigmaX,
/// where two hits are added, one to each side of the event, and a Gaussian is fitted.
/// The hits are added so that the fit works even for small events as shown in the figure below.
/// The parameter sigma is extracted from the fit and its absolute value is returned.
///
/// \htmlonly <style>div.image img[src="hitsGaussianFit.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the GetGaussSigmaX method and why two hits are added.](hitsGaussianFit.png)
///
/// Other methods determine the number of hits or the total energy contained in a particular
/// geometrical shape, see for example TRestHits::GetEnergyInCylinder, and different
/// physical quantities on such fiducialization, i.e. TRestHits::GetMeanPositionInPrism.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-Sep: First concept. Created as part of the conceptualization of existing
///           REST software.
/// \author  Javier Galan (javier.galan@unizar.es)
///
/// 2015-Nov: Changed vectors fX fY fZ and fEnergy from <Int_t> to < Float_t>.
/// \author   JuanAn Garcia (juanan318@gmail.com)
///
/// 2022-July: Introducing gausian hits fitting
/// \author    Cristina Margalejo (cmargalejo@unizar.es)
///
/// \class TRestHits
///
/// <hr>
///

#include "TRestHits.h"

#include <limits.h>
#include "TROOT.h"

using namespace std;
using namespace TMath;

ClassImp(TRestHits);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestHits::TRestHits() = default;

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestHits::~TRestHits() = default;

///////////////////////////////////////////////
/// \brief It will return true only if all the hits inside are of type XY.
///
Bool_t TRestHits::areXY() const {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != XY) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////
/// \brief It will return true only if all the hits inside are of type XZ.
///
Bool_t TRestHits::areXZ() const {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != XZ) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////
/// \brief It will return true only if all the hits inside are of type YZ.
///
Bool_t TRestHits::areYZ() const {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != YZ) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////
/// \brief It will return true only if all the hits inside are of type XYZ.
///
Bool_t TRestHits::areXYZ() const {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != XYZ) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////
/// \brief It will return true only if all the 3-coordinates of hit number
/// `n` are not a number,
///
Bool_t TRestHits::isNaN(Int_t n) const {
    if (IsNaN(GetX(n)) && IsNaN(GetY(n)) && IsNaN(GetZ(n))) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief It returns the added energy integral.
///
Double_t TRestHits::GetEnergyIntegral() const {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfHits(); i++) sum += GetEnergy(i);
    return sum;
}

///////////////////////////////////////////////
/// \brief It determines if hit `n` is contained inside a prisma delimited between `x0` and `y0`
/// vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate the
/// prism along its axis to give full freedom.
///
/// TODO: It seems to me there is a problem with the rotation of the hits, which are rotated
/// along Z axis, and not along the prism axis = x1-x0. As soon as the prism is aligned with Z
/// no problem though.
///
Bool_t TRestHits::isHitNInsidePrism(Int_t n, const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                    Double_t sizeY, Double_t theta) const {
    TVector3 axis = x1 - x0;

    Double_t prismLength = axis.Mag();

    TVector3 hitPos = this->GetPosition(n) - x0;
    hitPos.RotateZ(theta);
    Double_t l = axis.Dot(hitPos) / prismLength;

    if ((l > 0) && (l < prismLength))
        if ((TMath::Abs(hitPos.X()) < sizeX / 2) && (TMath::Abs(hitPos.Y()) < sizeY / 2)) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief It determines the total hit energy contained inside a prisma delimited between `x0` and `y0`
/// vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate the
/// prism along its axis to give full freedom.
///
Double_t TRestHits::GetEnergyInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta) const {
    Double_t energy = 0.;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) energy += this->GetEnergy(n);

    return energy;
}

///////////////////////////////////////////////
/// \brief It determines the total number of hits contained inside a prisma delimited between `x0`
/// and `y0` vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate
/// the prism along its axis to give full freedom.
///
Int_t TRestHits::GetNumberOfHitsInsidePrism(const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                            Double_t sizeY, Double_t theta) const {
    Int_t hits = 0;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) hits++;

    return hits;
}

///////////////////////////////////////////////
/// \brief It determines if hit `n` is contained inside a cylinder with a given `radius` and
/// delimited between `x0` and `x1` vertex.
///
Bool_t TRestHits::isHitNInsideCylinder(Int_t n, const TVector3& x0, const TVector3& x1,
                                       Double_t radius) const {
    TVector3 axis = x1 - x0;

    Double_t cylLength = axis.Mag();

    TVector3 hitPos = this->GetPosition(n) - x0;

    Double_t l = axis.Dot(hitPos) / cylLength;

    if (l > 0 && l < cylLength) {
        Double_t hitPosNorm2 = hitPos.Mag2();
        Double_t r = TMath::Sqrt(hitPosNorm2 - l * l);

        if (r < radius) return true;
    }

    return false;
}

///////////////////////////////////////////////
/// \brief It determines the total energy contained inside a cylinder with a given
/// `radius` and delimited between the hit number `i` and the hit number `j`.
///
Double_t TRestHits::GetEnergyInCylinder(Int_t i, Int_t j, Double_t radius) const {
    return GetEnergyInCylinder(this->GetPosition(i), this->GetPosition(j), radius);
}

///////////////////////////////////////////////
/// \brief It determines the total energy contained inside a cylinder with a given
/// `radius` and delimited between `x0` and `y0` vertex.
///
Double_t TRestHits::GetEnergyInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const {
    Double_t energy = 0.;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (isHitNInsideCylinder(n, x0, x1, radius)) energy += this->GetEnergy(n);
    }

    return energy;
}

///////////////////////////////////////////////
/// \brief It determines the total number of hits contained inside a cylinder with a given
/// `radius` and delimited between the hit number `i` and the hit number `j`.
///
Int_t TRestHits::GetNumberOfHitsInsideCylinder(Int_t i, Int_t j, Double_t radius) const {
    return GetNumberOfHitsInsideCylinder(this->GetPosition(i), this->GetPosition(j), radius);
}

///////////////////////////////////////////////
/// \brief It determines the total number of hits contained inside a cylinder with a given
/// `radius` and delimited between `x0` and `y0` vertex.
///
Int_t TRestHits::GetNumberOfHitsInsideCylinder(const TVector3& x0, const TVector3& x1,
                                               Double_t radius) const {
    Int_t hits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++)
        if (isHitNInsideCylinder(n, x0, x1, radius)) hits++;

    return hits;
}

///////////////////////////////////////////////
/// \brief It determines the total energy contained in a sphere with position `pos0` for
/// a given spherical `radius`.
///
Double_t TRestHits::GetEnergyInSphere(const TVector3& pos0, Double_t radius) const {
    return GetEnergyInSphere(pos0.X(), pos0.Y(), pos0.Z(), radius);
}

///////////////////////////////////////////////
/// \brief It determines the total energy contained in a sphere with position `x0`,`y0`
/// and `y0` for a given `radius`.
///
Double_t TRestHits::GetEnergyInSphere(Double_t x0, Double_t y0, Double_t z0, Double_t radius) const {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfHits(); i++) {
        Double_t x = this->GetPosition(i).X();
        Double_t y = this->GetPosition(i).Y();
        Double_t z = this->GetPosition(i).Z();

        Double_t dist = (x - x0) * (x - x0) + (y - y0) * (y - y0) + (z - z0) * (z - z0);

        if (dist < radius * radius) sum += GetEnergy(i);
    }
    return sum;
}

///////////////////////////////////////////////
/// \brief It determines if the hit `n` is contained in a sphere with position `pos0`
/// for a given sphereical `radius`.
///
Bool_t TRestHits::isHitNInsideSphere(Int_t n, const TVector3& pos0, Double_t radius) const {
    return isHitNInsideSphere(n, pos0.X(), pos0.Y(), pos0.Z(), radius);
}

///////////////////////////////////////////////
/// \brief It determines the total energy contained in a sphere with position `x0`,`y0`
/// and `y0` for a given `radius`.
///
Bool_t TRestHits::isHitNInsideSphere(Int_t n, Double_t x0, Double_t y0, Double_t z0, Double_t radius) const {
    Double_t x = this->GetPosition(n).X();
    Double_t y = this->GetPosition(n).Y();
    Double_t z = this->GetPosition(n).Z();

    Double_t dist = (x - x0) * (x - x0) + (y - y0) * (y - y0) + (z - z0) * (z - z0);

    if (dist < radius * radius) return kTRUE;

    return kFALSE;
}

///////////////////////////////////////////////
/// \brief Adds a new hit to the list of hits using explicit x,y,z values.
///
void TRestHits::AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t, REST_HitType type) {
    fNHits++;
    fX.push_back((Float_t)(x));
    fY.push_back((Float_t)(y));
    fZ.push_back((Float_t)(z));
    fTime.push_back((Float_t)t);
    fEnergy.push_back((Float_t)(en));
    fType.push_back(type);

    fTotalEnergy += en;
}

///////////////////////////////////////////////
/// \brief Adds a new hit to the list of hits using a TVector3.
///
void TRestHits::AddHit(const TVector3& pos, Double_t en, Double_t t, REST_HitType type) {
    fNHits++;

    fX.push_back((Float_t)(pos.X()));
    fY.push_back((Float_t)(pos.Y()));
    fZ.push_back((Float_t)(pos.Z()));
    fTime.push_back((Float_t)t);
    fEnergy.push_back((Float_t)(en));
    fType.push_back(type);

    fTotalEnergy += en;
}

///////////////////////////////////////////////
/// \brief Adds a new hit to the list of hits using the hit `n` inside another TRestHits object.
///
void TRestHits::AddHit(TRestHits& hits, Int_t n) {
    Double_t x = hits.GetX(n);
    Double_t y = hits.GetY(n);
    Double_t z = hits.GetZ(n);
    Double_t en = hits.GetEnergy(n);
    Double_t t = hits.GetTime(n);
    REST_HitType type = hits.GetType(n);

    AddHit(x, y, z, en, t, type);
}

///////////////////////////////////////////////
/// \brief It removes all hits inside the class.
///
void TRestHits::RemoveHits() {
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fTime.clear();
    fEnergy.clear();
    fType.clear();
    fTotalEnergy = 0;
}

///////////////////////////////////////////////
/// \brief It moves hit `n` by a given amount (x,y,z).
///
void TRestHits::Translate(Int_t n, double x, double y, double z) {
    fX[n] += x;
    fY[n] += y;
    fZ[n] += z;
}

///////////////////////////////////////////////
/// \brief It rotates hit `n` following rotations in Z, Y and X by angles gamma, beta and alpha. The
/// rotation is performed with center at `vMean`.
///
void TRestHits::RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma, const TVector3& vMean) {
    TVector3 position = GetPosition(n);
    TVector3 vHit = position - vMean;

    vHit.RotateZ(gamma);
    vHit.RotateY(beta);
    vHit.RotateX(alpha);

    fX[n] = vHit[0] + vMean[0];
    fY[n] = vHit[1] + vMean[1];
    fZ[n] = vHit[2] + vMean[2];
}

///////////////////////////////////////////////
/// \brief It rotates hit `n` by an angle akpha along the `vAxis` with center at `vMean`.
///
void TRestHits::Rotate(Int_t n, Double_t alpha, const TVector3& vAxis, const TVector3& vMean) {
    TVector3 vHit;

    vHit[0] = fX[n] - vMean[0];
    vHit[1] = fY[n] - vMean[1];
    vHit[2] = fZ[n] - vMean[2];

    vHit.Rotate(alpha, vAxis);

    fX[n] = vHit[0] + vMean[0];
    fY[n] = vHit[1] + vMean[1];
    fZ[n] = vHit[2] + vMean[2];
}

///////////////////////////////////////////////
/// \brief It returns the maximum hit energy
///
Double_t TRestHits::GetMaximumHitEnergy() const {
    Double_t energy = 0;
    for (int i = 0; i < GetNumberOfHits(); i++)
        if (GetEnergy(i) > energy) energy = GetEnergy(i);
    return energy;
}

///////////////////////////////////////////////
/// \brief It returns the minimum hit energy
///
Double_t TRestHits::GetMinimumHitEnergy() const {
    Double_t energy = GetMaximumHitEnergy();
    for (int i = 0; i < GetNumberOfHits(); i++)
        if (GetEnergy(i) < energy) energy = GetEnergy(i);
    return energy;
}

///////////////////////////////////////////////
/// \brief It returns the mean hits energy
///
Double_t TRestHits::GetMeanHitEnergy() const { return GetTotalEnergy() / GetNumberOfHits(); }

///////////////////////////////////////////////
/// \brief It merges hits `n` and `m` being the resulting hit placed at the weighted center
/// and being its final energy the addition of the energies of the hits `n` and `m`.
///
void TRestHits::MergeHits(int n, int m) {
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];
    fX[n] = (fX[n] * fEnergy[n] + fX[m] * fEnergy[m]) / totalEnergy;
    fY[n] = (fY[n] * fEnergy[n] + fY[m] * fEnergy[m]) / totalEnergy;
    fZ[n] = (fZ[n] * fEnergy[n] + fZ[m] * fEnergy[m]) / totalEnergy;
    fTime[n] = (fTime[n] * fEnergy[n] + fTime[m] * fEnergy[m]) / totalEnergy;
    fEnergy[n] += fEnergy[m];

    fX.erase(fX.begin() + m);
    fY.erase(fY.begin() + m);
    fZ.erase(fZ.begin() + m);
    fTime.erase(fTime.begin() + m);
    fEnergy.erase(fEnergy.begin() + m);
    fType.erase(fType.begin() + m);
    fNHits--;
}

///////////////////////////////////////////////
/// \brief It exchanges hits `n` and `m` affecting to the ordering of the hits inside the
/// list of hits.
///
void TRestHits::SwapHits(Int_t i, Int_t j) {
    iter_swap(fX.begin() + i, fX.begin() + j);
    iter_swap(fY.begin() + i, fY.begin() + j);
    iter_swap(fZ.begin() + i, fZ.begin() + j);
    iter_swap(fEnergy.begin() + i, fEnergy.begin() + j);
    iter_swap(fType.begin() + i, fType.begin() + j);
    iter_swap(fTime.begin() + i, fTime.begin() + j);
}

///////////////////////////////////////////////
/// \brief It returns true if the hits are ordered in increasing energies.
///
Bool_t TRestHits::isSortedByEnergy() const {
    for (int i = 0; i < GetNumberOfHits() - 1; i++)
        if (GetEnergy(i + 1) > GetEnergy(i)) return false;

    return true;
}

///////////////////////////////////////////////
/// \brief It removes the hit at position `n` from the list.
///
void TRestHits::RemoveHit(int n) {
    fTotalEnergy -= GetEnergy(n);
    fX.erase(fX.begin() + n);
    fY.erase(fY.begin() + n);
    fZ.erase(fZ.begin() + n);
    fTime.erase(fTime.begin() + n);
    fEnergy.erase(fEnergy.begin() + n);
    fType.erase(fType.begin() + n);
    fNHits--;
}

///////////////////////////////////////////////
/// \brief It returns the position of hit number `n`.
///
TVector3 TRestHits::GetPosition(int n) const {
    if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] == XY)) {
        return {(Double_t)fX[n], (Double_t)fY[n], 0};
    }
    if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] == XZ)) {
        return {(Double_t)fX[n], 0, (Double_t)fZ[n]};
    }
    if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] == YZ)) {
        return {0, (Double_t)fY[n], (Double_t)fZ[n]};
    }
    return {(Double_t)fX[n], (Double_t)fY[n], (Double_t)fZ[n]};
}

///////////////////////////////////////////////
/// \brief It returns the vector that goes from hit `j` to hit `i`.
///
TVector3 TRestHits::GetVector(int i, int j) const { return GetPosition(i) - GetPosition(j); }

///////////////////////////////////////////////
/// \brief It returns the number of hits with a valid X coordinate
///
Int_t TRestHits::GetNumberOfHitsX() const {
    Int_t nHitsX = 0;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0)) nHitsX++;

    return nHitsX;
}

///////////////////////////////////////////////
/// \brief It returns the number of hits with a valid Y coordinate
///
Int_t TRestHits::GetNumberOfHitsY() const {
    Int_t nHitsY = 0;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0)) nHitsY++;

    return nHitsY;
}

///////////////////////////////////////////////
/// \brief It calculates the total energy of hits with a valid X coordinate
///
Double_t TRestHits::GetEnergyX() const {
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0)) {
            totalEnergy += fEnergy[n];
        }
    }

    return totalEnergy;
}

///////////////////////////////////////////////
/// \brief It calculates the total energy of hits with a valid Y coordinate
///
Double_t TRestHits::GetEnergyY() const {
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0)) {
            totalEnergy += fEnergy[n];
        }
    }

    return totalEnergy;
}

///////////////////////////////////////////////
/// \brief It calculates the mean X position weighting with the energy of the
/// hits with a valid X coordinate
///
Double_t TRestHits::GetMeanPositionX() const {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0)) {
            meanX += fX[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    if (totalEnergy == 0) return 0;
    meanX /= totalEnergy;

    return meanX;
}

///////////////////////////////////////////////
/// \brief It calculates the mean Y position weighting with the energy of the
/// hits with a valid Y coordinate
///
Double_t TRestHits::GetMeanPositionY() const {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0)) {
            meanY += fY[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    if (totalEnergy == 0) return 0;
    meanY /= totalEnergy;

    return meanY;
}

///////////////////////////////////////////////
/// \brief It calculates the mean Z position weighting with the energy of the
/// hits with a valid Z coordinate
///
Double_t TRestHits::GetMeanPositionZ() const {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (!IsNaN(fZ[n])) {
            meanZ += fZ[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    if (totalEnergy == 0) return 0;
    meanZ /= totalEnergy;

    return meanZ;
}

///////////////////////////////////////////////
/// \brief It calculates the mean position weighting with the energy of the
/// hits. Each coordinate is calculated considering the valid coordinates of
/// each hit component.
///
TVector3 TRestHits::GetMeanPosition() const {
    TVector3 mean(GetMeanPositionX(), GetMeanPositionY(), GetMeanPositionZ());
    return mean;
}

///////////////////////////////////////////////
/// \brief It calculates the 2-dimensional hits variance.
///
Double_t TRestHits::GetSigmaXY2() const {
    Double_t sigmaXY2 = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanX = this->GetMeanPositionX();
    Double_t meanY = this->GetMeanPositionY();
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0))
            sigmaXY2 += fEnergy[n] * (meanY - fY[n]) * (meanY - fY[n]);
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0))
            sigmaXY2 += fEnergy[n] * (meanX - fX[n]) * (meanX - fX[n]);
    }
    return sigmaXY2 /= totalEnergy;
}

///////////////////////////////////////////////
/// \brief It calculates the hits standard deviation in the X-coordinate
///
Double_t TRestHits::GetSigmaX() const {
    Double_t sigmaX2 = 0;
    Double_t sigmaX = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanX = this->GetMeanPositionX();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0))
            sigmaX2 += fEnergy[n] * (meanX - fX[n]) * (meanX - fX[n]);
    }
    sigmaX2 /= totalEnergy;

    return sigmaX = TMath::Sqrt(sigmaX2);
}

///////////////////////////////////////////////
/// \brief It calculates the hits standard deviation in the Y-coordinate
///
Double_t TRestHits::GetSigmaY() const {
    Double_t sigmaY2 = 0;
    Double_t sigmaY = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanY = this->GetMeanPositionY();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0))
            sigmaY2 += fEnergy[n] * (meanY - fY[n]) * (meanY - fY[n]);
    }
    sigmaY2 /= totalEnergy;

    return sigmaY = TMath::Sqrt(sigmaY2);
}

///////////////////////////////////////////////
/// \brief It writes the hits to a plain text file
///
void TRestHits::WriteHitsToTextFile(TString filename) {
    FILE* fff = fopen(filename.Data(), "w");
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if ((fType.size() == 0 ? !IsNaN(fX[i]) : fType[i] % X == 0))
            fprintf(fff, "%d\t%e\t%s\t%e\t%e\n", i, fX[i], "NaN", fZ[i], fEnergy[i]);
        if ((fType.size() == 0 ? !IsNaN(fY[i]) : fType[i] % Y == 0))
            fprintf(fff, "%d\t%s\t%e\t%e\t%e\n", i, "NaN", fY[i], fZ[i], fEnergy[i]);
    }
    fclose(fff);
}

///////////////////////////////////////////////
/// \brief TODO This method is not using any TRestHits member. This probably means that it should
/// be placed somewhere else.
///
void TRestHits::GetBoundaries(std::vector<double>& dist, double& max, double& min, int& nBins,
                              double offset) {
    std::sort(dist.begin(), dist.end());
    max = dist.back();
    min = dist.front();

    double minDiff = 1E6;
    double prevVal = 1E6;
    for (const auto& h : dist) {
        double diff = std::abs(h - prevVal);
        if (diff > 0 && diff < minDiff) minDiff = diff;
        prevVal = h;
    }

    max += offset * minDiff + minDiff / 2.;
    min -= offset * minDiff + minDiff / 2.;
    nBins = std::round((max - min) / minDiff);
}
///////////////////////////////////////////////
/// \brief It computes the gaussian sigma in the X-coordinate.
/// It adds a hit to the right and a hit to the left, with energy = 0 +/- 70 ADC.
/// Then it fits a gaussian to the hits and extracts the sigma. The hits are just added
/// for fitting purposes and do not go into any further processing.
///
Double_t TRestHits::GetGaussSigmaX() {
    Double_t gausSigmaX = 0;
    Int_t nHits = GetNumberOfHits();
    if (nHits <= 0) {
        gausSigmaX = 0;
    } else {
        Int_t nAdd = 0;
        bool doHitCorrection = true;
        // bool doHitCorrection = nHits <= 18; //in case we want to apply it only to the smaller events
        if (doHitCorrection) {
            nAdd = 2;
        }
        Int_t nElems = nHits + nAdd;
        vector<Double_t> x(nElems), y(nElems), ex(nElems), ey(nElems);
        Int_t k = nAdd / 2;
        Double_t xMin = std::numeric_limits<double>::max();
        Double_t xMax = std::numeric_limits<double>::lowest();
        for (int n = 0; n < GetNumberOfHits(); k++, n++) {
            x[k] = fX[n];
            y[k] = fEnergy[n];
            ex[k] = 0;
            xMin = min(xMin, x[k]);
            xMax = max(xMax, x[k]);
            if (y[k] != 0) {
                ey[k] = 10 * sqrt(y[k]);
            } else {
                ey[k] = 0;
            }
        }
        Int_t h = nHits + nAdd / 2;
        if (doHitCorrection) {
            x[0] = xMin - 0.5;
            x[h] = xMax + 0.5;
            y[0] = 0.0;
            y[h] = 0.0;
            ex[0] = 0.0;
            ex[h] = 0.0;
            ey[0] = 70.0;
            ey[h] = 70.0;
        }
        TGraphErrors* grX = new TGraphErrors(nElems, &x[0], &y[0], &ex[0], &ey[0]);
        // TCanvas *c = new TCanvas("c","X position fit",200,10,500,500);
        // grX->Draw();
        // Defining the starting parameters for the fit.
        Double_t maxY = MaxElement(nElems, grX->GetY());
        Double_t maxX = grX->GetX()[LocMax(nElems, grX->GetY())];
        Double_t sigma = abs(x[0] - x[h]) / 2.0;
        // std::cout << "maxX: " << maxX << ", maxY: " << maxY << ", sigma: " << sigma << std::endl;

        TF1* fit = new TF1("", "gaus");
        fit->SetParameter(0, maxY);
        fit->SetParameter(1, maxX);
        fit->SetParameter(2, sigma);
        grX->Fit(fit, "QNB");  // Q = quiet, no info in screen; N = no plot; B = no automatic start
                               // parameters; R = Use the Range specified in the function range

        gausSigmaX = fit->GetParameter(2);
    }

    return abs(gausSigmaX);
}
/// \brief It computes the gaussian sigma in the Y-coordinate.
/// It adds a hit to the right and a hit to the left, with energy = 0 +/- 70 ADC.
/// Then it fits a gaussian to the hits and extracts the sigma. The hits are just added
/// for fitting purposes and do not go into any further processing.
Double_t TRestHits::GetGaussSigmaY() {
    Double_t gausSigmaY = 0;
    Int_t nHits = GetNumberOfHits();
    if (nHits <= 0) {
        gausSigmaY = 0;
    } else {
        Int_t nAdd = 0;
        bool doHitCorrection = true;
        if (doHitCorrection) {
            nAdd = 2;
        }
        Int_t nElems = nHits + nAdd;
        vector<Double_t> x(nElems), y(nElems), ex(nElems), ey(nElems);
        Int_t k = nAdd / 2;
        Double_t xMin = std::numeric_limits<double>::max();
        Double_t xMax = std::numeric_limits<double>::lowest();
        for (int n = 0; n < GetNumberOfHits(); k++, n++) {
            x[k] = fY[n];
            y[k] = fEnergy[n];
            ex[k] = 0;
            xMin = min(xMin, x[k]);
            xMax = max(xMax, x[k]);
            if (y[k] != 0) {
                ey[k] = 10 * sqrt(y[k]);
            } else {
                ey[k] = 0;
            }
        }
        Int_t h = nHits + nAdd / 2;
        if (doHitCorrection) {
            x[0] = xMin - 0.5;
            x[h] = xMax + 0.5;
            y[0] = 0.0;
            y[h] = 0.0;
            ex[0] = 0.0;
            ex[h] = 0.0;
            ey[0] = 70.0;
            ey[h] = 70.0;
        }
        TGraphErrors* grY = new TGraphErrors(nElems, &x[0], &y[0], &ex[0], &ey[0]);
        // TCanvas *c = new TCanvas("c","Y position fit",200,10,500,500);
        // grY->Draw();
        // Defining the starting parameters for the fit.
        Double_t maxY = MaxElement(nElems, grY->GetY());
        Double_t maxX = grY->GetX()[LocMax(nElems, grY->GetY())];
        Double_t sigma = abs(x[0] - x[h]) / 2.0;

        TF1* fit = new TF1("", "gaus");
        fit->SetParameter(0, maxY);
        fit->SetParameter(1, maxX);
        fit->SetParameter(2, sigma);
        grY->Fit(fit, "QNB");  // Q = quiet, no info in screen; N = no plot; B = no automatic start
                               // parameters; R = Use the Range specified in the function range

        gausSigmaY = fit->GetParameter(2);
    }

    return abs(gausSigmaY);
}
/// \brief It computes the gaussian sigma in the Z-coordinate.
/// It adds a hit to the right and a hit to the left, with energy = 0 +/- 70 ADC.
/// Then it fits a gaussian to the hits and extracts the sigma. The hits are just added
/// for fitting purposes and do not go into any further processing.
Double_t TRestHits::GetGaussSigmaZ() {
    Double_t gausSigmaZ = 0;
    Int_t nHits = GetNumberOfHits();
    if (nHits <= 0) {
        gausSigmaZ = 0;
    } else {
        Int_t nAdd = 0;
        bool doHitCorrection = true;
        if (doHitCorrection) {
            nAdd = 2;
        }
        Int_t nElems = nHits + nAdd;
        vector<Double_t> x(nElems), y(nElems), ex(nElems), ey(nElems);
        Int_t k = nAdd / 2;
        Double_t xMin = std::numeric_limits<double>::max();
        Double_t xMax = std::numeric_limits<double>::lowest();
        for (int n = 0; n < GetNumberOfHits(); k++, n++) {
            x[k] = fY[n];
            y[k] = fEnergy[n];
            ex[k] = 0;
            xMin = min(xMin, x[k]);
            xMax = max(xMax, x[k]);
            if (y[k] != 0) {
                ey[k] = 10 * sqrt(y[k]);
            } else {
                ey[k] = 0;
            }
        }
        Int_t h = nHits + nAdd / 2;
        if (doHitCorrection) {
            x[0] = xMin - 0.5;
            x[h] = xMax + 0.5;
            y[0] = 0.0;
            y[h] = 0.0;
            ex[0] = 0.0;
            ex[h] = 0.0;
            ey[0] = 70.0;
            ey[h] = 70.0;
        }
        TGraphErrors* grZ = new TGraphErrors(nElems, &x[0], &y[0], &ex[0], &ey[0]);
        // TCanvas *c = new TCanvas("c","Z position fit",200,10,500,500);
        // grZ->Draw();
        // Defining the starting parameters for the fit.
        Double_t maxY = MaxElement(nElems, grZ->GetY());
        Double_t maxX = grZ->GetX()[LocMax(nElems, grZ->GetY())];
        Double_t sigma = abs(x[0] - x[h]) / 2.0;

        TF1* fit = new TF1("", "gaus");
        fit->SetParameter(0, maxY);
        fit->SetParameter(1, maxX);
        fit->SetParameter(2, sigma);
        grZ->Fit(fit, "QNB");  // Q = quiet, no info in screen; N = no plot; B = no automatic start
                               // parameters; R = Use the Range specified in the function range

        gausSigmaZ = fit->GetParameter(2);
    }

    return abs(gausSigmaZ);
}

///////////////////////////////////////////////
/// \brief It returns the 2-dimensional skewness on the XY-plane which is a measure of the hits
/// distribution asymmetry.
///
Double_t TRestHits::GetSkewXY() const {
    Double_t skewXY = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t sigmaXY = TMath::Sqrt(this->GetSigmaXY2());
    Double_t meanX = this->GetMeanPositionX();
    Double_t meanY = this->GetMeanPositionY();
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0))
            skewXY += fEnergy[n] * (meanY - fY[n]) * (meanY - fY[n]) * (meanY - fY[n]);
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0))
            skewXY += fEnergy[n] * (meanX - fX[n]) * (meanX - fX[n]) * (meanX - fX[n]);
    }
    return skewXY /= (totalEnergy * sigmaXY * sigmaXY * sigmaXY);
}

///////////////////////////////////////////////
/// \brief It returns the hits distribution variance on the Z-axis.
///
Double_t TRestHits::GetSigmaZ2() const {
    Double_t sigmaZ2 = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanZ = this->GetMeanPositionZ();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (!IsNaN(fZ[n])) sigmaZ2 += fEnergy[n] * (meanZ - fZ[n]) * (meanZ - fZ[n]);
    }
    return sigmaZ2 /= totalEnergy;
}

///////////////////////////////////////////////
/// \brief It returns the hits distribution skewness, or asymmetry on the Z-axis.
///
Double_t TRestHits::GetSkewZ() const {
    Double_t skewZ = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t sigmaZ = TMath::Sqrt(this->GetSigmaZ2());
    Double_t meanZ = this->GetMeanPositionZ();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (!IsNaN(fZ[n])) skewZ += fEnergy[n] * (meanZ - fZ[n]) * (meanZ - fZ[n]) * (meanZ - fZ[n]);
    }
    return skewZ /= (totalEnergy * sigmaZ * sigmaZ * sigmaZ);
}

///////////////////////////////////////////////
/// \brief It determines the mean X position of hits contained inside a prisma delimited between `x0`
/// and `x1` vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate
/// the prism along its axis to give full freedom.
///
Double_t TRestHits::GetMeanPositionXInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                            Double_t sizeY, Double_t theta) const {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0) &&
             (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)))) {
            meanX += fX[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanX /= totalEnergy;

    return meanX;
}

///////////////////////////////////////////////
/// \brief It determines the mean Y position of hits contained inside a prisma delimited between `x0`
/// and `x1` vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate
/// the prism along its axis to give full freedom.
///
Double_t TRestHits::GetMeanPositionYInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                            Double_t sizeY, Double_t theta) const {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0) &&
             (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)))) {
            meanY += fY[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanY /= totalEnergy;

    return meanY;
}

///////////////////////////////////////////////
/// \brief It determines the mean Z position of hits contained inside a prisma delimited between `x0`
/// and `x1` vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate
/// the prism along its axis to give full freedom.
///
Double_t TRestHits::GetMeanPositionZInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                            Double_t sizeY, Double_t theta) const {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((!IsNaN(fZ[n]) && (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)))) {
            meanZ += fZ[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanZ /= totalEnergy;

    return meanZ;
}

///////////////////////////////////////////////
/// \brief It determines the mean position of hits contained inside a prisma delimited between `x0`
/// and `x1` vertex, and with face dimensions sizeX by sizeY. The angle theta should serve to rotate
/// the prism along its axis to give full freedom.
///
TVector3 TRestHits::GetMeanPositionInPrism(const TVector3& x0, const TVector3& x1, Double_t sizeX,
                                           Double_t sizeY, Double_t theta) const {
    TVector3 mean(GetMeanPositionXInPrism(x0, x1, sizeX, sizeY, theta),
                  GetMeanPositionYInPrism(x0, x1, sizeX, sizeY, theta),
                  GetMeanPositionZInPrism(x0, x1, sizeX, sizeY, theta));
    return mean;
}

///////////////////////////////////////////////
/// \brief It determines the mean position X using the hits contained inside a cylinder with a
/// given `radius` and delimited between `x0` and `x1` vertex.
///
Double_t TRestHits::GetMeanPositionXInCylinder(const TVector3& x0, const TVector3& x1,
                                               Double_t radius) const {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0) &&
             (isHitNInsideCylinder(n, x0, x1, radius)))) {
            meanX += fX[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanX /= totalEnergy;

    return meanX;
}

///////////////////////////////////////////////
/// \brief It determines the mean position Y using the hits contained inside a cylinder with a
/// given `radius` and delimited between `x0` and `x1` vertex.
///
Double_t TRestHits::GetMeanPositionYInCylinder(const TVector3& x0, const TVector3& x1,
                                               Double_t radius) const {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0) &&
             (isHitNInsideCylinder(n, x0, x1, radius)))) {
            meanY += fY[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanY /= totalEnergy;

    return meanY;
}

///////////////////////////////////////////////
/// \brief It determines the mean position Z using the hits contained inside a cylinder with a
/// given `radius` and delimited between `x0` and `x1` vertex.
///
Double_t TRestHits::GetMeanPositionZInCylinder(const TVector3& x0, const TVector3& x1,
                                               Double_t radius) const {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((!IsNaN(fZ[n]) && (isHitNInsideCylinder(n, x0, x1, radius)))) {
            meanZ += fZ[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanZ /= totalEnergy;

    return meanZ;
}

///////////////////////////////////////////////
/// \brief It determines the mean position using the hits contained inside a cylinder with a
/// given `radius` and delimited between `x0` and `x1` vertex.
///
TVector3 TRestHits::GetMeanPositionInCylinder(const TVector3& x0, const TVector3& x1, Double_t radius) const {
    TVector3 mean(GetMeanPositionXInCylinder(x0, x1, radius), GetMeanPositionYInCylinder(x0, x1, radius),
                  GetMeanPositionZInCylinder(x0, x1, radius));
    return mean;
}

///////////////////////////////////////////////
/// \brief It determines the distance required to travel from hit `n` to hit `m` adding all
/// the distances of the hits that are found between both.
///
Double_t TRestHits::GetHitsPathLength(Int_t n, Int_t m) const {
    if (n < 0) n = 0;
    if (m > GetNumberOfHits() - 1) m = GetNumberOfHits() - 1;

    Double_t distance = 0;
    for (int i = n; i < m; i++) distance += TMath::Sqrt(GetDistance2(i, i + 1));
    return distance;
}

///////////////////////////////////////////////
/// \brief It determines the distance required to travel from the first to the last hit
/// adding all the distances of the hits that are found between both.
///
Double_t TRestHits::GetTotalDistance() const {
    Double_t distance = 0;
    for (int i = 0; i < GetNumberOfHits() - 1; i++) distance += TMath::Sqrt(GetDistance2(i, i + 1));
    return distance;
}

///////////////////////////////////////////////
/// \brief It returns the euclidian distance between hits `n` and `m`.
///
Double_t TRestHits::GetDistance2(int n, int m) const {
    Double_t dx = GetX(n) - GetX(m);
    Double_t dy = GetY(n) - GetY(m);
    Double_t dz = GetZ(n) - GetZ(m);

    if (areXY()) return dx * dx + dy * dy;
    if (areXZ()) return dx * dx + dz * dz;
    if (areYZ()) return dy * dy + dz * dz;

    return dx * dx + dy * dy + dz * dz;
}

///////////////////////////////////////////////
/// \brief It determines the distance required to travel from the first hit to the hit
/// `n` adding all the distances of the hits that are found till the hit `n`.
///
Double_t TRestHits::GetDistanceToNode(Int_t n) const {
    Double_t distance = 0;
    if (n > GetNumberOfHits() - 1) n = GetNumberOfHits() - 1;

    for (int hit = 0; hit < n; hit++) distance += GetVector(hit + 1, hit).Mag();

    return distance;
}

///////////////////////////////////////////////
/// \brief It returns the most energetic hit found between hits `n` and `m`.
///
Int_t TRestHits::GetMostEnergeticHitInRange(Int_t n, Int_t m) const {
    Int_t maxEnergy = 0;
    Int_t hit = -1;
    for (int i = n; i < m; i++) {
        if (GetEnergy(i) > maxEnergy) {
            maxEnergy = GetEnergy(i);
            hit = i;
        }
    }
    return hit;
}

///////////////////////////////////////////////
/// \brief It returns the closest hit to a given `position`.
///
Int_t TRestHits::GetClosestHit(const TVector3& position) const {
    Int_t closestHit = 0;

    Double_t minDistance = 1.e30;
    for (int nHit = 0; nHit < GetNumberOfHits(); nHit++) {
        TVector3 vector = position - GetPosition(nHit);

        Double_t distance = vector.Mag2();
        if (distance < minDistance) {
            closestHit = nHit;
            minDistance = distance;
        }
    }

    return closestHit;
}

///////////////////////////////////////////////
/// \brief It returns the longitudinal and transversal projections of `position` to the
/// axis defined by the hits `n` and `m`.
///
TVector2 TRestHits::GetProjection(Int_t n, Int_t m, const TVector3& position) const {
    TVector3 nodesSegment = this->GetVector(n, m);

    TVector3 origin = position - this->GetPosition(m);

    if (origin == TVector3(0, 0, 0)) return {0, 0};

    Double_t longitudinal = nodesSegment.Unit().Dot(origin);
    if (origin == nodesSegment) return {longitudinal, 0};

    Double_t transversal = TMath::Sqrt(origin.Mag2() - longitudinal * longitudinal);

    return {longitudinal, transversal};
}

///////////////////////////////////////////////
/// \brief It returns the transversal projection of `position` to the line defined by
/// `position` and `direction`.
///
Double_t TRestHits::GetTransversalProjection(const TVector3& p0, const TVector3& direction,
                                             const TVector3& position) const {
    TVector3 oX = position - p0;

    if (oX == TVector3(0, 0, 0)) return 0;

    Double_t longitudinal = direction.Unit().Dot(oX);

    return TMath::Sqrt(oX.Mag2() - longitudinal * longitudinal);
}

//////////////////////////////////////////////
/// \brief A parameter measuring how straight is a given sequence of hits. If the value
/// is close to zero, the hits follow a straight path in average. I believe the value
/// should be then -1 to 1 depending where the track is twisting. Or may be just a
/// positive value giving the measurement of twist. Not 100% sure now.
///
Double_t TRestHits::GetHitsTwist(Int_t n, Int_t m) const {
    if (n < 0) n = 0;
    if (m == 0) m = this->GetNumberOfHits();

    if (m - n < 2) return 0;

    Double_t sum = 0;
    Int_t cont = 0;
    for (int N = n + 1; N < m - 1; N++) {
        TVector3 a = (this->GetPosition(N - 1) - this->GetPosition(N + 1)).Unit();
        TVector3 b = (this->GetPosition(N) - this->GetPosition(N + 1)).Unit();

        sum += (1 - a.Dot(b)) / 2.;
        cont++;
    }

    if (cont == 0) return -1;

    return sum / cont;
}

//////////////////////////////////////////////
/// \brief Same as GetHitsTwist but weighting with the energy
///
Double_t TRestHits::GetHitsTwistWeighted(Int_t n, Int_t m) const {
    if (n < 0) n = 0;
    if (m == 0) m = this->GetNumberOfHits();

    if (m - n < 2) return 0;

    Double_t sum = 0;
    Int_t cont = 0;
    for (int N = n + 1; N < m - 1; N++) {
        TVector3 a = (this->GetPosition(N - 1) - this->GetPosition(N)).Unit();
        TVector3 b = (this->GetPosition(N) - this->GetPosition(N + 1)).Unit();

        Double_t weight = TMath::Abs(this->GetEnergy(N - 1) - this->GetEnergy(N));
        weight += TMath::Abs(this->GetEnergy(N) - this->GetEnergy(N + 1));

        sum += weight * (1 - a.Dot(b)) / 2.;
        cont++;
    }

    if (cont == 0) return -1;

    return sum / cont;
}

//////////////////////////////////////////////
/// \brief It returns the maximum distance between 2-hits.
///
Double_t TRestHits::GetMaximumHitDistance() const {
    Double_t maxDistance = 0;
    for (int n = 0; n < this->GetNumberOfHits(); n++)
        for (int m = n + 1; m < this->GetNumberOfHits(); m++) {
            Double_t d = this->GetDistance(n, m);
            if (d > maxDistance) maxDistance = d;
        }

    return maxDistance;
}

//////////////////////////////////////////////
/// \brief It returns the maximum squared distance between 2-hits.
///
Double_t TRestHits::GetMaximumHitDistance2() const {
    Double_t maxDistance = 0;
    for (int n = 0; n < this->GetNumberOfHits(); n++)
        for (int m = n + 1; m < this->GetNumberOfHits(); m++) {
            Double_t d = this->GetDistance2(n, m);
            if (d > maxDistance) maxDistance = d;
        }

    return maxDistance;
}

//////////////////////////////////////////////
/// \brief It prints on screen the first `nHits` from the list.
///
void TRestHits::PrintHits(Int_t nHits) const {
    Int_t N = nHits;

    if (N == -1) N = GetNumberOfHits();
    if (N > GetNumberOfHits()) N = GetNumberOfHits();

    for (int n = 0; n < N; n++) {
        cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n)
             << " Energy: " << GetEnergy(n) << " T: " << GetTime(n);
        cout << endl;
    }
}

///////////////////////
// Iterator methods

TRestHits::TRestHits_Iterator::TRestHits_Iterator(TRestHits* h, int _index) {
    fHits = h;
    index = _index;
    maxIndex = fHits->GetNumberOfHits();
    if (index < 0) index = 0;
    if (index >= maxIndex) index = maxIndex;
}

void TRestHits::TRestHits_Iterator::toaccessor() {
    _x = x();
    _y = y();
    _z = z();
    _t = t();
    _e = e();
    _type = type();
    isAccessor = true;
}

TRestHits::TRestHits_Iterator TRestHits::TRestHits_Iterator::operator*() const {
    TRestHits_Iterator i(*this);
    i.toaccessor();
    return i;
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator++() {
    index++;
    if (index >= maxIndex) index = maxIndex;
    return *this;
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator+=(const int& n) {
    if (index + n >= maxIndex) {
        index = maxIndex;
    } else {
        index += n;
    }
    return *this;
}

TRestHits::TRestHits_Iterator TRestHits::TRestHits_Iterator::operator+(const int& n) {
    if (index + n >= maxIndex) {
        return TRestHits_Iterator(fHits, maxIndex);
    } else {
        return TRestHits_Iterator(fHits, index + n);
    }
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator--() {
    index--;
    if (index <= 0) index = 0;
    return *this;
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator-=(const int& n) {
    if (index - n <= 0) {
        index = 0;
    } else {
        index -= n;
    }
    return *this;
}

TRestHits::TRestHits_Iterator TRestHits::TRestHits_Iterator::operator-(const int& n) {
    if (index - n <= 0) {
        return TRestHits_Iterator(fHits, 0);
    } else {
        return TRestHits_Iterator(fHits, index - n);
    }
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator=(const TRestHits_Iterator& iter) {
    if (isAccessor) {
        (fHits ? fHits->fX[index] : x()) = iter.x();
        (fHits ? fHits->fY[index] : y()) = iter.y();
        (fHits ? fHits->fZ[index] : z()) = iter.z();
        (fHits ? fHits->fEnergy[index] : e()) = iter.e();
        (fHits ? fHits->fTime[index] : t()) = iter.t();
        (fHits ? fHits->fType[index] : type()) = iter.type();
    } else {
        fHits = iter.fHits;
        index = iter.index;
    }
    return *this;
}
