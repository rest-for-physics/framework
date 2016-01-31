///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReductionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
//
///_______________________________________________________________________________

#include "TRestTrackReductionProcess.h"
using namespace std;

ClassImp(TRestTrackReductionProcess)
//______________________________________________________________________________
TRestTrackReductionProcess::TRestTrackReductionProcess( )
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackReductionProcess::TRestTrackReductionProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();
}

//______________________________________________________________________________
TRestTrackReductionProcess::~TRestTrackReductionProcess( )
{
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestTrackReductionProcess::LoadDefaultConfig( )
{
    SetName( "trackReductionProcess" );
    SetTitle( "Default config" );

    fMinimumDistance = 1.0;
    fMaximumDistance = 10.0;
    fMaxNodes = 30;
}

//______________________________________________________________________________
void TRestTrackReductionProcess::Initialize( )
{
    SetName("trackReductionProcess");

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fOutputEvent = fOutputTrackEvent;
    fInputEvent  = fInputTrackEvent;
}

void TRestTrackReductionProcess::LoadConfig( string cfgFilename )
{

    if( LoadConfigFromFile( cfgFilename ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();

}

//______________________________________________________________________________
void TRestTrackReductionProcess::InitProcess()
{
    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestTrackReductionProcess::BeginOfEventProcess() 
{
    fOutputTrackEvent->Initialize(); 
    cout << "Processing Event : " << fInputTrackEvent->GetEventID() << endl;
}

//______________________________________________________________________________
TRestEvent* TRestTrackReductionProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputTrackEvent = (TRestTrackEvent *) evInput;


    fOutputTrackEvent->SetEventID( fInputTrackEvent->GetEventID() );
    fOutputTrackEvent->SetEventTime( fInputTrackEvent->GetEventTime() );

    // Copying the input tracks to the output track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 

    // Reducing the hits inside each track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
     //   fInputTrackEvent->PrintOnlyTracks();
        if( !fInputTrackEvent->isTopLevel( tck ) ) continue;

        TRestTrack *track = fInputTrackEvent->GetTrack( tck );
        TRestVolumeHits *hits = track->GetVolumeHits();

        Double_t distance = fMinimumDistance;
        while( distance < fMaximumDistance || hits->GetNumberOfHits() > fMaxNodes )
        {
            for( int i = 0; i < hits->GetNumberOfHits(); i++ )
                for( int j = i+1; j < hits->GetNumberOfHits(); j++ )
                {
                    if( hits->GetDistance2( i, j ) < distance * distance )
                        hits->MergeHits( i, j );
                }
            distance *= 2;
        }

        track->SetParentID( track->GetTrackID() );
        track->SetTrackID( fOutputTrackEvent->GetNumberOfTracks()+1 );

        //fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 
        fOutputTrackEvent->AddTrack( track ); 
    }

    /*
    cout << "output event" << endl;
    cout << "+++++++++++++++++" << endl;
    fOutputTrackEvent->PrintOnlyTracks();
    cout << "+++++++++++++++++" << endl;
    getchar();

    cout << "Checking levels " << endl;
    cout << "---------------------------------------------_" << endl;
    cout << "TrackEvent pointer : " << fOutputTrackEvent << endl;
    cout << fOutputTrackEvent->GetNumberOfTracks() << endl;
    cout << "TrackEvent pointer : " << fOutputTrackEvent << endl;

    fOutputTrackEvent->GetLevels();
    cout << "---------------" << endl;
    */

    //cout << "Number output of tracks : " << fOutputTrackEvent->GetNumberOfTracks() << endl;

    fOutputTrackEvent->SetLevels();
    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestTrackReductionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestTrackReductionProcess::EndProcess()
{
}

//______________________________________________________________________________
void TRestTrackReductionProcess::InitFromConfigFile( )
{
    fMinimumDistance = StringToDouble( GetParameter( "minimumDistance" ) );
    fMaximumDistance = StringToDouble( GetParameter( "maximumDistance" ) );
    fMaxNodes = StringToDouble( GetParameter( "maxNodes" ) );
}

