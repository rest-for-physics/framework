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
/// The methods and physics constants in this class can be accessed using the
/// REST_Physics:: namespace.
///
/// We include basic geometrical operations that transform space coordinates
/// or implement vector transportation.
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
/// If the vector is parallel to the plane the position `pos` will not be translated.
///
TVector3 MoveToPlane(const TVector3& pos, const TVector3& dir, const TVector3& n, const TVector3& a) {
    if (n * dir != 0) {
        Double_t t = (n * a - n * pos) / (n * dir);

        return pos + t * dir;
    }
    return pos;
}

///////////////////////////////////////////////
/// \brief This method will return the distance from `point` to the straight defined by `axisPoint` and
/// `axisVector`.
///
Double_t DistanceToAxis(const TVector3& axisPoint, const TVector3& axisVector, const TVector3& point) {
    TVector3 a = axisVector.Cross(axisPoint - point);
    return a.Mag() / axisVector.Mag();
}

///////////////////////////////////////////////
/// \brief This method will find the intersection of the trajectory defined by the vector starting at `pos`
/// and  moving in direction `dir` and the plane defined by its normal vector `n` and the point `a`. This is
/// equivalent to move/translate the position `pos` to the plane.
///
TVector3 GetPlaneVectorIntersection(const TVector3& pos, const TVector3& dir, const TVector3& n,
                                    const TVector3& a) {
    return MoveToPlane(pos, dir, n, a);
}

//////////////////////////////////////////////
/// This method will find the intersection between a vector and a parabolic shape where `alpha` is the angle
/// between the optical axis and the paraboloid at the plane where the paraboloid has a radius of `R3`.
/// The paraboloid is rotationally symmetric around the optical axis. `alpha` in rad.
/// The region in which the intersection can happen here is between `-lMirr` and 0 on the z (optical) axis
///
/// In case no intersection is found this method returns the unmodified input position
///
TVector3 GetParabolicVectorIntersection(const TVector3& pos, const TVector3& dir, const Double_t alpha,
                                        const Double_t R3, const Double_t lMirr) {
    Double_t e = 2 * R3 * TMath::Tan(alpha);
    Double_t a = dir.X() * dir.X() + dir.Y() * dir.Y();
    Double_t b = 2 * (pos.X() * dir.X() + pos.Y() * dir.Y()) + e * dir.Z();
    Double_t half_b = b / 2;
    Double_t c = pos.X() * pos.X() + pos.Y() * pos.Y() - R3 * R3 + e * pos.Z();
    if (a != 0) {
        Double_t root1 = (-half_b - TMath::Sqrt(half_b * half_b - a * c)) / a;
        Double_t root2 = (-half_b + TMath::Sqrt(half_b * half_b - a * c)) / a;
        if (pos.Z() + root1 * dir.Z() > -lMirr and pos.Z() + root1 * dir.Z() < 0) {
            return pos + root1 * dir;
        } else if (pos.Z() + root2 * dir.Z() > -lMirr and pos.Z() + root2 * dir.Z() < 0) {
            return pos + root2 * dir;
        }
        return pos;
    }
    return pos - c / b * dir;
}

//////////////////////////////////////////////
/// This method will find the intersection between a vector and a hyperbolic shape where 3 * `alpha` is the
/// angle between the optical axis and the hyperboloid at the plane where the hyperboloid has a radius of
/// `R3`. The hyperboloid is rotationally symmetric around the optical axis. `alpha` in rad. The region in
/// which the intersection can happen here is between 0 and `lMirr` on the `z` (optical) axis
///
/// In case no intersection is found this method returns the unmodified input position
///
TVector3 GetHyperbolicVectorIntersection(const TVector3& pos, const TVector3& dir, const Double_t alpha,
                                         const Double_t R3, const Double_t lMirr, const Double_t focal) {
    Double_t beta = 3 * alpha;
    Double_t e = 2 * R3 * TMath::Tan(beta);
    /// Just replaced here *TMath::Cot by /TMath::Tan to fix compilation issues
    Double_t g = 2 * R3 * TMath::Tan(beta) / (focal + R3 / TMath::Tan(2 * alpha));
    Double_t a = dir.X() * dir.X() + dir.Y() * dir.Y() - g * dir.Z() * dir.Z();
    Double_t b = 2 * (pos.X() * dir.X() + pos.Y() * dir.Y() - g * dir.Z() * pos.Z()) + e * dir.Z();
    Double_t half_b = b / 2;
    Double_t c = pos.X() * pos.X() + pos.Y() * pos.Y() - R3 * R3 + e * pos.Z() - g * pos.Z() * pos.Z();
    Double_t root1 = (-half_b - TMath::Sqrt(half_b * half_b - a * c)) / a;
    Double_t root2 = (-half_b + TMath::Sqrt(half_b * half_b - a * c)) / a;
    if (pos.Z() + root1 * dir.Z() > 0 and pos.Z() + root1 * dir.Z() < lMirr) {
        return pos + root1 * dir;
    } else if (pos.Z() + root2 * dir.Z() > 0 and pos.Z() + root2 * dir.Z() < lMirr) {
        return pos + root2 * dir;
    }

    return pos;
}

///////////////////////////////////////////////
/// \brief It returns the cone matrix M = d^T x d - cosTheta^2 x I, extracted from the document
/// by "David Eberly, Geometric Tools, Redmond WA 98052, Intersection of a Line and a Cone".
///
TMatrixD GetConeMatrix(const TVector3& d, const Double_t cosTheta) {
    double cAxis[3];
    d.GetXYZ(cAxis);

    TVectorD coneAxis(3, cAxis);

    TMatrixD M(3, 3);
    M.Rank1Update(coneAxis, coneAxis);

    double cT2 = cosTheta * cosTheta;
    TMatrixD gamma(3, 3);
    gamma.UnitMatrix();
    gamma *= cT2;

    M -= gamma;
    return M;
}

///////////////////////////////////////////////
/// \brief This method will return the reflected vector respect to a plane defined by its
/// normal vector `n`. The normal vector should be already normalized!
///
TVector3 GetVectorReflection(const TVector3& dir, const TVector3& n) { return dir - 2 * dir.Dot(n) * n; }

///////////////////////////////////////////////
/// \brief This method will return the angle in radians between 2 vectors.
///
Double_t GetVectorsAngle(const TVector3& v1, const TVector3& v2) { return TMath::ACos(v1.Dot(v2)); }

///////////////////////////////////////////////
/// \brief This method will return a vector that is normal to the cone surface. The position `pos`
/// should be at the cone surface, and the angle `alpha` should be the angle that defines the
/// cone construction.
///
/// It is assumed that the vertex is found at the right of `pos`.
///
/// Optionally, the radius of the cone at the given `pos` can be provided to facilitate the
/// calculation
///
TVector3 GetConeNormal(const TVector3& pos, const Double_t alpha, const Double_t R) {
    Double_t r = 0;
    if (R == 0)
        r = TMath::Sqrt(pos.X() * pos.X() + pos.Y() * pos.Y());
    else
        r = R;

    Double_t cosA = TMath::Cos(alpha);
    Double_t sinA = TMath::Sin(alpha);

    return -TVector3(cosA * pos.X() / r, cosA * pos.Y() / r, sinA);
}

///////////////////////////////////////////////
/// \brief This method returns the normal vector on a parabolic surface pointing towards the inside
/// of the paraboloid. `pos` is the origin point of the normal vector on the parabolic plane and
/// `alpha` is the angle between the paraboloid and the optical (z) axis at the plane where the
/// paraboloid has the radius `R3`.
///
TVector3 GetParabolicNormal(const TVector3& pos, const Double_t alpha, const Double_t R3) {
    TVector3 normalVec = pos;
    Double_t m =
        1 / (R3 * TMath::Tan(alpha) / TMath::Sqrt(R3 * R3 + R3 * 2 * TMath::Tan(alpha) * (-pos.Z())));
    Double_t n = TMath::Sqrt(pos.X() * pos.X() + pos.Y() * pos.Y()) - m * pos.Z();
    normalVec.SetZ(pos.Z() - (-n / m));
    return normalVec.Unit();
}

///////////////////////////////////////////////
/// \brief This method returns the normal vector on a hyperbolic surface pointing towards the inside
/// of the hyperboloid. `pos` is the origin point of the normal vector on the hyperbolic plane and
/// `beta` is the angle between the hyperboloid and the optical (z) axis at the plane where the
/// hyperboloid has the radius `R3`.
///
TVector3 GetHyperbolicNormal(const TVector3& pos, const Double_t alpha, const Double_t R3,
                             const Double_t focal) {
    TVector3 normalVec = pos;
    Double_t beta = 3 * alpha;
    /// Just replaced here *TMath::Cot by /TMath::Tan to fix compilation issues
    Double_t m = 1 / (R3 * TMath::Tan(beta) * (1 - 2 * pos.Z() / (focal + R3 / TMath::Tan(2 * alpha))) /
                      TMath::Sqrt(R3 * R3 - R3 * 2 * TMath::Tan(beta) * pos.Z() *
                                                (1.0 - pos.Z() / (focal + R3 / TMath::Tan(2 * alpha)))));
    Double_t n = TMath::Sqrt(pos.X() * pos.X() + pos.Y() * pos.Y()) - m * pos.Z();
    normalVec.SetZ(pos.Z() - (-n / m));
    return normalVec.Unit();
}

///////////////////////////////////////////////
/// \brief This method will find the intersection of the trajectory defined by the vector starting at
/// `pos` and  moving in direction `dir` and the cone defined by its axis vector `d` and the vertex`v`.
/// The cosine of the angle defining the cone should be also given inside the `cosTheta` argument.
///
/// This method will return `t`, which is the value the particle position, `pos`, needs to be displaced
/// by the vector, `dir`, to get the particle at the surface of the cone. If the particle does not
/// cross the cone, then the value returned will be zero (no particle displacement).
//
/// This method is based on the document by "David Eberly, Geometric Tools, Redmond WA 98052,
/// Intersection of a Line and a Cone".
///
Double_t GetConeVectorIntersection(const TVector3& pos, const TVector3& dir, const TVector3& d,
                                   const TVector3& v, const Double_t cosTheta) {
    TMatrixD M = GetConeMatrix(d, cosTheta);
    return GetConeVectorIntersection(pos, dir, M, d, v);
}

///////////////////////////////////////////////
/// \brief This method will find the intersection of the trajectory defined by the vector starting at `pos`
/// and moving in direction `dir` and the cone defined by its characteristic matrix `M`, which is built
/// using the cone axis vector `d` as `d^T x d`,  and the vertex`v`. The resulting TVector3 will be the
/// position of the particle placed at the cone surface.
///
/// This method will return `t`, which is the value the particle position, `pos`, needs to be displaced
/// by the vector, `dir`, to get the particle at the surface of the cone. If the particle does not
/// cross the cone, then the value returned will be zero (no particle displacement).
///
/// This method is based on the document by "David Eberly, Geometric Tools, Redmond WA 98052,
/// Intersection of a Line and a Cone".
///
Double_t GetConeVectorIntersection(const TVector3& pos, const TVector3& dir, const TMatrixD& M,
                                   const TVector3& axis, const TVector3& v) {
    double u[3];
    dir.GetXYZ(u);
    TMatrixD U(3, 1, u);
    TMatrixD Ut(1, 3, u);

    double delta[3];
    TVector3 deltaV = pos - v;
    deltaV.GetXYZ(delta);
    TMatrixD D(3, 1, delta);
    TMatrixD Dt(1, 3, delta);

    TMatrixD C2 = Ut * M * U;
    Double_t c2 = C2[0][0];

    TMatrixD C1 = Ut * M * D;
    Double_t c1 = C1[0][0];

    TMatrixD C0 = Dt * M * D;
    Double_t c0 = C0[0][0];

    Double_t root = c1 * c1 - c0 * c2;
    if (root < 0) return 0;

    Double_t t1 = (-c1 + TMath::Sqrt(root)) / c2;
    Double_t t2 = (-c1 - TMath::Sqrt(root)) / c2;

    // The projections along the cone axis. If positive then the solution
    // gives the cone intersection with the side defined by `axis`
    Double_t h1 = t1 * dir.Dot(axis) + axis.Dot(deltaV);
    Double_t h2 = t2 * dir.Dot(axis) + axis.Dot(deltaV);

    // We use it to select the root we are interested in
    if (h2 > 0)
        return t2;
    else
        return t1;
}

///////////////////////////////////////////////
/// \brief This method transports a position `pos` by a distance `d` in the direction defined by `dir`.
///
TVector3 MoveByDistance(const TVector3& pos, const TVector3& dir, Double_t d) { return pos + d * dir.Unit(); }

///////////////////////////////////////////////
/// \brief This method transports a position `pos` by a distance `d` in the direction defined by `dir`.
/// This method assumes the vector `dir` is unitary!
///
TVector3 MoveByDistanceFast(const TVector3& pos, const TVector3& dir, Double_t d) { return pos + d * dir; }

///////////////////////////////////////////////
/// \brief This method returns the cartesian distance between vector v2 and v1.
///
Double_t GetDistance(const TVector3& v1, const TVector3& v2) { return (v2 - v1).Mag(); }

///////////////////////////////////////////////
/// \brief This method returns the squared cartesian distance between vector v2 and v1.
///
Double_t GetDistance2(const TVector3& v1, const TVector3& v2) { return (v2 - v1).Mag2(); }
}  // namespace REST_Physics
