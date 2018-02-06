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
using namespace std;

ClassImp(TRestGeometry)

//______________________________________________________________________________

TRestGeometry::TRestGeometry() : TGeoManager()//, fGfGeometry(0), fDriftElec(0) 
{
#if defined USE_Garfield
   // TRestGeometry default constructor
   vReadoutElec.clear();
   vGfComponent.clear();
   vGfSensor.clear();
#endif
}


//______________________________________________________________________________

TRestGeometry::~TRestGeometry()
{
#if defined USE_Garfield
   // TRestGeometry destructor
   vReadoutElec.clear();
   vGfSensor.clear();
   vGfComponent.clear();
#endif
}


//------------------------------------------------------------------------------


void TRestGeometry::InitGfGeometry()  
{
#if defined USE_Garfield
  fGfGeometry = new Garfield::GeometryRoot();
  fGfGeometry->SetGeometry(this);
#endif
}



//------------------------------------------------------------------------------


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

