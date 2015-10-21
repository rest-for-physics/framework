///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeometry.cxx
///
///             Base geometry class used to store a geometry written by GDML or define a new geometry using TGeoManager class
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestGeometry.h"

ClassImp(TRestGeometry)
//______________________________________________________________________________
    TRestGeometry::TRestGeometry() : TGeoManager()
{
   // TRestGeometry default constructor
}

//______________________________________________________________________________
TRestGeometry::~TRestGeometry()
{
   // TRestGeometry destructor
}

void TRestGeometry::PrintGeometry()
{
    cout << "--------------------------------------" << endl;
    cout << "Volumes : " << this->GetListOfVolumes()->GetEntries() << endl;
    cout << "Mother volume : " << this->GetTopVolume()->GetName() << endl;
    cout << "--------------------------------------" << endl;

    TGeoVolume *vol;
    TGeoMaterial *mat;
    for( int volID = 0; volID < this->GetListOfVolumes()->GetEntries(); volID++ )
    {
        vol = this->GetVolume(volID);
        mat = vol->GetMaterial();
        cout << "Volume " << volID << endl;
        cout << "       Name : " << vol->GetName() << endl;
        cout << "       Material : " << mat->GetName() << endl;
        cout << "       Density : " << mat->GetDensity() << endl;
    }
}

