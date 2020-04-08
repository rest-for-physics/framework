/******************** REST disclaimer ***********************************
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

#include "TRestPhysics.h"
#include <iostream>
#include <limits>
using namespace std;

//////////////////////////////////////////////////////////////////////////
///
/// This class is a collection of physics constants and basic methods that
/// can be accessed using the REST_Physics:: namespace.
///
/// We include basic geometrical operations that transform space coordinates.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2019-Mar:   First concept and implementation of REST_Physics namespace.
/// \author     Javier Galan <javier.galan@unizar.es>
///
/// \namespace REST_Physics
///
/// <hr>
namespace REST_Physics {

///////////////////////////////////////////////
/// \brief This method will translate the vector with direction `dir` starting at position `pos` to the plane
/// defined by the normal vector plane, `n` that contains the point `a` in the plane.
///
TVector3 MoveToPlane(TVector3 pos, TVector3 dir, TVector3 n, TVector3 a) {
    if (n * dir == 0) {
        cout << "MoveToPlane. The vector is parallel to the plane!!" << endl;
        cout << "Position will not be translated" << endl;
    } else {
        Double_t t = (n * a - n * pos) / (n * dir);

        return pos + t * dir;
    }
    return pos;
}

///////////////////////////////////////////////
/// \brief This method will find the intersection of the trajectory defined by the vector starting at `pos`
/// and  moving in direction `dir` and the plane defined by its normal vector `n` and the point `a`. This is
/// equivalent to move/translate the position `pos` to the plane.
///
TVector3 GetPlaneVectorIntersection(TVector3 pos, TVector3 dir, TVector3 n, TVector3 a) {
    return MoveToPlane(pos, dir, n, a);
}

///////////////////////////////////////////////
/// \brief This method transports a position `pos` by a distance `d` in the direction defined by `dir`.
///
TVector3 MoveByDistance(TVector3 pos, TVector3 dir, Double_t d) { return pos + d * dir.Unit(); }
}
