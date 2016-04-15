///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Hits.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestG4Hits.h"

ClassImp(TRestG4Hits)
    //______________________________________________________________________________
TRestG4Hits::TRestG4Hits() : TRestHits( )
{
    // TRestG4Hits default constructor
}

//______________________________________________________________________________
TRestG4Hits::~TRestG4Hits()
{
    // TRestG4Hits destructor
}
void TRestG4Hits::AddG4Hit( TVector3 pos, Double_t en, Int_t process, Int_t volume )
{
    AddHit( pos, en );

    fProcessID.Set( fNHits );

    fProcessID[fNHits-1] = process;

    fVolumeID.Set( fNHits );

    fVolumeID[fNHits-1] = volume;
}

void TRestG4Hits::AddG4Hit( Double_t X, Double_t Y, Double_t Z, Double_t en, Int_t process, Int_t volume )
{
    AddHit( X, Y, Z, en );

    fProcessID.Set( fNHits );

    fProcessID[fNHits-1] = process;

    fVolumeID.Set( fNHits );

    fVolumeID[fNHits-1] = volume;
}

void TRestG4Hits::RemoveG4Hits( )
{
    RemoveHits( );

    fProcessID.Set(0);

    fVolumeID.Set(0);
}

Double_t TRestG4Hits::GetEnergyInVolume( Int_t volID )
{
    Double_t en = 0;

    for( int n = 0; n < fNHits; n++ )
        if( fVolumeID[n] == volID ) en += GetEnergy( n );

    return en;
}
