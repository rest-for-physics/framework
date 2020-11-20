#include "TRestTask.h"

#ifndef RESTTask_ValidateGeometry
#define RESTTask_ValidateGeometry

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_ValidateGeometry(TString gdmlName) {
    TGeoManager* geo = new TGeoManager();
    geo->Import(gdmlName);

    geo->CheckOverlaps(0.0000001);
    geo->PrintOverlaps();

    // geo->GetTopVolume()->Draw("ogl");
    /////////////////////////////

    return 0;
}
