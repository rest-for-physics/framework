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
using namespace std;

#include "TObject.h"
#include "TGeoMaterial.h"
#include "TGeoManager.h"

class TRestGeometry:public TGeoManager {
 protected:

 public:

   void PrintGeometry();
   //Construtor
   TRestGeometry();
   //Destructor
   virtual ~ TRestGeometry();


   ClassDef(TRestGeometry, 1);     // REST event superclass
};
#endif

