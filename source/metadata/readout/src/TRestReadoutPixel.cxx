///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutPixel.cxx
///
///             Base class for managing run data storage. It contains a TRestEvent and TRestMetadata array. 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadoutPixel.h"

ClassImp(TRestReadoutPixel)
//______________________________________________________________________________
    TRestReadoutPixel::TRestReadoutPixel()
{
    Initialize();

}


//______________________________________________________________________________
TRestReadoutPixel::~TRestReadoutPixel()
{
 //   cout << "Deleting TRestReadoutPixel" << endl;
}


void TRestReadoutPixel::Initialize()
{
}

void TRestReadoutPixel::PrintReadoutPixel( )
{
        cout << "      ## Pixel ID : " << GetID() << " position : (" << GetOriginX() << "," << GetOriginY() << ") mm size : (" << GetSizeX() << "," << GetSizeY() << ") mm rotation : " << fRotation << " degrees" << endl;
}
