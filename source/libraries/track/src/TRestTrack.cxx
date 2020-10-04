///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrack.h
///
///             Event class to store signals form simulation and acquisition
///             events
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________

#include "TRestTrack.h"
using namespace std;

ClassImp(TRestTrack);
//______________________________________________________________________________
TRestTrack::TRestTrack() {
    // TRestTrack default constructor
}

//______________________________________________________________________________
TRestTrack::~TRestTrack() {
    // TRestTrack destructor
}

void TRestTrack::Initialize() {
    fTrackID = 0;
    fParentID = 0;
    fTrackEnergy = 0;
    fTrackLength = 0;
    fVolumeHits.RemoveHits();
}

void TRestTrack::SetVolumeHits(TRestVolumeHits hits) {
    fVolumeHits = hits;
    fTrackEnergy = hits.GetEnergyIntegral();
    fTrackLength = hits.GetTotalDistance();
}

void TRestTrack::RemoveVolumeHits() {
    fVolumeHits.RemoveHits();
    fTrackEnergy = 0;
    fTrackLength = 0;
}

void TRestTrack::PrintTrack(Bool_t fullInfo) {
    Double_t x = GetMeanPosition().X();
    Double_t y = GetMeanPosition().Y();
    Double_t z = GetMeanPosition().Z();

    cout << "Track ID : " << fTrackID << " Parent ID : " << fParentID;

    if (isXY()) cout << " is XY " << endl;
    if (isXZ()) cout << " is XZ " << endl;
    if (isYZ()) cout << " is YZ " << endl;
    if (isXYZ()) cout << " is XYZ " << endl;
    cout << "Energy : " << fTrackEnergy << endl;
    cout << "Length : " << fTrackLength << endl;
    cout << "Mean position : ( " << x << " , " << y << " , " << z << " ) " << endl;
    cout << "Number of track hits : " << fVolumeHits.GetNumberOfHits() << endl;
    cout << "----------------------------------------" << endl;

    if (fullInfo) {
        fVolumeHits.PrintHits();
        cout << "----------------------------------------" << endl;
    }
}
