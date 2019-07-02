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
    TRestBiasingVolume::TRestBiasingVolume() {
    fBiasingVolumeType = "virtualBox";
    // TRestBiasingVolume default constructor
}

//______________________________________________________________________________
TRestBiasingVolume::~TRestBiasingVolume() {
    // TRestBiasingVolume destructor
}

void TRestBiasingVolume::PrintBiasingVolume() {
    cout << "-----------------------------" << endl;
    cout << "Biasing volume" << endl;
    cout << "-----------------------------" << endl;
    cout << "volume size : " << GetBiasingVolumeSize() << " mm" << endl;
    cout << "volume type : " << GetBiasingVolumeType() << endl;
    cout << "volume factor : " << GetBiasingFactor() << endl;
    cout << "volume position : ( " << GetBiasingVolumePosition().X() << " , "
         << GetBiasingVolumePosition().Y() << " , " << GetBiasingVolumePosition().Z() << " ) mm" << endl;
    cout << "Energy range : ( " << GetMinEnergy() << " , " << GetMaxEnergy() << " ) keV" << endl;
    cout << "-----------------------------" << endl;
}
