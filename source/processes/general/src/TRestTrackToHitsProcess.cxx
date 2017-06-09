//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackToHitsProcess.cxx
///
///             Apr 2017:   First concept (Javier Galan)
///             
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackToHitsProcess.h"
using namespace std;

ClassImp(TRestTrackToHitsProcess)
//______________________________________________________________________________
TRestTrackToHitsProcess::TRestTrackToHitsProcess( )
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackToHitsProcess::TRestTrackToHitsProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestTrackToHitsProcess::~TRestTrackToHitsProcess( )
{
    delete fInputTrackEvent;
    delete fOutputHitsEvent;
}

void TRestTrackToHitsProcess::LoadDefaultConfig( )
{
    SetName( "trackToHitsProcess" );
    SetTitle( "Default config" );

    fTrackLevel = 0.5;
}

//______________________________________________________________________________
void TRestTrackToHitsProcess::Initialize( )
{
    SetSectionName( this->ClassName() );

    fInputTrackEvent = new TRestTrackEvent();
    fOutputHitsEvent = new TRestHitsEvent();

    fOutputEvent = fOutputHitsEvent;
    fInputEvent  = fInputTrackEvent;
}

void TRestTrackToHitsProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestTrackToHitsProcess::InitProcess()
{
    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
TRestEvent* TRestTrackToHitsProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputTrackEvent = (TRestTrackEvent *) evInput;

    if( this->GetVerboseLevel() >= REST_Debug )
        fInputTrackEvent->PrintOnlyTracks();

    for ( int n = 0; n < fInputTrackEvent->GetNumberOfTracks(); n++ )
        if ( fInputTrackEvent->GetLevel( n ) == fTrackLevel )
        {
            TRestHits *hits = fInputTrackEvent->GetTrack( n )->GetHits();

            for( int h = 0; h < hits->GetNumberOfHits(); h++ )
                fOutputHitsEvent->AddHit( hits->GetX(h), hits->GetY(h), hits->GetZ(h), hits->GetEnergy(h) );
        }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestTrackToHitsProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestTrackToHitsProcess::EndProcess()
{
}

//______________________________________________________________________________
void TRestTrackToHitsProcess::InitFromConfigFile( )
{
    fTrackLevel = StringToInteger( GetParameter( "trackLevel", "1" ) );
}

