#include "TRestTask.h"

#ifndef RESTTask_ValidateGeometry
#define RESTTask_ValidateGeometry

//*******************************************************************************************************
//***
//*** This macro will implement geometry validation routines
//*** For the moment it is just calling to CheckOverlaps, although
//*** additional routines could be incorparated in the future.
//***
//*******************************************************************************************************
Int_t REST_ValidateGeometry(TString gdmlName) {
    TRestGDMLParser* g = new TRestGDMLParser();
    TGeoManager* geo = g->GetGeoManager((std::string)gdmlName);

    geo->CheckOverlaps(0.0000001);
    geo->PrintOverlaps();

    // geo->GetTopVolume()->Draw("ogl");
    /////////////////////////////

    return 0;
}
#endif
