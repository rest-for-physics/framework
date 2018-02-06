///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeometry.h
///
///             A geometry class to store detector geometry 
///
///             jul 2015:   First concept
///                 J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestGeometry
#define RestCore_TRestGeometry

#include <iostream>
#include <vector>

#include "TObject.h"
#include "TGeoMaterial.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"


#include <TRestGas.h>

#if defined USE_Garfield
#include "GeometryRoot.hh"
#include "ComponentBase.hh"
#include "Sensor.hh"
#endif


using namespace std;


class TRestGeometry:public TGeoManager {
 protected:
#if defined USE_Garfield
   Garfield::GeometryRoot  *fGfGeometry;   //!///< Pointer to Garfield::GeometryRoot object of the geometry
   vector<Garfield::ComponentBase*> vGfComponent;   //!///< Vector of pointers to Garfield Component object
   vector<Garfield::Sensor*> vGfSensor;   //!///< Vector of pointers to Garfield Sensor object
   TGeoNode* fDriftElec;  //!///< pointer to drift electrode
   vector<TGeoNode*> vReadoutElec;  //!///< vector of pointers to readout planes
#endif

 public:

   //Construtor
   TRestGeometry();
   //Destructor
   virtual ~ TRestGeometry();


   /// initialize Garfield::GeometryRoot geometry object
   void InitGfGeometry();

#if defined USE_Garfield
   /// Return pointer to Garfield::GeometryRoot geometry object
   Garfield::GeometryRoot*  GetGfGeometry()  { return fGfGeometry; }

   /// Set Garfield medium of gas volume to TGeometry one
   void  SetGfGeoMedium(const char* geoMatName, TRestGas* gas)
           { fGfGeometry->SetMedium(geoMatName, gas->GetGasMedium()); }

   /// Set Garfield field component
   void  AddGfComponent(Garfield::ComponentBase* c)
           { c->SetGeometry(fGfGeometry); vGfComponent.push_back(c); }

   /// Set Garfield field component
   void  AddGfSensor(Garfield::Sensor* s)
           { vGfSensor.push_back(s); }

   /// Set drift electrode node
   void  SetDriftElecNode(TGeoNode* n)  { fDriftElec = n; }

   /// Add readout electrode node to the list
   void  AddReadoutElecNode(TGeoNode* n)  { vReadoutElec.push_back(n); }

   /// Get drift electrode node
   TGeoNode* GetDriftElecNode()  { return fDriftElec; }

   /// Get number of readout electrode nodes
   int GetNReadoutElecNodes()  { return vReadoutElec.size(); }

   /// Get i^th readout electrode node
   TGeoNode* GetReadoutElecNode(unsigned int i)
               { if (i<vReadoutElec.size()) return vReadoutElec[i];
                 else return 0; }

   /// Get i^th Gf component
   Garfield::ComponentBase*  GetGfComponent(unsigned int i)
                              { if (i<vGfComponent.size()) return vGfComponent[i];
                                else return 0; }

   /// Getnumber of Gf components
   int  GetNbOfGfComponent()  { return vGfComponent.size(); }

   /// Get i^th Gf sensor
   Garfield::Sensor*  GetGfSensor(unsigned int i)
                        { if (i<vGfSensor.size()) return vGfSensor[i];
                          else return 0; }

   /// Getnumber of Gf sensors
   int  GetNbOfGfSensor()  { return vGfSensor.size(); }

   /// Get Gf Medium at position
   Garfield::Medium*  GetGfMedium(double x, double y, double z)
                        { FindNode(x/10.,y/10.,z/10.);  // needed due to a bug in TGeoNavigator
                          return fGfGeometry->GetMedium(x/10.,y/10.,z/10.); }  // mm to cm
#endif


   void PrintGeometry();

   ClassDef(TRestGeometry, 1);     // REST event superclass
};

#endif