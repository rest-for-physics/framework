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
    fNodesX = nodes - 1;
    fNodesY = nodes - 1;
    fNodesZ = nodes - 1;

    fNetOrigin = TVector3(0, 0, 0);
}

TRestMesh::TRestMesh(TVector3 size, TVector3 position, Int_t nx, Int_t ny, Int_t nz) {
    fNetSizeX = size.X();
    fNetSizeY = size.Y();
    fNetSizeZ = size.Z();

    // We will divide the grid in intervals not nodes.
    fNodesX = nx - 1;
    fNodesY = ny - 1;
    fNodesZ = nz - 1;

    fNetOrigin = position;
}

//______________________________________________________________________________
TRestMesh::~TRestMesh() {
    // TRestMesh destructor
}

//! Gets the X position of node (i,j)
Double_t TRestMesh::GetX(Int_t nX) { return fNetOrigin.X() + (fNetSizeX / fNodesX) * nX; }

//! Gets the Y position of node (i,j)
Double_t TRestMesh::GetY(Int_t nY) { return fNetOrigin.Y() + (fNetSizeY / fNodesY) * nY; }

//! Gets the Z position of node (i,j)
Double_t TRestMesh::GetZ(Int_t nZ) { return fNetOrigin.Z() + (fNetSizeZ / fNodesZ) * nZ; }

//! Gets the nodeX index corresponding to the x coordinate
Int_t TRestMesh::GetNodeX(Double_t x) {
    if (IsNaN(x)) return 0;

    if (x > fNetSizeX + fNetOrigin.X()) {
        cout << "REST WARNING (TRestMesh) : X node (" << x
             << ") outside boundaries. Setting it to : " << fNodesX - 1 << endl;
        return fNodesX - 1;
    }

    if (x < fNetOrigin.X()) {
        cout << "REST WARNING (TRestMesh) : X node (" << x << ") outside boundaries. Setting it to : " << 0
             << endl;
        return 0;
    }

    Int_t nX = (Int_t)(((x - fNetOrigin.X()) * fNodesX / fNetSizeX));

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

    Int_t nY = (Int_t)(((y - fNetOrigin.Y()) / fNetSizeY) * fNodesY);

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

    Int_t nZ = (Int_t)(((z - fNetOrigin.Z()) / fNetSizeZ) * fNodesZ);

    return nZ;
}

void TRestMesh::SetNodesFromHits(TRestHits* hits) {
    for (int hit = 0; hit < hits->GetNumberOfHits(); hit++) {
        this->AddNode(hits->GetX(hit), hits->GetY(hit), hits->GetZ(hit));
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
    fNodesX = nX - 1;
    fNodesY = nY - 1;
    fNodesZ = nZ - 1;
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

void TRestMesh::Print() {
    std::cout << "Mesh. Number of nodes : " << GetNumberOfNodes()
              << " Number of groups : " << GetNumberOfGroups() << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    for (int i = 0; i < GetNumberOfNodes(); i++)
        std::cout << "Group : " << nodeGroupID[i] << " X : " << nodeX[i] << " Y : " << nodeY[i]
                  << " Z : " << nodeZ[i] << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
}
