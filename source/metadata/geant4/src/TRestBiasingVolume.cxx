///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestBiasingVolume.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestBiasingVolume.h"
using namespace std;

ClassImp(TRestBiasingVolume)
//______________________________________________________________________________
    TRestBiasingVolume::TRestBiasingVolume()
{
    fBiasingVolumeType = "virtualBox";
   // TRestBiasingVolume default constructor
}

//______________________________________________________________________________
TRestBiasingVolume::~TRestBiasingVolume()
{
   // TRestBiasingVolume destructor
}

Int_t TRestBiasingVolume::isInside( Double_t x, Double_t y, Double_t z )
{
    if( fVolumeType.Contains( "virtualBox" ) )
    {
        if( x < fVolumeSize/2. && x > -fVolumeSize/2. )
            if( y < fVolumeSize/2. && y > -fVolumeSize/2. )
                if( z < fVolumeSize/2. && z > -fVolumeSize/2. )
                    return 1;
    }
    else if( fVolumeType == "virtualSphere" )
    {
        Double_t r2 = x*x + y*y + z*z;
        if( r2 < fVolumeSize*fVolumeSize )
            return 1;
    }
    else
    {
        std::cout << "TRestBiasingVolume::isInside. Biasing volume type not found" << std::endl;
    }
    return 0;
}

void TRestBiasingVolume::PrintBiasingVolume( )
{
    cout << "-----------------------------" << endl;
    cout << "Biasing volume" << endl;
    cout << "-----------------------------" << endl;
    cout << "volume size : " << GetBiasingVolumeSize() << " mm" << endl;
    cout << "volume type : " << GetBiasingVolumeType()  << endl;
    cout << "volume factor : " << GetBiasingFactor() << endl;
    cout << "volume position : ( " << GetBiasingVolumePosition().X() << " , " << GetBiasingVolumePosition().Y() << " , " << GetBiasingVolumePosition().Z() << " ) mm" << endl;
    cout << "Energy range : ( " << GetMinEnergy() << " , " << GetMaxEnergy() << " ) keV" << endl;
    cout << "-----------------------------" << endl;
}

