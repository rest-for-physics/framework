///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMesh.h
///
///             Event class to help for using mesh nodes
///
///             feb 2016:
///                 Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestMesh
#define RestCore_TRestMesh

#include <iostream>

#include <TObject.h>
#include <TVector3.h>
#include "TRestHits.h"

const int NODE_NOT_SET = -1;
const int GROUP_NOT_FOUND = -1;
const int NODE_NOT_FOUND = -1;

class TRestMesh : public TObject {
   protected:
    TVector3 fNetOrigin;

    Double_t fNetSizeX;
    Double_t fNetSizeY;
    Double_t fNetSizeZ;

    Int_t fNodesX;
    Int_t fNodesY;
    Int_t fNodesZ;

    Int_t fNumberOfNodes;
    Int_t fNumberOfGroups;

    std::vector<Int_t> nodeGroupID;
    std::vector<Int_t> nodeX;
    std::vector<Int_t> nodeY;
    std::vector<Int_t> nodeZ;

   public:
    // Getters
    Int_t GetNumberOfNodes() { return fNumberOfNodes; }
    Int_t GetNumberOfGroups() { return fNumberOfGroups; }

    Double_t GetX(Int_t nX);
    Double_t GetY(Int_t nY);
    Double_t GetZ(Int_t nZ);

    Int_t GetNodeX(Double_t x);
    Int_t GetNodeY(Double_t y);
    Int_t GetNodeZ(Double_t z);

    TVector3 GetNodeByIndex(Int_t index) {
        TVector3 node(nodeX[index], nodeY[index], nodeZ[index]);
        return node;
    }

    void SetNodesFromHits(TRestHits* hits);
    void Regrouping();

    Int_t GetNodeIndex(Int_t nx, Int_t ny, Int_t nz);

    Int_t GetGroupId(Double_t x, Double_t y, Double_t z);
    Int_t GetGroupId(Int_t index);
    //       Int_t GetGroupIdByIndex( Int_t index );

    Int_t FindNeighbourGroup(Int_t nx, Int_t ny, Int_t nz);
    Int_t FindForeignNeighbour(Int_t nx, Int_t ny, Int_t nz);

    // Setters
    void SetOrigin(Double_t oX, Double_t oY, Double_t oZ);
    void SetOrigin(TVector3 pos);

    void SetSize(Double_t sX, Double_t sY, Double_t sZ);

    void SetNodes(Int_t nX, Int_t nY, Int_t nZ);

    Int_t GetNodesX() { return fNodesX + 1; }
    Int_t GetNodesY() { return fNodesY + 1; }
    Int_t GetNodesZ() { return fNodesZ + 1; }

    void AddNode(Double_t x, Double_t y, Double_t z);

    void RemoveNodes();

    void Print();

    // Construtor
    TRestMesh();

    TRestMesh(Double_t size, Int_t nodes);
    TRestMesh(TVector3 size, TVector3 position, Int_t nx, Int_t ny, Int_t nz);
    // Destructor
    ~TRestMesh();

    ClassDef(TRestMesh, 1);
};
#endif
