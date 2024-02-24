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

#ifndef __RestCore_TRestPhysics_H__
#define __RestCore_TRestPhysics_H__

#include <TMatrixD.h>
#include <TString.h>
#include <TVector3.h>
#include <TVectorD.h>

#include <iostream>

/// This namespace serves to define physics constants and other basic physical operations
namespace REST_Physics {

/// Vacuum permitivity in F/m
constexpr double vacuumPermitivity = 8.854E-12;
/// Vacuum permeability in H/m
constexpr double vacuumPermeability = 4E-7 * 3.141592653589793;
/// Speed of light in m/s
constexpr double lightSpeed = 2.99792458E8;
/// Electron charge in C
constexpr double qElectron = 1.602E-19;
/// Electron mass in Kg
constexpr double mElectron = 9.107E-31;
/// Boltzman  constant in J/K
constexpr double kBoltzman = 1.380E-23;
/// Planck constant in J*s
constexpr double hPlanck = 1.054E-34;

/// A meter in eV
constexpr double PhMeterIneV = 5067731.236453719;
/// A second in eV (using natural units)
constexpr double secondIneV = 1519225802531030.2;
/// Electron charge in natural units
constexpr double naturalElectron = 0.302822120214353;
/// A kelvin in eV
constexpr double kelvinToeV = 86.172809e-6;

/// Average Sun-Earth distance in m
constexpr double AU = 1.49597870691E11;

// Solar radius in m
constexpr double solarRadius = 6.95700E8;

TVector3 MoveToPlane(const TVector3& pos, const TVector3& dir, const TVector3& n, const TVector3& a);

TVector3 MoveByDistance(const TVector3& pos, const TVector3& dir, Double_t d);
TVector3 MoveByDistanceFast(const TVector3& pos, const TVector3& dir, Double_t d);

TVector3 GetVectorReflection(const TVector3& dir, const TVector3& n);

Double_t GetVectorsAngle(const TVector3& v1, const TVector3& v2);

TVector3 GetPlaneVectorIntersection(const TVector3& pos, const TVector3& dir, TVector3 const& n,
                                    TVector3 const& a);

TVector3 GetParabolicVectorIntersection(const TVector3& pos, const TVector3& dir, const Double_t alpha,
                                        const Double_t R3);

TVector3 GetHyperbolicVectorIntersection(const TVector3& pos, const TVector3& dir, const Double_t beta,
                                         const Double_t R3, const Double_t focal);

TVector3 GetConeNormal(const TVector3& pos, const Double_t alpha, const Double_t R = 0);

TVector3 GetParabolicNormal(const TVector3& pos, const Double_t alpha, const Double_t R3);

TVector3 GetHyperbolicNormal(const TVector3& pos, const Double_t beta, const Double_t R3,
                             const Double_t focal);

TMatrixD GetConeMatrix(const TVector3& d, const Double_t cosTheta);

Double_t GetConeVectorIntersection(const TVector3& pos, const TVector3& dir, const TVector3& d,
                                   const TVector3& v, const Double_t cosTheta);
Double_t GetConeVectorIntersection(const TVector3& pos, const TVector3& dir, const TMatrixD& M,
                                   const TVector3& axis, const TVector3& v);

Double_t DistanceToAxis(const TVector3& axisPoint, const TVector3& axisVector, const TVector3& point);

Double_t GetDistance(const TVector3& v1, const TVector3& v2);
Double_t GetDistance2(const TVector3& v1, const TVector3& v2);
}  // namespace REST_Physics

#endif
