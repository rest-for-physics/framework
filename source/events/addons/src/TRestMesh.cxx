///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMesh.cxx
///
///             Event class to help for using mesh nodes
///
///             feb 2016:
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestMesh.h"
#include "TRestPhysics.h"
using namespace std;
using namespace TMath;

ClassImp(TRestMesh);
//______________________________________________________________________________
TRestMesh::TRestMesh() {
    // TRestMesh default constructor
}

//______________________________________________________________________________
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

//______________________________________________________________________________
TRestMesh::~TRestMesh() {
    // TRestMesh destructor
}

//! Gets the X position of node (i,j)
Double_t TRestMesh::GetX(Int_t nX) { return fNetOrigin.X() + (fNetSizeX / (fNodesX - 1)) * nX; }

//! Gets the Y position of node (i,j)
Double_t TRestMesh::GetY(Int_t nY) { return fNetOrigin.Y() + (fNetSizeY / (fNodesY - 1)) * nY; }

//! Gets the Z position of node (i,j)
Double_t TRestMesh::GetZ(Int_t nZ) { return fNetOrigin.Z() + (fNetSizeZ / (fNodesZ - 1)) * nZ; }

//! Gets the nodeX index corresponding to the x coordinate
Int_t TRestMesh::GetNodeX(Double_t x) {
    if (IsNaN(x)) return 0;

    if (x > fNetSizeX + fNetOrigin.X()) {
        cout << "REST WARNING (TRestMesh) : X node (" << x
             << ") outside boundaries. Setting it to : " << fNodesX << endl;
        return fNodesX - 1;
    }

    if (x < fNetOrigin.X()) {
        cout << "REST WARNING (TRestMesh) : X node (" << x << ") outside boundaries. Setting it to : " << 0
             << endl;
        return 0;
    }

    Int_t nX = (Int_t)((x - fNetOrigin.X()) * (fNodesX - 1) / fNetSizeX);

    return nX;
}

//! Gets the nodeY index corresponding to the x coordinate
Int_t TRestMesh::GetNodeY(Double_t y) {
    if (IsNaN(y)) return 0;

    if (y > fNetSizeY + fNetOrigin.Y()) {
        cout << "REST WARNING (TRestMesh) : Y node (" << y
             << ") outside boundaries. Setting it to : " << fNodesY - 1 << endl;
        return fNodesY - 1;
    }

    if (y < fNetOrigin.Y()) {
        cout << "REST WARNING (TRestMesh) : Y node (" << y << ") outside boundaries. Setting it to : " << 0
             << endl;
        return 0;
    }

    Int_t nY = (Int_t)((y - fNetOrigin.Y()) * (fNodesY - 1) / fNetSizeY);

    return nY;
}

//! Gets the nodeY index corresponding to the x coordinate
Int_t TRestMesh::GetNodeZ(Double_t z) {
    if (IsNaN(z)) return 0;

    if (z > fNetSizeZ + fNetOrigin.Z()) {
        cout << "REST WARNING (TRestMesh) : Z node (" << z
             << ") outside boundaries. Setting it to : " << fNodesZ - 1 << endl;
        return fNodesZ - 1;
    }

    if (z < fNetOrigin.Z()) {
        cout << "REST WARNING (TRestMesh) : Z node (" << z << ") outside boundaries. Setting it to : " << 0
             << endl;
        return 0;
    }

    Int_t nZ = (Int_t)((z - fNetOrigin.Z()) * (fNodesZ - 1) / fNetSizeZ);

    return nZ;
}

void TRestMesh::SetNodesFromHits(TRestHits* hits) {
    double nan = numeric_limits<double>::quiet_NaN();
    for (int hit = 0; hit < hits->GetNumberOfHits(); hit++) {
        REST_HitType type = hits->GetType(hit);
        this->AddNode(type == YZ ? nan : hits->GetX(hit), type == XZ ? nan : hits->GetY(hit),
                      hits->GetZ(hit));
    }

    Regrouping();
}

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
                    nodeGroupID[nbIndex] = g;
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
                if (GetGroupId(n) == groups[i]) nodeGroupID[n] = i;
        }
    }
}

Int_t TRestMesh::GetNodeIndex(Int_t nx, Int_t ny, Int_t nz) {
    for (int i = 0; i < GetNumberOfNodes(); i++)
        if (nodeX[i] == nx && nodeY[i] == ny && nodeZ[i] == nz) return i;
    return -1;
}

Int_t TRestMesh::GetGroupId(Double_t x, Double_t y, Double_t z) {
    Int_t nx = GetNodeX(x);
    Int_t ny = GetNodeY(y);
    Int_t nz = GetNodeZ(z);

    Int_t index = GetNodeIndex(nx, ny, nz);
    if (index != NODE_NOT_SET) return nodeGroupID[index];

    return GROUP_NOT_FOUND;
}

Int_t TRestMesh::GetGroupId(Int_t index) {
    if (index > (int)nodeGroupID.size()) return GROUP_NOT_FOUND;

    return nodeGroupID[index];
}

Int_t TRestMesh::FindNeighbourGroup(Int_t nx, Int_t ny, Int_t nz) {
    Int_t index = NODE_NOT_SET;

    for (int i = nx - 1; i <= nx + 1; i++)
        for (int j = ny - 1; j <= ny + 1; j++)
            for (int k = nz - 1; k <= nz + 1; k++) {
                if (i != nx || j != ny || k != nz) {
                    index = GetNodeIndex(i, j, k);
                    if (index != NODE_NOT_SET) return nodeGroupID[index];
                }
            }
    return GROUP_NOT_FOUND;
}

Int_t TRestMesh::FindForeignNeighbour(Int_t nx, Int_t ny, Int_t nz) {
    Int_t index = GetNodeIndex(nx, ny, nz);

    if (index == NODE_NOT_SET) return GROUP_NOT_FOUND;

    Int_t nodeGroup = nodeGroupID[index];

    for (int i = nx - 1; i <= nx + 1; i++)
        for (int j = ny - 1; j <= ny + 1; j++)
            for (int k = nz - 1; k <= nz + 1; k++) {
                if (i != nx || j != ny || k != nz) {
                    index = GetNodeIndex(i, j, k);
                    if (index != NODE_NOT_SET && nodeGroupID[index] != nodeGroup) return index;
                }
            }

    return GROUP_NOT_FOUND;
}

// Setters
void TRestMesh::SetOrigin(Double_t oX, Double_t oY, Double_t oZ) {
    fNetOrigin = TVector3(oX, oY, oZ);
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

void TRestMesh::SetOrigin(TVector3 pos) {
    fNetOrigin = pos;
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

void TRestMesh::SetSize(Double_t sX, Double_t sY, Double_t sZ) {
    fNetSizeX = sX;
    fNetSizeY = sY;
    fNetSizeZ = sZ;
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

void TRestMesh::SetNodes(Int_t nX, Int_t nY, Int_t nZ) {
    fNodesX = nX;
    fNodesY = nY;
    fNodesZ = nZ;
    // TODO instead of removing nodes we might need just to re-translate the
    // existing nodes
    RemoveNodes();
}

void TRestMesh::AddNode(Double_t x, Double_t y, Double_t z) {
    Int_t nx = GetNodeX(x);
    Int_t ny = GetNodeY(y);
    Int_t nz = GetNodeZ(z);

    /*
    cout << "Adding node : x=" << x  << " y=" << y << " z=" << z << endl;
    cout << "Node : " << nx << " " << ny << " " << nz << endl;
    cout << "Index : " << GetNodeIndex( nx, ny, nz ) << endl;
    */

    if (GetNodeIndex(nx, ny, nz) == NODE_NOT_SET) {
        Int_t gId = FindNeighbourGroup(nx, ny, nz);
        if (gId == GROUP_NOT_FOUND) {
            gId = GetNumberOfGroups();
            fNumberOfGroups++;
        }
        /*
        cout << "Adding it!" << endl;
        getchar();
        */

        nodeX.push_back(nx);
        nodeY.push_back(ny);
        nodeZ.push_back(nz);
        nodeGroupID.push_back(gId);

        fNumberOfNodes++;
    }
}

void TRestMesh::RemoveNodes() {
    nodeGroupID.clear();
    nodeX.clear();
    nodeY.clear();
    nodeZ.clear();
    fNumberOfNodes = 0;
    fNumberOfGroups = 0;
}

Bool_t TRestMesh::IsInside(TVector3 pos) {
    if (pos.X() < fNetOrigin.X() || pos.X() > fNetOrigin.X() + fNetSizeX) return false;
    if (pos.Y() < fNetOrigin.Y() || pos.Y() > fNetOrigin.Y() + fNetSizeY) return false;
    if (pos.Z() < fNetOrigin.Z() || pos.Z() > fNetOrigin.Z() + fNetSizeZ) return false;

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
    if (posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Y() > yL && posAtPlane.Y() < yH)
        boundaries.push_back(posAtPlane);

    TVector3 planePosition_TopZ = TVector3(0, 0, GetNetSizeZ() / 2.) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 0, 1), planePosition_TopZ);
    if (posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Y() > yL && posAtPlane.Y() < yH)
        boundaries.push_back(posAtPlane);

    TVector3 planePosition_BottomY = TVector3(0, -GetNetSizeY() / 2., 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 1, 0), planePosition_BottomY);
    if (posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Z() > zL && posAtPlane.Z() < zH)
        boundaries.push_back(posAtPlane);

    TVector3 planePosition_TopY = TVector3(0, GetNetSizeY() / 2., 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(0, 1, 0), planePosition_TopY);
    if (posAtPlane.X() > xL && posAtPlane.X() < xH && posAtPlane.Z() > zL && posAtPlane.Z() < zH)
        boundaries.push_back(posAtPlane);

    TVector3 planePosition_BottomX = TVector3(-GetNetSizeX() / 2., 0, 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(1, 0, 0), planePosition_BottomX);
    if (posAtPlane.Y() > yL && posAtPlane.Y() < yH && posAtPlane.Z() > zL && posAtPlane.Z() < zH)
        boundaries.push_back(posAtPlane);

    TVector3 planePosition_TopX = TVector3(GetNetSizeX() / 2., 0, 0) + netCenter;
    posAtPlane = REST_Physics::MoveToPlane(pos, dir, TVector3(1, 0, 0), planePosition_TopX);
    if (posAtPlane.Y() > yL && posAtPlane.Y() < yH && posAtPlane.Z() > zL && posAtPlane.Z() < zH)
        boundaries.push_back(posAtPlane);

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

void TRestMesh::Print() {
    std::cout << "Mesh. Number of nodes : " << GetNumberOfNodes()
              << " Number of groups : " << GetNumberOfGroups() << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    for (int i = 0; i < GetNumberOfNodes(); i++)
        std::cout << "Group : " << nodeGroupID[i] << " X : " << nodeX[i] << " Y : " << nodeY[i]
                  << " Z : " << nodeZ[i] << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
}
