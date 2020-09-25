///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4BiasingVolume.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4BiasingVolume.h"
using namespace std;

ClassImp(TRestGeant4BiasingVolume)
    //______________________________________________________________________________
    TRestGeant4BiasingVolume::TRestGeant4BiasingVolume() {
    fBiasingVolumeType = "virtualBox";
    // TRestGeant4BiasingVolume default constructor
}

//______________________________________________________________________________
TRestGeant4BiasingVolume::~TRestGeant4BiasingVolume() {
    // TRestGeant4BiasingVolume destructor
}

void TRestGeant4BiasingVolume::PrintBiasingVolume() {
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
