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
using namespace std;

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

    fPrimaryEventDirection.clear();
    fPrimaryEventEnergy.clear();
    fPrimaryEventOrigin.SetXYZ( 0, 0, 0 );

    fTrack.clear();
    fNTracks = 0;

    fTotalDepositedEnergy = 0;
    fSensitiveVolumeEnergy = 0;
    fMaxSubEventID = 0;
}

void TRestG4Event::AddActiveVolume( ) 
{
    fNVolumes++; 
    fVolumeStored.push_back( 1 ); 
    fVolumeDepositedEnergy.push_back(0);
}

void TRestG4Event::ClearVolumes( )
{
    fNVolumes = 0;
    fVolumeStored.clear();
    fVolumeDepositedEnergy.clear();
}

void TRestG4Event::AddEnergyDepositToVolume( Int_t volID, Double_t eDep ) 
{ 
    fVolumeDepositedEnergy[volID] += eDep; 
}

void TRestG4Event::SetTrackSubEventID( Int_t n, Int_t id ) 
{
    fTrack[n].SetSubEventID( id ); 
    if( fMaxSubEventID < id ) fMaxSubEventID = id;
}

void TRestG4Event::AddTrack( TRestG4Track trk ) 
{ 
    fTrack.push_back( trk ); 
    fNTracks = fTrack.size(); 
    fTotalDepositedEnergy += trk.GetTotalDepositedEnergy(); 
    for( int n = 0; n < GetNumberOfActiveVolumes(); n++ )
        fVolumeDepositedEnergy[n] += trk.GetEnergyInVolume( n );
}

Double_t TRestG4Event::GetTotalDepositedEnergyFromTracks()
{
    Double_t eDep = 0;

    for( int tk = 0; tk < GetNumberOfTracks(); tk++ )
        eDep += GetTrack( tk )->GetTotalDepositedEnergy();

    return eDep;
}

TVector3 TRestG4Event::GetMeanPositionInVolume(Int_t volID)
{
    TVector3 pos;
    Double_t eDep = 0;

    for( int t = 0; t < GetNumberOfTracks(); t++ )
    {
        TRestG4Track *tck = GetTrack( t );
        if( tck->GetEnergyInVolume( volID ) > 0 )
        {
            pos += tck->GetMeanPositionInVolume( volID ) * tck->GetEnergyInVolume( volID );

            eDep += tck->GetEnergyInVolume( volID );
        }
    }

    if( eDep == 0 )
    {
        TVector3 pos;
        Double_t nan = TMath::QuietNaN();
        return TVector3( nan, nan, nan );
    }

    pos = (1/eDep) * pos;
    return pos;
}

TVector3 TRestG4Event::GetFirstPositionInVolume(Int_t volID)
{
    for( int t = 0; t < GetNumberOfTracks(); t++ )
        if( GetTrack( t )->GetEnergyInVolume( volID ) > 0 )
            return GetTrack( t )->GetFirstPositionInVolume( volID );

    TVector3 pos;
    Double_t nan = TMath::QuietNaN();
    return TVector3( nan, nan, nan );
}

TVector3 TRestG4Event::GetLastPositionInVolume(Int_t volID)
{
    for( int t = GetNumberOfTracks()-1; t >= 0; t-- )
        if( GetTrack( t )->GetEnergyInVolume( volID ) > 0 )
            return GetTrack( t )->GetLastPositionInVolume( volID );

    TVector3 pos;
    Double_t nan = TMath::QuietNaN();
    return TVector3( nan, nan, nan );
}

TRestG4Track *TRestG4Event::GetTrackByID( int id ) 
{ 
    for( int i = 0; i < fNTracks; i++ ) 
        if( fTrack[i].GetTrackID( ) == id ) 
            return &fTrack[i]; 
    return NULL; 
}

Int_t TRestG4Event::GetNumberOfHits( )
{
    Int_t hits = 0;
    for( int i = 0; i < fNTracks; i++ ) 
        hits += GetTrack( i )->GetNumberOfHits();

    return hits;
}

TRestHits TRestG4Event::GetHits( )
{
    TRestHits hits;
    for( int t = 0; t < fNTracks; t++ ) 
    {
        TRestG4Hits *g4Hits = GetTrack( t )->GetHits( );
        for( int n = 0; n < g4Hits->GetNumberOfHits(); n++ )
        {
            Double_t x = g4Hits->GetX(n);
            Double_t y = g4Hits->GetY(n);
            Double_t z = g4Hits->GetZ(n);
            Double_t en = g4Hits->GetEnergy(n);

            hits.AddHit( x, y, z, en );
        }
    }

    return hits;
}

Int_t TRestG4Event::GetNumberOfTracksForParticle( TString parName )
{
    Int_t tcks = 0;
    for( Int_t t = 0; t < GetNumberOfTracks(); t++ )
        if( GetTrack( t )->GetParticleName() == parName ) tcks++;

    return tcks;
}

Int_t TRestG4Event::GetEnergyDepositedByParticle( TString parName )
{
    Double_t en = 0;
    for( Int_t t = 0; t < GetNumberOfTracks(); t++ )
    {
        if( GetTrack( t )->GetParticleName() == parName )
            en += GetTrack( t )->GetEnergy();
    }

    return en;
}

void TRestG4Event::PrintEvent( int maxTracks, int maxHits )
{
    TRestEvent::PrintEvent();

    cout.precision(4);

    cout << "Total energy : " << fTotalDepositedEnergy << " keV" << endl;
    cout << "Sensitive volume energy : " << fSensitiveVolumeEnergy << " keV" << endl;
    cout << "Source origin : (" << fPrimaryEventOrigin.X() << "," << fPrimaryEventOrigin.Y() << "," << fPrimaryEventOrigin.Z() << ") mm" << endl;

    for( int n = 0; n < GetNumberOfPrimaries(); n++ )
    {
        TVector3 *dir = &fPrimaryEventDirection[n];
        cout << "Source " << n << " direction : (" << dir->X() << "," << dir->Y() << "," << dir->Z() << ")" << endl;
        cout << "Source " << n << " energy : " << fPrimaryEventEnergy[n] << " keV" << endl;
    }

    cout << "Number of active volumes : " << GetNumberOfActiveVolumes() << endl;
    for( int i = 0; i < GetNumberOfActiveVolumes(); i++ )
    {
        if( isVolumeStored(i) )
        {
            cout << "Active volume " << i << " has been stored." << endl;
            cout << "Total energy deposit in volume " << i << " : " << fVolumeDepositedEnergy[i] << " keV" << endl;
        }
        else cout << "Active volume " << i << " has not been stored" << endl;
    }

    cout << "---------------------------------------------------------------------------" << endl;
    cout << "Total number of tracks : " << fNTracks << endl;

    register int ntracks = GetNumberOfTracks();
    if ( maxTracks > 0 ) 
    {
      ntracks = min( maxTracks, GetNumberOfTracks() );
      cout << " Printing only the first " << ntracks << " tracks" <<endl;
    }

    for( int n = 0; n < ntracks; n++ )
        GetTrack(n)->PrintTrack( maxHits );
}
