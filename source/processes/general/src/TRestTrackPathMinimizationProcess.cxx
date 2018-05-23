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

const int Nmax = 30;

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

    cout << "Event id : " << evInput->GetID() << endl;

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
        TRestVolumeHits *originHits = fInputTrackEvent->GetOriginTrackById( tckId )->GetVolumeHits();

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

        Float_t x[nHits], y[nHits], z[nHits];

        hits->GetXArray( x );
        hits->GetYArray( y );
        hits->GetZArray( z );

        Bool_t isXZ = fInputTrackEvent->GetTrack( tck )->isXZ();
        Bool_t isYZ = fInputTrackEvent->GetTrack( tck )->isYZ();
        Bool_t isXYZ = fInputTrackEvent->GetTrack( tck )->isXYZ();

        Int_t rval = 0;
        int bestPath[nHits];

        if( GetVerboseLevel() >= REST_Warning && nHits > Nmax )
        {
            cout << "REST WARNING. Number of nodes : " << nHits << endl;
            cout << "This number is higher than the recommended maximum for heldkarp method." << endl;
            cout << "Still, you might get a reasonable track path minimization" << endl;
        }


        if( !fWeightHits && nHits > 3 )
        {
            int xInt[nHits], yInt[nHits], zInt[nHits];

            for( int i = 0; i < nHits; i++ )
            {
                xInt[i] = (int) (10.* x[i]);
                yInt[i] = (int) (10.* y[i]);
                zInt[i] = (int) (10.* z[i]);
            }

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
        else if( nHits > 3 )
        {
            Int_t segment_count = nHits * ( nHits - 1 ) / 2;
            int *elen = (int *) malloc( segment_count * sizeof( int ) );

            Double_t lenghtReduction = 0.25;
            Double_t fTubeRadius = 2.5;

            int k = 0;
            for ( int i = 0; i < nHits; i++ )
            {
                bestPath[i] = i;
                for ( int j = 0; j < i; j++) 
                {
                    TVector3 x0, x1, pos0, pos1;

                    x0 = hits->GetPosition( i );
                    x1 = hits->GetPosition( j );

                    pos0 = lenghtReduction * ( x1-x0 ) + x0;
                    pos1 = (1-lenghtReduction) * ( x1-x0 ) + x0;

                    Double_t distance = (x1-x0).Mag();
                    Double_t energyBetween = originHits->GetEnergyInCylinder( pos0, pos1, fTubeRadius );

                    if( GetVerboseLevel() >= REST_Extreme )
                    {
                        cout << "Distance : " << distance << endl;
                        cout << "Segment energy (" << i << " , " << j << " ) : " << energyBetween << endl;
                    }

                    if( energyBetween > 0 )
                        elen[k] = (int) (100. * distance/energyBetween);
                    else
                        elen[k] = 10000;
                    if( elen[k] > 10000 ) elen[k] = 10001;

                    //elen[k] = (int) ( 10. * distance );
                    k++;
                }
            }

            if( GetVerboseLevel() >= REST_Extreme )
                GetChar();

            if( GetVerboseLevel() >= REST_Extreme )
            {
                for( int n = 0; n < segment_count; n++ )
                    cout << "n : " << n << " elen : " << elen[n] << endl;
                GetChar();
            }


            rval = TrackMinimization_3D_segment( nHits, elen, bestPath );

            free( elen );

            if( GetVerboseLevel() >= REST_Extreme )
                GetChar();
        }
        else
        {
            bestPath[0] = 0;
            bestPath[1] = 1;
            bestPath[2] = 2;
        }

        if( GetVerboseLevel() >= REST_Info )
            cout << "hits : " << nHits << endl;

        if( GetVerboseLevel() >= REST_Warning && rval != 0 ) { cout << "REST WARNING. TRestTrackPathMinimizationProcess. HELDKARP FAILED." << endl; }

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
    fWeightHits = true;
    if( (TString) GetParameter( "weightHits", "false" ) == "false" )
        fWeightHits = false;
}

