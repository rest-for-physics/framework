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

#ifndef RestCore_TRestMesh
#define RestCore_TRestMesh

#include <TObject.h>
#include <TVector3.h>

#include <iostream>

#include "TRestHits.h"

const int NODE_NOT_SET = -1;
const int GROUP_NOT_FOUND = -1;
const int NODE_NOT_FOUND = -1;

/// A basic class inhereting from TObject to help creating a node grid definition
class TRestMesh : public TObject {
   protected:
    /// The bottom-left corner of the bounding-box grid.
    TVector3 fNetOrigin = TVector3(0, 0, 0);

    /// The size of the grid in X dimension
    Double_t fNetSizeX = 0;
    /// The size of the grid in Y dimension
    Double_t fNetSizeY = 0;
    /// The size of the grid in Z dimension
    Double_t fNetSizeZ = 0;

    /// Number of nodes in X-dimension (R in spherical coordinates)
    Int_t fNodesX = 0;
    /// Number of nodes in Y-dimension (Theta in spherical coordinates)
    Int_t fNodesY = 0;
    /// Number of nodes in Z-dimension (Phi in spherical coordinates)
    Int_t fNodesZ = 0;

    /// Total number of nodes filled
    Int_t fNumberOfNodes = 0;
    /// Total number of groups found
    Int_t fNumberOfGroups = 0;

    /// A vector storing the group ID of the corresponding nodes activated
    std::vector<Int_t> fNodeGroupID;
    /// A vector storing the X-dimension cell id
    std::vector<Int_t> fNodeX;
    /// A vector storing the Y-dimension cell id
    std::vector<Int_t> fNodeY;
    /// A vector storing the Z-dimension cell id
    std::vector<Int_t> fNodeZ;

    /// A vector storing the total energy inside the cell id
    std::vector<Double_t> fEnergy;

    /// A flag to indentify if we use cylindrical coordinates
    Bool_t fIsCylindrical = false;
    /// A flag to indentify if we use spherical coordinates
    Bool_t fIsSpherical = false;

   public:
    Double_t GetX(Int_t nX);
    Double_t GetY(Int_t nY);
    Double_t GetZ(Int_t nZ);

    /// Returns the total number of nodes added
    Int_t GetNumberOfNodes() { return fNumberOfNodes; }
    /// Returns the total number of groups identified
    Int_t GetNumberOfGroups() { return fNumberOfGroups; }

    TVector3 GetPosition(Int_t nX, Int_t nY, Int_t nZ);

    Int_t GetNodeX(Double_t x, Bool_t relative = false);
    Int_t GetNodeY(Double_t y, Bool_t relative = false);
    Int_t GetNodeZ(Double_t z, Bool_t relative = false);

    Int_t GetNodeX(TVector3 v, Bool_t relative = false);
    Int_t GetNodeY(TVector3 v, Bool_t relative = false);
    Int_t GetNodeZ(TVector3 v, Bool_t relative = false);

    /// Returns true if the coordinate system is set to cylindrical
    Bool_t IsCylindrical() { return fIsCylindrical; }
    /// Returns true if the coordinate system is set to spherical
    Bool_t IsSpherical() { return fIsSpherical; }

    /// Returns a node by its position in the vector
    TVector3 GetNodeByIndex(Int_t index) {
        TVector3 node(fNodeX[index], fNodeY[index], fNodeZ[index]);
        return node;
    }

    /// Returns the energy at a particular node
    Double_t GetEnergyAtNode(Int_t nx, Int_t ny, Int_t nz) { return fEnergy[GetNodeIndex(nx, ny, nz)]; }

    void SetNodesFromHits(TRestHits* hits);
    void SetNodesFromSphericalHits(TRestHits* hits);
    void Regrouping();

    Int_t GetNodeIndex(Int_t nx, Int_t ny, Int_t nz);

    Int_t GetGroupId(Double_t x, Double_t y, Double_t z);
    Int_t GetGroupId(Int_t index);

    Double_t GetGroupEnergy(Int_t index);
    TVector3 GetGroupPosition(Int_t index);

    Int_t FindNeighbourGroup(Int_t nx, Int_t ny, Int_t nz);
    Int_t FindForeignNeighbour(Int_t nx, Int_t ny, Int_t nz);

    void SetOrigin(Double_t oX, Double_t oY, Double_t oZ);
    void SetOrigin(TVector3 pos);

    void SetSize(Double_t sX, Double_t sY, Double_t sZ);

    void SetNodes(Int_t nX, Int_t nY, Int_t nZ);

    /// Sets the coordinate system to cylindrical
    void SetCylindrical(Bool_t v) { fIsCylindrical = v; }

    /// Sets the coordinate system to spherical
    void SetSpherical(Bool_t v) {
        if (v && (fNetSizeX != fNetSizeY || fNetSizeY != fNetSizeZ))
            std::cout << "Warning!! The net size should be the same in X, Y and Z for spherical net!"
                      << std::endl;

        fIsSpherical = v;
    }

    /// Returns the number of nodes defined in the X-dimension
    Int_t GetNodesX() { return fNodesX; }
    /// Returns the number of nodes defined in the Y-dimension
    Int_t GetNodesY() { return fNodesY; }
    /// Returns the number of nodes defined in the Z-dimension
    Int_t GetNodesZ() { return fNodesZ; }

    /// Returns the net size on X-dimension
    Double_t GetNetSizeX() { return fNetSizeX; }
    /// Returns the net size on Y-dimension
    Double_t GetNetSizeY() { return fNetSizeY; }
    /// Returns the net size on Z-dimension
    Double_t GetNetSizeZ() { return fNetSizeZ; }

    /// Returns the origin of the grid (bottom-left corner)
    TVector3 GetOrigin() { return fNetOrigin; }
    /// Returns a vector with the size/dimensions of the bounding box
    TVector3 GetNetSize() { return TVector3(fNetSizeX, fNetSizeY, fNetSizeZ); }
    TVector3 GetNetCenter();
    TVector3 GetVertex(Int_t id);

    /// It returns the bottom boundary vertex
    TVector3 GetBottomVertex() { return GetVertex(0); }
    /// It returns the top boundary vertex
    TVector3 GetTopVertex() { return GetVertex(1); }

    void AddNode(Double_t x, Double_t y, Double_t z, Double_t en = 0);
    void AddSphericalNode(Double_t x, Double_t y, Double_t z, Double_t en = 0);

    void RemoveNodes();

    Bool_t IsInside(TVector3 pos);
    Bool_t IsInsideBoundingBox(TVector3 pos);

    std::vector<TVector3> GetTrackBoundaries(TVector3 pos, TVector3 dir, Bool_t particle = true);
    std::vector<TVector3> GetTrackBoundariesCylinder(TVector3 pos, TVector3 dir, Bool_t particle = true);

    void Print();

    // Constructor
    TRestMesh();

    TRestMesh(Double_t size, Int_t nodes);
    TRestMesh(TVector3 size, TVector3 position, Int_t nx, Int_t ny, Int_t nz);
    // Destructor
    ~TRestMesh();

    ClassDef(TRestMesh, 2);
};
#endif
