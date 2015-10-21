///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Event.h
///
///             G4 Event class to store results from Geant4 REST simulation  
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestG4Event.h"

ClassImp(TRestG4Event)
//______________________________________________________________________________
    TRestG4Event::TRestG4Event()
{
    fNVolumes = 0;
   // TRestG4Event default constructor
   Initialize();
}

//______________________________________________________________________________
TRestG4Event::~TRestG4Event()
{
   // TRestG4Event destructor
}

void TRestG4Event::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestG4Event";

    //cout << "Initializing G4Event" << endl;
    fPrimaryEventDirection.clear();
    fPrimaryEventEnergy.clear();
    fPrimaryEventOrigin.SetXYZ( 0, 0, 0 );

    fTotalSensitiveVolumeEnergy = 0;

    /*
    fVolumeStored.clear();
    fVolumeStored.push_back(0);
    fVolumeStored.push_back(0);
    */

    fTrack.clear();
    fNTracks = 0;

    fTotalEventEnergy = 0;
}

void TRestG4Event::PrintEvent()
{
    TRestEvent::PrintEvent();

    cout.precision(4);

    cout << "Source origin : (" << fPrimaryEventOrigin.X() << "," << fPrimaryEventOrigin.Y() << "," << fPrimaryEventOrigin.Z() << ") mm" << endl;

    for( int n = 0; n < GetNumberOfPrimaries(); n++ )
    {
        cout << "Source " << n << " direction : (" << fPrimaryEventDirection[n].X() << "," << fPrimaryEventDirection[n].Y() << "," << fPrimaryEventDirection[n].Z() << ")" << endl;
        cout << "Source " << n << " energy : " << fPrimaryEventEnergy[n] << " keV" << endl;
    }
    cout << "Number of active volumes : " << GetNumberOfActiveVolumes() << endl;
    for( int i = 0; i < GetNumberOfActiveVolumes(); i++ )
    {
        if( isVolumeStored(i) ) cout << "Active volume " << i << " has been stored" << endl;
        else cout << "Active volume " << i << " has not been stored" << endl;
    }

    cout << "---------------------------------------------------------------------------" << endl;
    cout << "Total number of tracks : " << fNTracks << endl;
    for( int n = 0; n < GetNumberOfTracks(); n++ )
    {
        GetTrack(n)->PrintTrack();
    }


    /*

        vector <TRestG4Track> fTrack;
        */


}
