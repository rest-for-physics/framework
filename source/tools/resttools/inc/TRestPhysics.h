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

#include <iostream>
using namespace std;

#include "TString.h"
#include "TVector3.h"

/// This namespace serves to define physics constants and other basic physical operations
namespace REST_Physics {

/// Vacuum permitivity in F/m
const double vacuumPermitivity = 8.854E-12;
/// Vacuum permeability in H/m
const double vacuumPermeability = 4E-7 * 3.141592653589793;
/// Speed of light in m/s
const double speedLight = 2.99792458E8;
/// Electron charge in C
const double qElectron = 1.602E-19;
/// Electron mass in Kg
const double mElectron = 9.107E-31;
/// Boltzman  constant in J/K
const double kBoltzman = 1.380E-23;
/// Planck constant in J*s
const double hPlanck = 1.054E-34;

/// A meter in eV
const double PhMeterIneV = 5067731.236453719;  // 8.0655447654281218E5;// 506.773123645372;
/// Speed of light in m/s
const double lightSpeed = 2.99792458E8;
/// Electron charge in natural units
const double naturalElectron = 0.302822120214353;

TVector3 MoveToPlane(TVector3 pos, TVector3 dir, TVector3 n, TVector3 a);
TVector3 MoveByDistance(TVector3 pos, TVector3 dir, Double_t d);
TVector3 GetPlaneVectorIntersection(TVector3 pos, TVector3 dir, TVector3 n, TVector3 a);

Double_t GetDistance(TVector3 v1, TVector3 v2);
Double_t GetDistance2(TVector3 v1, TVector3 v2);
}  // namespace REST_Physics

// dummy class
class TRestPhysics {};
#endif
