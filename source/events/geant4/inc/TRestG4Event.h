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


#ifndef RestCore_TRestG4Event
#define RestCore_TRestG4Event

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TH1D.h>
#include <TRestEvent.h>
#include <TVector3.h>
#include <TRestG4Track.h>

class TRestG4Event: public TRestEvent {

    protected:
        TVector3 fPrimaryEventOrigin;

        vector <TVector3> fPrimaryEventDirection;
        vector <Double_t> fPrimaryEventEnergy;
        
        Double_t fTotalEventEnergy;

        Double_t fTotalSensitiveVolumeEnergy;

        Int_t fNTracks;

        vector <TRestG4Track> fTrack;

        Int_t fNVolumes;
        vector <Int_t> fVolumeStored;


    public:

        Double_t GetTotalSensitiveVolumeEnergy() { return fTotalSensitiveVolumeEnergy; }
        TVector3 GetPrimaryEventDirection( int n ) { return fPrimaryEventDirection[n]; }
        TVector3 GetPrimaryEventOrigin() { return fPrimaryEventOrigin; }
        Double_t GetPrimaryEventEnergy( int n ) { return fPrimaryEventEnergy[n]; }
        Int_t GetNumberOfTracks() { return fNTracks; }
        Int_t GetNumberOfPrimaries() { return fPrimaryEventDirection.size(); }
        Int_t GetNumberOfActiveVolumes() { return fNVolumes; }
        Int_t isVolumeStored( int n ) { return fVolumeStored[n]; }
        TRestG4Track *GetTrack( int n ) { return &fTrack[n]; }
        TRestG4Track *GetTrackByID( int id ) { for( int i = 0; i < fNTracks; i++ ) if( fTrack[i].GetTrackID( ) == id ) return &fTrack[i]; return NULL; }

        void AddEnergyDepositInSensitiveVolume( Double_t eDep ) { fTotalSensitiveVolumeEnergy += eDep; }

        Double_t GetTotalEventEnergy() { return fTotalEventEnergy; }

        Double_t GetTotalDepositedEnergy()
        {
            Double_t eDep = 0;
            //cout << "Number of tracks : " << GetNumberOfTracks() << endl;
            for( int tk = 0; tk < GetNumberOfTracks(); tk++ )
            {
                eDep += GetTrack( tk )->GetTotalDepositedEnergy();
            }

            return eDep;
        }

        void SetPrimaryEventOrigin( TVector3 pos ) { fPrimaryEventOrigin = pos; }
        void SetPrimaryEventDirection( TVector3 dir ) { fPrimaryEventDirection.push_back( dir ); }
        void SetPrimaryEventEnergy( Double_t en ) { fPrimaryEventEnergy.push_back( en ); }
        void ActivateVolumeForStorage( Int_t n ) {  fVolumeStored[n] = 1; }
        void DisableVolumeForStorage( Int_t n ) {  fVolumeStored[n] = 0; }
        void AddActiveVolume( Int_t active ) { fNVolumes++; fVolumeStored.push_back( active ); }

        void SetTrackSubEventID( Int_t n, Int_t id ) { fTrack[n].SetSubEventID( id ); }
        void AddTrack( TRestG4Track trk ) { fTrack.push_back( trk ); fNTracks = fTrack.size(); fTotalEventEnergy += trk.GetTotalDepositedEnergy(); }

        void Initialize();

        void PrintEvent();


        //Construtor
        TRestG4Event();
        //Destructor
        virtual ~ TRestG4Event();

        ClassDef(TRestG4Event, 1);     // REST event superclass
};
#endif
