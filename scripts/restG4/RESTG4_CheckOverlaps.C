//#include <TObject.h>
//#include <TString.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <TBranch.h>

#include <iostream>
using namespace std;


Int_t RESTG4_CheckOverlaps( TString gdmlName )
{
    gSystem->Load("librestcore.so");

    TGeoManager *geo = new TGeoManager();
    geo->Import(gdmlName);
    
    geo->CheckOverlaps(0.0000001);
    geo->PrintOverlaps();
    
    //geo->GetTopVolume()->Draw("ogl");
    /////////////////////////////

}
