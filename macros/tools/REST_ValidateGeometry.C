#include "TRestTask.h"
Int_t REST_ValidateGeometry( TString gdmlName )
{
    TGeoManager *geo = new TGeoManager();
    geo->Import(gdmlName);
    
    geo->CheckOverlaps(0.0000001);
    geo->PrintOverlaps();
    
    //geo->GetTopVolume()->Draw("ogl");
    /////////////////////////////

    return 0;
}
