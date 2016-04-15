///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestLinearTrack.h
///
///             Event class to store a linearized track
///
///             Feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///
///_______________________________________________________________________________


#include "TRestLinearTrack.h"
using namespace std;

ClassImp(TRestLinearTrack)
//______________________________________________________________________________
    TRestLinearTrack::TRestLinearTrack()
{
   // TRestLinearTrack default constructor
   Initialize();
}

//______________________________________________________________________________
TRestLinearTrack::~TRestLinearTrack()
{
   // TRestLinearTrack destructor
}

void TRestLinearTrack::Initialize()
{
    TRestEvent::Initialize();

    fMeanPosition = TVector3( -1, -1, -1 );
    fTrackEnergy = 0;
}

void TRestLinearTrack::Print( Bool_t fullInfo )
{
    Double_t x = fMeanPosition.X();
    Double_t y = fMeanPosition.Y();
    Double_t z = fMeanPosition.Z();

    cout << "Linear track is ";
    if( isXY() ) cout << " XY" << endl;
    if( isXZ() ) cout << " XZ" << endl;
    if( isYZ() ) cout << " YZ" << endl;
    if( isXYZ() ) cout << " XYZ" << endl;

    cout << "Energy : " << fTrackEnergy << " electrons" << endl;
    cout << "Mean position : ( " << x << " , " << y << " , " << z << " ) " <<endl;

    if( fullInfo )
    {
        cout << "-------------------------------" << endl;
        cout << "------ Longitudinal hits ------" << 
            cout << "-------------------------------" << endl;
        fLinearCharge.Print();
        cout << "-------------------------------" << endl;
        cout << "------ Transversal hits ------" << 
            cout << "-------------------------------" << endl;
        fTransversalCharge.Print();
        cout << "-------------------------------" << endl;
    }

}
