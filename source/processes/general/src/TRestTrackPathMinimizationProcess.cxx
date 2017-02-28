//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackPathMinimizationProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackPathMinimizationProcess.h"
using namespace std;

const int Nmax = 50;

ClassImp(TRestTrackPathMinimizationProcess)
//______________________________________________________________________________
TRestTrackPathMinimizationProcess::TRestTrackPathMinimizationProcess( )
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackPathMinimizationProcess::TRestTrackPathMinimizationProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();
}

//______________________________________________________________________________
TRestTrackPathMinimizationProcess::~TRestTrackPathMinimizationProcess( )
{
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestTrackPathMinimizationProcess::LoadDefaultConfig( )
{
    SetName( "trackPathMinimizationProcess" );
    SetTitle( "Default config" );

}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::Initialize( )
{
    SetSectionName( this->ClassName() );

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fOutputEvent = fOutputTrackEvent;
    fInputEvent  = fInputTrackEvent;
}

void TRestTrackPathMinimizationProcess::LoadConfig( std::string cfgFilename, std::string name )
{

    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );

    if( fMaxNodes > Nmax )
    {
        cout << "REST WARNING : Maximum number of nodes is " << Nmax << endl;
        cout << "Reducing maximum number of nodes from " << fMaxNodes << " to " << Nmax << endl;
        fMaxNodes = Nmax;
    }

    PrintMetadata();

}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::InitProcess()
{
}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::BeginOfEventProcess() 
{
    fOutputTrackEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestTrackPathMinimizationProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputTrackEvent = (TRestTrackEvent *) evInput;


    if( this->GetVerboseLevel() >= REST_Debug )
        cout << "TRestTrackPathMinimizationProcess. Number of tracks : " << fInputTrackEvent->GetNumberOfTracks() << endl;

    // Copying the input tracks to the output track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 

    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fInputTrackEvent->isTopLevel( tck ) ) continue;
        Int_t tckId = fInputTrackEvent->GetTrack( tck )->GetTrackID();

        TRestVolumeHits *hits = fInputTrackEvent->GetTrack(tck)->GetVolumeHits();
        Int_t nHits = hits->GetNumberOfHits();

        /* {{{ Debug output */

        if( this->GetVerboseLevel() >= REST_Debug )
        {
            Int_t pId = fInputTrackEvent->GetTrack( tck )->GetParentID();
            cout << "Track : " << tck << " TrackID : " << tckId << " ParentID : " << pId << endl;
            cout << "-----------------" << endl;
            hits->PrintHits();
            cout << "-----------------" << endl;
            GetChar();
        }
        /* }}} */

        Float_t x[fMaxNodes], y[fMaxNodes], z[fMaxNodes];

        hits->GetXArray( x );
        hits->GetYArray( y );
        hits->GetZArray( z );

        int xInt[Nmax], yInt[Nmax], zInt[Nmax], bestPath[Nmax];

        for( int i = 0; i < fMaxNodes; i++ )
        {
            xInt[i] = (int) (10.* x[i]);
            yInt[i] = (int) (10.* y[i]);
            zInt[i] = (int) (10.* z[i]);
        }

        Bool_t isXZ = fInputTrackEvent->GetTrack( tck )->isXZ();
        Bool_t isYZ = fInputTrackEvent->GetTrack( tck )->isYZ();
        Bool_t isXYZ = fInputTrackEvent->GetTrack( tck )->isXYZ();

        Int_t rval = 0;
        if( nHits > 3 )
        {
            if( isYZ && rval == 0 )
            {
                if( this->GetVerboseLevel() >= REST_Debug )
                    cout << "Minimizing track in Y" << endl;
                rval = TrackMinimization_2D( yInt, zInt, nHits, bestPath );
            }

            if( isXZ && rval == 0 )
            {
                if( this->GetVerboseLevel() >= REST_Debug )
                    cout << "Minimizing track in X" << endl;
                rval = TrackMinimization_2D( xInt, zInt, nHits, bestPath );
            }

            if( isXYZ && rval == 0 )
            {
                if( this->GetVerboseLevel() >= REST_Debug )
                    cout << "Minimizing track in XYZ" << endl;
                rval = TrackMinimization_3D( xInt, yInt, zInt, nHits, bestPath );
            }
        }
        else
        {
            bestPath[0] = 0;
            bestPath[1] = 1;
            bestPath[2] = 2;
        }

        TRestVolumeHits bestHitsOrder;

        if( rval != 0 ) fOutputTrackEvent->SetOK( false );

        for( int i = 0; i < nHits; i++ )
        {
            Int_t n;
            if( rval == 0 ) n = bestPath[i];
            else n = i;

            Double_t x = hits->GetX(n);
            Double_t y = hits->GetY(n);
            Double_t z = hits->GetZ(n);
            Double_t sX = hits->GetSigmaX(n);
            Double_t sY = hits->GetSigmaY(n);
            Double_t sZ = hits->GetSigmaZ(n);
            Double_t energy = hits->GetEnergy(n);

            bestHitsOrder.AddHit( x, y, z, energy, sX, sY, sZ);
        }

        // TODO We must also copy other track info here
        TRestTrack bestTrack;
        bestTrack.SetTrackID( fOutputTrackEvent->GetNumberOfTracks() + 1);

        bestTrack.SetParentID( tckId );

        bestTrack.SetVolumeHits( bestHitsOrder );

        fOutputTrackEvent->AddTrack( &bestTrack );
    }

    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::EndProcess()
{
}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::InitFromConfigFile( )
{
    fMaxNodes = StringToDouble( GetParameter( "maxNodes" ) );
}

