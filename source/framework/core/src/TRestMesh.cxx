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
/// TRestMesh is a helper class allowing to define a uniform grid that can be
/// filled directly with a TRestHits structure.
///
/// The class keeps track only of those nodes (cells in the grid) where a hit
/// was found. TRestMesh also includes grouping algorithms allowing to define
/// a node group id. Assigning a unique group id to those cells that are
/// neighbours.
///
/// This class was originally created to be used at TRestAxionMagneticField
/// and TRestDetectorHitsToTrackFastProcess.
///
/// If spherical coordinates are enabled, the X,Y,Z dimensions will relate to
/// R, Theta, and Phi. The interface with the class still keeps using
/// X,Y,Z coordinates, but the nodes (nX,nY,nZ) will make reference to the
/// position in a grid divided at uniform steps in R, Theta and Phi, where
/// `GetX( nX, nY, nZ)`, `GetY( nX, nY, nZ) and `GetZ( nX, nY, nZ)` will
/// return the cartesian coordinates, but nX, nY and nZ will make reference to
/// the cell at a given radius, and theta and phi angles.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-February: First concept and implementation of TRestMesh.
/// \author     Javier Galan
///
/// 2021-Abril: Including spherical coordinates capability
/// \author     Javier Galan
///
/// \class TRestMesh
///
/// <hr>
///

#include "TRestMesh.h"

#include "TRestPhysics.h"

using namespace std;
using namespace TMath;

ClassImp(TRestMesh);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestMesh::TRestMesh() {}

///////////////////////////////////////////////
/// \brief Constructor specifying the size (sX=sY=sZ=size) and the number of nodes (nX=nY=nZ=nodes).
///
TRestMesh::TRestMesh(Double_t size, Int_t nodes) {
    fNetSizeX = size;
    fNetSizeY = size;
    fNetSizeZ = size;

    // We will divide the grid in intervals not nodes.
    fNodesX = nodes;
    fNodesY = nodes;
    fNodesZ = nodes;

    fNetOrigin = TVector3(0, 0, 0);
}

///////////////////////////////////////////////
/// \brief Constructor specifying the size, origin, and number of nodes in each dimension.
///
TRestMesh::TRestMesh(TVector3 size, TVector3 position, Int_t nx, Int_t ny, Int_t nz) {
    fNetSizeX = size.X();
    fNetSizeY = size.Y();
    fNetSizeZ = size.Z();

    // We will divide the grid in intervals not nodes.
    fNodesX = nx;
    fNodesY = ny;
    fNodesZ = nz;

    fNetOrigin = position;
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestMesh::~TRestMesh() {
    // TRestMesh destructor
}

///////////////////////////////////////////////
/// \brief Gets the cartesian position of nodeX
///
Double_t TRestMesh::GetX(Int_t nX) { return GetPosition(nX, 0, 0).X(); }

///////////////////////////////////////////////
/// \brief Gets the cartesian position of nodeY
///
Double_t TRestMesh::GetY(Int_t nY) { return GetPosition(0, nY, 0).Y(); }

///////////////////////////////////////////////
/// \brief Gets the cartesian position of nodeZ
///
Double_t TRestMesh::GetZ(Int_t nZ) { return GetPosition(0, 0, nZ).Z(); }

///////////////////////////////////////////////
/// \brief Gets the position of the corresponding node.
///
TVector3 TRestMesh::GetPosition(Int_t nX, Int_t nY, Int_t nZ) {
    if (fIsSpherical) {
        Double_t r = (fNetSizeX / (fNodesX - 1)) * nX;
        Double_t theta = (TMath::Pi() / (fNodesY - 1)) * nY;
        Double_t phi = (2 * TMath::Pi() / (fNodesY - 1)) * nZ - TMath::Pi();

        TVector3 v(1, 0, 0);
        v.SetMag(r);
        v.SetTheta(theta);
        v.SetPhi(phi);

        return v;

    } else {
        Double_t x = fNetOrigin.X() + (fNetSizeX / (fNodesX - 1)) * nX;
        Double_t y = fNetOrigin.Y() + (fNetSizeY / (fNodesY - 1)) * nY;
        Double_t z = fNetOrigin.Z() + (fNetSizeZ / (fNodesZ - 1)) * nZ;
        return TVector3(x, y, z);
    }

    return TVector3(0, 0, 0);
}

///////////////////////////////////////////////
/// \brief Gets the node index corresponding to the x-coordinate.
///
/// If relative it is true it means the x-coordinate is already relative to the origin of the mesh.
/// By default the x-coordinate is given in absolute coordinates, i.e. relative=false.
///
Int_t TRestMesh::GetNodeX(Double_t x, Bool_t relative) {
    if (IsNaN(x)) return 0;

    Double_t xInside = x - fNetOrigin.X();
    if (relative) xInside = x;

    if (xInside > fNetSizeX) {
        cout << "REST WARNING (TRestMesh) : X node (" << x
             << ") outside boundaries. Setting it to : " << fNodesX - 1 << endl;
        return fNodesX - 1;
    }

    if (xInside < 0) {
        cout << "REST WARNING (TRestMesh) : X node (" << x << ") outside boundaries. Setting it to : " << 0
             << endl;
        return 0;
    }

    return (Int_t)(xInside * (fNodesX - 1) / fNetSizeX);
}

///////////////////////////////////////////////
/// \brief Gets the node index corresponding to the x-coordinate or r-coordinate in spherical coordinates.
///
/// If relative it is true it means the x-coordinate is already relative to the origin of the mesh.
/// By default the x-coordinate is given in absolute coordinates, i.e. relative=false.
///
Int_t TRestMesh::GetNodeX(TVector3 v, Bool_t relative) {
    // If it is cylindrical we still follow the cartesian approach :(
    if (fIsCylindrical || !fIsSpherical) return GetNodeX(v.X(), relative);

    // if( fIsSpherical ) {

    TVector3 posInside = v - fNetOrigin;
    if (relative) posInside = v;

    if (posInside.Mag() > fNetSizeX) {
        cout << "REST WARNING (TRestMesh) : Relative position (" << posInside.X() << ", " << posInside.Y()
             << ", " << posInside.Z() << ")"
             << ") outside boundaries. Setting it to : " << fNodesX - 1 << endl;
        return fNodesX - 1;
    }

    // }

    return (Int_t)(posInside.Mag() * (fNodesX - 1) / fNetSizeX);
}

///////////////////////////////////////////////
/// \brief Gets the node index corresponding to the y-coordinate.
///
/// If relative it is true it means the y-coordinate is already relative to the origin of the mesh.
/// By default the y-coordinate is given in absolute coordinates, i.e. relative=false.
///
Int_t TRestMesh::GetNodeY(Double_t y, Bool_t relative) {
    if (IsNaN(y)) return 0;

    Double_t yInside = y - fNetOrigin.Y();
    if (relative) yInside = y;

    if (yInside > fNetSizeY) {
        cout << "REST WARNING (TRestMesh) : Y node (" << y
             << ") outside boundaries. Setting it to : " << fNodesY - 1 << endl;
        return fNodesY - 1;
    }

    if (yInside < 0) {
        cout << "REST WARNING (TRestMesh) : Y node (" << y << ") outside boundaries. Setting it to : " << 0
             << endl;
        return 0;
    }

    Int_t nY = (Int_t)(yInside * (fNodesY - 1) / fNetSizeY);

    return nY;
}

///////////////////////////////////////////////
/// \brief Gets the node index corresponding to the x-coordinate or theta-coordinate in spherical coordinates.
///
/// If relative it is true it means the x-coordinate is already relative to the origin of the mesh.
/// By default the x-coordinate is given in absolute coordinates, i.e. relative=false.
///
Int_t TRestMesh::GetNodeY(TVector3 v, Bool_t relative) {
    // If it is cylindrical we still follow the cartesian approach :(
    if (fIsCylindrical || !fIsSpherical) return GetNodeY(v.Y(), relative);

    // if( fIsSpherical ) {

    TVector3 posInside = v - fNetOrigin;
    if (relative) posInside = v;

    if (posInside.Mag() > fNetSizeX) {  // fNetSizeX = fNetSizeY = fNetSizeZ
        cout << "REST WARNING (TRestMesh) : Relative position (" << posInside.X() << ", " << posInside.Y()
             << ", " << posInside.Z() << ") outside boundaries. Setting it to : " << fNodesY - 1 << endl;
        return fNodesY - 1;
    }
    // }

    return (Int_t)(posInside.Theta() * (fNodesY - 1) / TMath::Pi());
}

///////////////////////////////////////////////
/// \brief Gets the node index corresponding to the z-coordinate.
///
/// If relative it is true it means the z-coordinate is already relative to the origin of the mesh.
/// By default the z-coordinate must be given in absolute coordinates, i.e. relative=false.
///
Int_t TRestMesh::GetNodeZ(Double_t z, Bool_t relative) {
    if (IsNaN(z)) return 0;

    Double_t zInside = z - fNetOrigin.Z();
    if (relative) zInside = z;
    if (zInside > fNetSizeZ) {
        // cout << "REST WARNING (TRestMesh) : Z node (" << z
        //     << ") outside boundaries. Setting it to : " << fNodesZ - 1 << endl;
        return fNodesZ - 1;
    }

    if (zInside < 0) {
        // cout << "REST WARNING (TRestMesh) : Z node (" << z << ") outside boundaries. Setting it to : " << 0
        //     << endl;
        return 0;
    }

    Int_t nZ = (Int_t)(zInside * (fNodesZ - 1) / fNetSizeZ);

    return nZ;
}

///////////////////////////////////////////////
/// \brief Gets the node index corresponding to the x-coordinate or theta-coordinate in spherical coordinates.
///
/// If relative it is true it means the x-coordinate is already relative to the origin of the mesh.
/// By default the x-coordinate is given in absolute coordinates, i.e. relative=false.
///
Int_t TRestMesh::GetNodeZ(TVector3 v, Bool_t relative) {
    // If it is cylindrical we still follow the cartesian approach :(
    if (fIsCylindrical || !fIsSpherical) return GetNodeY(v.Y(), relative);

    // if( fIsSpherical ) {

    TVector3 posInside = v - fNetOrigin;
    if (relative) posInside = v;

    if (posInside.Mag() > fNetSizeX) {  // fNetSizeX = fNetSizeY = fNetSizeZ
        cout << "REST WARNING (TRestMesh) : Relative position (" << posInside.X() << ", " << posInside.Y()
             << ", " << posInside.Z() << ") outside boundaries. Setting it to : " << fNodesZ - 1 << endl;
        return fNodesZ - 1;
    }
    // }

    return (Int_t)((posInside.Phi() + TMath::Pi()) * (fNodesZ - 1) / 2. / TMath::Pi());
}

///////////////////////////////////////////////
/// \brief It initializes the nodes using the hit coordinates found inside a TRestHits structure
///
void TRestMesh::SetNodesFromHits(TRestHits* hits) {
    double nan = numeric_limits<double>::quiet_NaN();
    for (int hit = 0; hit < hits->GetNumberOfHits(); hit++) {
        if (hits->GetEnergy(hit) <= 0) continue;
        REST_HitType type = hits->GetType(hit);
        this->AddNode(type == YZ ? nan : hits->GetX(hit), type == XZ ? nan : hits->GetY(hit), hits->GetZ(hit),
                      hits->GetEnergy(hit));
    }

    Regrouping();
}

///////////////////////////////////////////////
/// \brief It initializes the nodes using the hit coordinates found inside a TRestHits structure.
/// This method will assume that the hit coordinates are expressed in spherical coordinates.
/// (R, Theta, Phi).
///
void TRestMesh::SetNodesFromSphericalHits(TRestHits* hits) {
    if (!fIsSpherical)
        cout << "TRestMesh::SetNodesFromSphericalHits is only to be used with a spherical mesh!" << endl;

    for (int hit = 0; hit < hits->GetNumberOfHits(); hit++) {
        if (hits->GetEnergy(hit) <= 0) continue;
        this->AddSphericalNode(hits->GetX(hit), hits->GetY(hit), hits->GetZ(hit), hits->GetEnergy(hit));
    }

    Regrouping();
}

///////////////////////////////////////////////
/// \brief Needs TO BE documented
///
void TRestMesh::Regrouping() {
    for (int g = 0; g < GetNumberOfGroups(); g++) {
        Int_t nbIndex = 0;
        Bool_t change = true;
        while (change) {
            change = false;
            for (int n = 0; n < GetNumberOfNodes(); n++) {
                if (GetGroupId(n) != g) continue;

                TVector3 nde = GetNodeByIndex(n);
                nbIndex = FindForeignNeighbour(nde.X(), nde.Y(), nde.Z());

                if (nbIndex != GROUP_NOT_FOUND) {
                    fNodeGroupID[nbIndex] = g;
                    change = true;
                }
            }
        }
    }

    vector<Int_t> groups;
    for (int n = 0; n < GetNumberOfNodes(); n++) {
        Bool_t groupFound = false;
        for (unsigned int i = 0; i < groups.size(); i++)
            if (GetGroupId(n) == groups[i]) groupFound = true;

        if (!groupFound) groups.push_back(GetGroupId(n));
    }

    fNumberOfGroups = groups.size();

    for (int i = 0; i < fNumberOfGroups; i++) {
        if (i != groups[i]) {
            for (int n = 0; n < GetNumberOfNodes(); n++)
                if (GetGroupId(n) == groups[i]) fNodeGroupID[n] = i;
        }
    }
}

///////////////////////////////////////////////
/// \brief Returns the vector position for a given node index.
/// If the node is not found, -1 will be returned.
///
Int_t TRestMesh::GetNodeIndex(Int_t nx, Int_t ny, Int_t nz) {
    for (int i = 0; i < GetNumberOfNodes(); i++)
        if (fNodeX[i] == nx && fNodeY[i] == ny && fNodeZ[i] == nz) return i;
    return -1;
}

///////////////////////////////////////////////
/// \brief Returns the group id corresponding to the x,y,z coordinate.
/// If the coordinate falls at a non-initialized node, it will return
/// GROUP_NOT_FOUND.
///
Int_t TRestMesh::GetGroupId(Double_t x, Double_t y, Double_t z) {
    Int_t nx, ny, nz;
    if (fIsSpherical) {
        TVector3 v(x, y, z);  // Because if one of them is nan, this might cause problems
        nx = GetNodeX(TVector3(x, y, z));
        ny = GetNodeY(TVector3(x, y, z));
        nz = GetNodeZ(TVector3(x, y, z));
    } else {
        nx = GetNodeX(x);
        ny = GetNodeY(y);
        nz = GetNodeZ(z);
    }

    Int_t index = GetNodeIndex(nx, ny, nz);
    if (index != NODE_NOT_SET) return fNodeGroupID[index];

    return GROUP_NOT_FOUND;
}

///////////////////////////////////////////////
/// \brief Returns the group id using the position inside the nodes vector.
///
Int_t TRestMesh::GetGroupId(Int_t index) {
    if (index > (int)fNodeGroupID.size()) return GROUP_NOT_FOUND;

    return fNodeGroupID[index];
}

///////////////////////////////////////////////
/// \brief It returns the total energy of all nodes corresponding to the group id given by argument
///
Double_t TRestMesh::GetGroupEnergy(Int_t index) {
    if (index > (int)fNodeGroupID.size()) return 0.0;

    Double_t sum = 0;
    for (int n = 0; n < GetNumberOfNodes(); n++)
        if (fNodeGroupID[n] == index) sum += fEnergy[n];

    return sum;
}

///////////////////////////////////////////////
/// \brief It returns the average position for all nodes weighted with their corresponding energy
///
TVector3 TRestMesh::GetGroupPosition(Int_t index) {
    double nan = numeric_limits<double>::quiet_NaN();

    if (index > (int)fNodeGroupID.size()) return TVector3(nan, nan, nan);

    TVector3 v(0, 0, 0);
    for (int n = 0; n < GetNumberOfNodes(); n++)
        if (fNodeGroupID[n] == index) v += fEnergy[n] * GetPosition(fNodeX[n], fNodeY[n], fNodeZ[n]);

    v *= 1. / GetGroupEnergy(index);

    return v;
}

///////////////////////////////////////////////
/// \brief Returns the group id of the first node identified in the
/// neighbour cell from cell=(nx,ny,nz).
///
Int_t TRestMesh::FindNeighbourGroup(Int_t nx, Int_t ny, Int_t nz) {
    Int_t index = NODE_NOT_SET;

    for (int i = nx - 1; i <= nx + 1; i++)
        for (int j = ny - 1; j <= ny + 1; j++)
            for (int k = nz - 1; k <= nz + 1; k++) {
                if (i != nx || j != ny || k != nz) {
                    index = GetNodeIndex(i, j, k);
                    if (index != NODE_NOT_SET) return fNodeGroupID[index];
                }
            }
    return GROUP_NOT_FOUND;
}

///////////////////////////////////////////////
/// \brief It identifies a foreign neighbour. I.e. if the group id of the neighbour cell
/// is different to the cell=(nx,ny,nz) it will return the neighbour index with different
/// group id.
///
Int_t TRestMesh::FindForeignNeighbour(Int_t nx, Int_t ny, Int_t nz) {
    Int_t index = GetNodeIndex(nx, ny, nz);

    if (index == NODE_NOT_SET) return GROUP_NOT_FOUND;

    Int_t nodeGroup = fNodeGroupID[index];

    for (int i = nx - 1; i <= nx + 1; i++)
        for (int j = ny - 1; j <= ny + 1; j++)
            for (int k = nz - 1; k <= nz + 1; k++) {
                if (i != nx || j != ny || k != nz) {
                    index = GetNodeIndex(i, j, k);
                    if (index != NODE_NOT_SET && fNodeGroupID[index] != nodeGroup) return index;
                }
            }

    return GROUP_NOT_FOUND;
}

///////////////////////////////////////////////
/// \brief Sets the origin of the bounding-box and initializes the nodes vector to zero.
///
void TRestMesh::SetOrigin(Double_t oX, Double_t oY, Double_t oZ) {
    fNetOrigin = TVector3(oX, oY, oZ);
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

///////////////////////////////////////////////
/// \brief Sets the origin of the bounding-box and initializes the nodes vector to zero.
///
void TRestMesh::SetOrigin(TVector3 pos) {
    fNetOrigin = pos;
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

///////////////////////////////////////////////
/// \brief Sets the origin of the bounding-box and initializes the nodes vector to zero.
///
void TRestMesh::SetSize(Double_t sX, Double_t sY, Double_t sZ) {
    fNetSizeX = sX;
    fNetSizeY = sY;
    fNetSizeZ = sZ;
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

///////////////////////////////////////////////
/// \brief Sets the number of nodes and initializes the nodes vector to zero.
///
void TRestMesh::SetNodes(Int_t nX, Int_t nY, Int_t nZ) {
    fNodesX = nX;
    fNodesY = nY;
    fNodesZ = nZ;
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

///////////////////////////////////////////////
/// \brief If adds corresponding node to xyz-coordinates if not previously defined
///
void TRestMesh::AddNode(Double_t x, Double_t y, Double_t z, Double_t en) {
    Int_t nx, ny, nz;

    if (fIsSpherical) {
        TVector3 v(x, y, z);  // Because if one of them is nan, this might cause problems
        nx = GetNodeX(v);
        ny = GetNodeY(v);
        nz = GetNodeZ(v);
    } else {
        nx = GetNodeX(x);
        ny = GetNodeY(y);
        nz = GetNodeZ(z);
    }

    Int_t index = GetNodeIndex(nx, ny, nz);
    if (index == NODE_NOT_SET) {
        Int_t gId = FindNeighbourGroup(nx, ny, nz);
        if (gId == GROUP_NOT_FOUND) {
            gId = GetNumberOfGroups();
            fNumberOfGroups++;
        }

        fNodeX.push_back(nx);
        fNodeY.push_back(ny);
        fNodeZ.push_back(nz);
        fNodeGroupID.push_back(gId);
        fEnergy.push_back(en);

        fNumberOfNodes++;
    } else {
        fEnergy[index] += en;
    }
}

///////////////////////////////////////////////
/// \brief If adds corresponding node to xyz-coordinates if not previously defined
///
void TRestMesh::AddSphericalNode(Double_t r, Double_t theta, Double_t phi, Double_t en) {
    TVector3 v(1, 0, 0);
    v.SetMag(r);
    v.SetTheta(theta);
    v.SetPhi(phi);

    Int_t nx = GetNodeX(v);
    Int_t ny = GetNodeY(v);
    Int_t nz = GetNodeZ(v);

    Int_t index = GetNodeIndex(nx, ny, nz);
    if (index == NODE_NOT_SET) {
        Int_t gId = FindNeighbourGroup(nx, ny, nz);
        if (gId == GROUP_NOT_FOUND) {
            gId = GetNumberOfGroups();
            fNumberOfGroups++;
        }

        fNodeX.push_back(nx);
        fNodeY.push_back(ny);
        fNodeZ.push_back(nz);
        fNodeGroupID.push_back(gId);
        fEnergy.push_back(en);

        fNumberOfNodes++;
    } else {
        fEnergy[index] += en;
    }
}

///////////////////////////////////////////////
/// \brief It initializes all node vectors to zero
///
void TRestMesh::RemoveNodes() {
    fNodeGroupID.clear();
    fNodeX.clear();
    fNodeY.clear();
    fNodeZ.clear();
    fNumberOfNodes = 0;
    fNumberOfGroups = 0;
}

///////////////////////////////////////////////
/// \brief It returns true if the position is found inside the grid (box,sphere or cylinder).
///
Bool_t TRestMesh::IsInside(TVector3 pos) {
    if (pos.Z() < fNetOrigin.Z() || pos.Z() > fNetOrigin.Z() + fNetSizeZ) return false;

    if (IsSpherical()) {
        // By definition we use X coordinate to define sphere radius
        Double_t R = fNetSizeX / 2.;
        Double_t posRadius = (GetNetCenter() - pos).Mag();

        if (posRadius < R)
            return true;
        else
            return false;
    }

    if (IsCylindrical()) {
        // By definition we use X coordinate to define cylinder radius
        Double_t R2 = fNetSizeX * fNetSizeX / 4.;
        TVector3 relPos = GetNetCenter() - pos;
        Double_t radius2 = relPos.X() * relPos.X() + relPos.Y() * relPos.Y();

        if (radius2 > R2) return false;
    } else {
        if (pos.X() < fNetOrigin.X() || pos.X() > fNetOrigin.X() + fNetSizeX) return false;
        if (pos.Y() < fNetOrigin.Y() || pos.Y() > fNetOrigin.Y() + fNetSizeY) return false;
    }

    return true;
}

///////////////////////////////////////////////
/// \brief It returns true if the position is found inside the bounding box
///
Bool_t TRestMesh::IsInsideBoundingBox(TVector3 pos) {
    if (pos.Z() < fNetOrigin.Z() || pos.Z() > fNetOrigin.Z() + fNetSizeZ) return false;
    if (pos.X() < fNetOrigin.X() || pos.X() > fNetOrigin.X() + fNetSizeX) return false;
    if (pos.Y() < fNetOrigin.Y() || pos.Y() > fNetOrigin.Y() + fNetSizeY) return false;

    return true;
}

//////////////////////////////////////////////
/// \brief It returns the position of the mesh center.
///
TVector3 TRestMesh::GetNetCenter() {
    return (TVector3)(fNetOrigin + TVector3(fNetSizeX / 2., fNetSizeY / 2., fNetSizeZ / 2.));
}

///////////////////////////////////////////////
/// \brief It returns the position of both boundary vertex, bottom vertex identified with id = 0 and top
/// vertex identified with id = 1.
///
TVector3 TRestMesh::GetVertex(Int_t id) {
    if (id == 0) return fNetOrigin;
    if (id == 1) return fNetOrigin + TVector3(fNetSizeX, fNetSizeY, fNetSizeZ);
    return TVector3(0, 0, 0);
}

///////////////////////////////////////////////
/// \brief Finds the intersection of the straight line (track defined by the input arguments given,
/// a position `pos` and direction `dir`) with the boundary box defined by the mesh.
///
/// It is done by searching for the points where the particle trajectory intersects the boundary planes of
/// that region.
///
/// If the optional parameter `particle` is true (which is the default behaviour) this method will
/// return the found boundaries only if the particle is moving towards the volume. On top of that it
/// will be assured that the first boundary element corresponds with the boundary closer to the position
/// of the particle.
///
/// If the `particle` parameter is false, valid boundaries will be returned as soon as the track crosses
/// the volume without being crosschecked the relative positions of the boundaries respect to the particle
/// position.
///
std::vector<TVector3> TRestMesh::GetTrackBoundaries(TVector3 pos, TVector3 dir, Bool_t particle) {
    if (IsCylindrical()) return GetTrackBoundariesCylinder(pos, dir, particle);

    TVector3 netCenter = this->GetNetCenter();

    Double_t xH = netCenter.X() + GetNetSizeX() / 2.;
    Double_t xL = netCenter.X() - GetNetSizeX() / 2.;

    Double_t yH = netCenter.Y() + GetNetSizeY() / 2.;
    Double_t yL = netCenter.Y() - GetNetSizeY() / 2.;

    Double_t zH = netCenter.Z() + GetNetSizeZ() / 2.;
    Double_t zL = netCenter.Z() - GetNetSizeZ() / 2.;

    TVector3 posAtPlane;
    std::vector<TVector3> boundaries;
    boundaries.clear();

    // We move the vector position at each of the planes defined by the bounding box.
    // Then we check if it is within the face limits.

    TVector3 planePosition_BottomZ = TVector3(0, 0, -GetNetSizeZ() / 2.) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 0, 1), planePosition_BottomZ);
    if (pos != posAtPlane && posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Y() > yL &&
        posAtPlane.Y() < yH) {
        boundaries.push_back(posAtPlane);
    }

    TVector3 planePosition_TopZ = TVector3(0, 0, GetNetSizeZ() / 2.) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 0, 1), planePosition_TopZ);
    if (pos != posAtPlane && posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Y() > yL &&
        posAtPlane.Y() < yH) {
        boundaries.push_back(posAtPlane);
    }

    TVector3 planePosition_BottomY = TVector3(0, -GetNetSizeY() / 2., 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 1, 0), planePosition_BottomY);
    if (pos != posAtPlane && posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Z() > zL &&
        posAtPlane.Z() < zH) {
        boundaries.push_back(posAtPlane);
    }

    TVector3 planePosition_TopY = TVector3(0, GetNetSizeY() / 2., 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 1, 0), planePosition_TopY);
    if (pos != posAtPlane && posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Z() > zL &&
        posAtPlane.Z() < zH) {
        boundaries.push_back(posAtPlane);
    }

    TVector3 planePosition_BottomX = TVector3(-GetNetSizeX() / 2., 0, 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(1, 0, 0), planePosition_BottomX);
    if (pos != posAtPlane && posAtPlane.Y() > yL && posAtPlane.Y() < yH && posAtPlane.Z() > zL &&
        posAtPlane.Z() < zH) {
        boundaries.push_back(posAtPlane);
    }

    TVector3 planePosition_TopX = TVector3(GetNetSizeX() / 2., 0, 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(1, 0, 0), planePosition_TopX);
    if (pos != posAtPlane && posAtPlane.Y() > yL && posAtPlane.Y() < yH && posAtPlane.Z() > zL &&
        posAtPlane.Z() < zH) {
        boundaries.push_back(posAtPlane);
    }

    if (boundaries.size() == 2) {
        TVector3 center = 0.5 * (boundaries[0] + boundaries[1]);
        Double_t product_1 = (boundaries[0] - center) * dir;
        Double_t product_2 = (boundaries[1] - center) * dir;

        if (particle) {
            // d1 and d2 is the signed distance to the volume boundaries
            Double_t d1 = (boundaries[0] - pos) * dir;
            Double_t d2 = (boundaries[1] - pos) * dir;

            // Both should be positive so that the particle is approaching the volume
            if (d1 < 0 || d2 < 0) boundaries.clear();

            // The first boundary will be always related to the closer IN boundary
            // If it is no the case we exchange them.
            if (d1 > d2) iter_swap(boundaries.begin(), boundaries.begin() + 1);
        }
    }

    return boundaries;
}

///////////////////////////////////////////////
/// \brief Needs TO BE documented
///
std::vector<TVector3> TRestMesh::GetTrackBoundariesCylinder(TVector3 pos, TVector3 dir, Bool_t particle) {
    TVector3 netCenter = this->GetNetCenter();

    std::vector<TVector3> boundaries;
    boundaries.clear();

    // By definition we take the first component as the radius of the cylinder
    Double_t R2 = fNetSizeX * fNetSizeX / 4.;

    TVector3 pos2D = TVector3(pos.X() - netCenter.X(), pos.Y() - netCenter.Y(), 0);
    TVector3 dir2D = TVector3(dir.X(), dir.Y(), 0);

    Double_t product = pos2D * dir2D;
    Double_t product2 = product * product;

    Double_t dirMag2 = dir2D.Mag2();
    Double_t posMag2 = pos2D.Mag2();
    Double_t root = product2 - dirMag2 * (posMag2 - R2);

    // For simplicity we ignore tangencial tracks. Those that produce 1-solution.
    // If root < 0 there is no real solution to the intersection
    if (root > 0) {
        Double_t t1 = (-product - TMath::Sqrt(root)) / dirMag2;
        Double_t t2 = (-product + TMath::Sqrt(root)) / dirMag2;

        TVector3 firstVertex = pos + t1 * dir;
        TVector3 secondVertex = pos + t2 * dir;

        if (firstVertex.Z() >= GetVertex(0).Z() && firstVertex.Z() <= GetVertex(1).Z())
            boundaries.push_back(firstVertex);
        if (secondVertex.Z() >= GetVertex(0).Z() && secondVertex.Z() <= GetVertex(1).Z())
            boundaries.push_back(secondVertex);

        if (boundaries.size() == 2) {
            if (particle) {
                if (t1 > 0 && t2 > t1) {
                    return boundaries;
                } else {
                    boundaries.clear();
                }
            }
            return boundaries;
        }
    }

    TVector3 posAtPlane;

    // We check if the track is entering through one of the cylinder covers.
    TVector3 planePosition_BottomZ = TVector3(0, 0, -GetNetSizeZ() / 2.) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 0, 1), planePosition_BottomZ);
    TVector3 relPosBottom = posAtPlane - netCenter;
    relPosBottom.SetZ(0);
    // fNetSizeX is used to define the radius of the cylinder
    if (relPosBottom.Mag2() < fNetSizeX * fNetSizeX / 4.) boundaries.push_back(posAtPlane);

    TVector3 planePosition_TopZ = TVector3(0, 0, GetNetSizeZ() / 2.) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 0, 1), planePosition_TopZ);
    TVector3 relPosTop = posAtPlane - netCenter;
    relPosTop.SetZ(0);
    // fNetSizeX is used to define the radius of the cylinder
    if (relPosTop.Mag2() < fNetSizeX * fNetSizeX / 4.) boundaries.push_back(posAtPlane);

    if (boundaries.size() == 2 && particle) {
        // d1 and d2 is the signed distance to the volume boundaries
        Double_t d1 = (boundaries[0] - pos) * dir;
        Double_t d2 = (boundaries[1] - pos) * dir;

        // Both should be positive so that the particle is approaching the volume
        if (d1 < 0 || d2 < 0) boundaries.clear();

        // The first boundary will be always related to the closer IN boundary
        // If it is no the case we exchange them.
        if (d1 > d2) iter_swap(boundaries.begin(), boundaries.begin() + 1);
    }

    return boundaries;
}

///////////////////////////////////////////////
/// \brief Prints the nodes information
///
void TRestMesh::Print() {
    std::cout << "Mesh. Number of nodes : " << GetNumberOfNodes()
              << " Number of groups : " << GetNumberOfGroups() << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    for (int i = 0; i < GetNumberOfNodes(); i++)
        std::cout << "Group : " << fNodeGroupID[i] << " X : " << fNodeX[i] << " Y : " << fNodeY[i]
                  << " Z : " << fNodeZ[i] << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
}
