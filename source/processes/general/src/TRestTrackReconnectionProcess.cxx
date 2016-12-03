//////////////////////////////////////////////////////////////////////////
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReconnectionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackReconnectionProcess.h"
using namespace std;

ClassImp(TRestTrackReconnectionProcess)
//______________________________________________________________________________
TRestTrackReconnectionProcess::TRestTrackReconnectionProcess( )
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackReconnectionProcess::TRestTrackReconnectionProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();
}

//______________________________________________________________________________
TRestTrackReconnectionProcess::~TRestTrackReconnectionProcess( )
{
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestTrackReconnectionProcess::LoadDefaultConfig( )
{
    SetName( "trackReconnectionProcess" );
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::Initialize( )
{
    SetName("trackReconnectionProcess");

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fOutputEvent = fOutputTrackEvent;
    fInputEvent  = fInputTrackEvent;
}

void TRestTrackReconnectionProcess::LoadConfig( std::string cfgFilename, std::string name )
{

    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();

}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();

    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::BeginOfEventProcess() 
{
    fOutputTrackEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestTrackReconnectionProcess::ProcessEvent( TRestEvent *evInput )
{
    Int_t trackBranches = 0;

    fInputTrackEvent = (TRestTrackEvent *) evInput;

    // Copying the input tracks to the output track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 

    cout << "Event ID : " << fInputTrackEvent->GetID() << endl;

    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fInputTrackEvent->isTopLevel( tck ) ) continue;
        Int_t tckId = fInputTrackEvent->GetTrack( tck )->GetTrackID();

        TRestVolumeHits *hits = fInputTrackEvent->GetTrack(tck)->GetVolumeHits();
        Int_t nHits = hits->GetNumberOfHits();

        /* {{{ Debug output */
        cout << "Input hits" << endl;
        Int_t pId = fInputTrackEvent->GetTrack( tck )->GetParentID();
        cout << "Track : " << tck << " TrackID : " << tckId << " ParentID : " << pId << endl;
        cout << "-----------------" << endl;
        /* }}} */

        cout << endl;
        meanDistance = 0;
        for( int n = 1; n < nHits; n++ )
            meanDistance += hits->GetDistance( n-1, n );
        meanDistance /= nHits;
        sigma = TMath::Sqrt( meanDistance );

        /* {{{ Debug output 
        cout << "Mean ; " << meanDistance << endl;
        cout << "Sigma : " << sigma << endl;
        cout << endl;
        }}} */

        if( meanDistance == 0 ) continue; // We have just 1-hit

        vector <TRestVolumeHits> subHitSets;
        BreakTracks( hits, subHitSets );
        ReconnectTracks( subHitSets );

        // We create the new track and add it giving its parent ID
        TRestTrack bestTrack;
        bestTrack.SetTrackID( fOutputTrackEvent->GetNumberOfTracks() + 1);

        bestTrack.SetParentID( tckId );

        bestTrack.SetVolumeHits( subHitSets[0] );

        TRestHits finalHits = (TRestHits) subHitSets[0];

        Int_t tBranches = GetTrackBranches( finalHits, meanDistance, sigma );

        // We just take the value for the track with more number of branches
        if( tBranches > trackBranches ) trackBranches = tBranches;

        fOutputTrackEvent->AddTrack( &bestTrack );
    }

    TString obsName = this->GetName() + (TString) ".branches";
    fAnalysisTree->SetObservableValue( obsName, trackBranches );
    //cout << "Track branches : " << trackBranches << endl;

    return fOutputTrackEvent;
}

void TRestTrackReconnectionProcess::BreakTracks( TRestVolumeHits *hits, vector <TRestVolumeHits>& hitSets )
{
        TRestVolumeHits subHits;
        for( int n = 0; n < hits->GetNumberOfHits(); n++ )
        {
            Double_t x = hits->GetX(n);
            Double_t y = hits->GetY(n);
            Double_t z = hits->GetZ(n);
            Double_t sX = hits->GetSigmaX(n);
            Double_t sY = hits->GetSigmaY(n);
            Double_t sZ = hits->GetSigmaZ(n);
            Double_t energy = hits->GetEnergy(n);

            /* {{{ Debug output 
            if( n > 0 )
            {
                cout << "Distance : " << hits->GetDistance( n-1, n );
                if ( hits->GetDistance( n-1, n ) > meanDistance + 0.5 * sigma ) cout << " BREAKKKK";
                cout << endl;
            }
            }}} */

            if( n > 0 && hits->GetDistance( n-1, n ) > meanDistance + 0.5 * sigma ) 
            {
                hitSets.push_back( subHits );
                subHits.RemoveHits();
            }

            subHits.AddHit( x, y, z, energy, sX, sY, sZ);
            // cout << "H : " << n << " X : " << x << " Y : " << y << " Z : " << z << endl;
        }
        hitSets.push_back( subHits );


}

void TRestTrackReconnectionProcess::ReconnectTracks( vector <TRestVolumeHits>& hitSets )
{
    Int_t nSubTracks = hitSets.size();

    if ( nSubTracks == 1 ) return;

    Double_t minDistance = 1.e10;

    Int_t tracks[2][2];

    Int_t nHits[nSubTracks];
    for( int i = 0; i < nSubTracks; i++ )
        nHits[i] = hitSets[i].GetNumberOfHits();

    /* {{{ Debug output
    cout << "ORIGINAL" << endl;
    cout << "--------" << endl;
    for( int i = 0; i < nSubTracks; i++ )
    {
        cout << "Subset : " << i << endl;
        cout << " Sub hits : " << nHits[i] << endl;

        hitSets[i].PrintHits();
    }
    cout << "--------" << endl;
    }}} */

    /* {{{ Finds the closest sub-track extremes */
    for( int i = 0; i < nSubTracks; i++ )
    {
        for( int j = 0; j < nSubTracks; j++ )
        {
            if( i != j )
            {
                Int_t x1_0 = hitSets[i].GetX(0);
                Int_t x1_1 = hitSets[i].GetX(nHits[i]-1);

                Int_t y1_0 = hitSets[i].GetY(0);
                Int_t y1_1 = hitSets[i].GetY(nHits[i]-1);

                Int_t z1_0 = hitSets[i].GetZ(0);
                Int_t z1_1 = hitSets[i].GetZ(nHits[i]-1);

                Int_t x2_0 = hitSets[j].GetX(0);
                Int_t x2_1 = hitSets[j].GetX(nHits[j]-1);

                Int_t y2_0 = hitSets[j].GetY(0);
                Int_t y2_1 = hitSets[j].GetY(nHits[j]-1);

                Int_t z2_0 = hitSets[j].GetZ(0);
                Int_t z2_1 = hitSets[j].GetZ(nHits[j]-1);

                Double_t d;
                d = TMath::Sqrt( (x1_0 - x2_0) * (x1_0 - x2_0) + (y1_0 - y2_0) * (y1_0 - y2_0) + (z1_0 - z2_0) * (z1_0 - z2_0) );

                if( d < minDistance )
                {
                    tracks[0][0] = i;
                    tracks[0][1] = 0;
                    tracks[1][0] = j;
                    tracks[1][1] = 0;
                    minDistance = d;
                }

                d = TMath::Sqrt( (x1_0 - x2_1) * (x1_0 - x2_1) + (y1_0 - y2_1) * (y1_0 - y2_1) + (z1_0 - z2_1) * (z1_0 - z2_1) );

                if( d < minDistance )
                {
                    tracks[0][0] = i;
                    tracks[0][1] = 0;
                    tracks[1][0] = j;
                    tracks[1][1] = 1;
                    minDistance = d;
                }

                d = TMath::Sqrt( (x1_1 - x2_0) * (x1_1 - x2_0) + (y1_1 - y2_0) * (y1_1 - y2_0) + (z1_1 - z2_0) * (z1_1 - z2_0) );

                if( d < minDistance )
                {
                    tracks[0][0] = i;
                    tracks[0][1] = 1;
                    tracks[1][0] = j;
                    tracks[1][1] = 0;
                    minDistance = d;
                }

                d = TMath::Sqrt( (x1_1 - x2_1) * (x1_1 - x2_1) + (y1_1 - y2_1) * (y1_1 - y2_1) + (z1_1 - z2_1) * (z1_1 - z2_1) );

                if( d < minDistance )
                {
                    tracks[0][0] = i;
                    tracks[0][1] = 1;
                    tracks[1][0] = j;
                    tracks[1][1] = 1;
                    minDistance = d;
                }

            }
        }
    }
    /* }}} */

    /* {{{ Debug output
    cout << "Tracks" << endl;
    cout << tracks[0][0] << " ::: " << tracks[0][1] << endl;
    cout << tracks[1][0] << " ::: " << tracks[1][1] << endl;
    }}} */

    TRestVolumeHits newHits;
    newHits.RemoveHits();

    Int_t tck1 = tracks[0][0];
    Int_t tck2 = tracks[1][0];

    /* {{{ Rejoins the closest sub-track extremes into 1-single track */
    if( tracks[0][1] == 0 && tracks[1][1] == 0 )
    {
        // We invert the first and add the second
        for( int n = nHits[tck1]-1; n >= 0; n-- )
            newHits.AddHit( hitSets[tck1], n );

        for( int n = 0; n < nHits[tck2]; n++ )
            newHits.AddHit( hitSets[tck2], n );
    }
    else if( tracks[0][1] == 1 && tracks[1][1] == 0 )
    {
        // We add the first and then the second

        for( int n = 0; n < nHits[tck1]; n++ )
            newHits.AddHit( hitSets[tck1], n );

        for( int n = 0; n < nHits[tck2]; n++ )
            newHits.AddHit( hitSets[tck2], n );
    }
    else if( tracks[0][1] == 0 && tracks[1][1] == 1 )
    {
        // We add the second and then the first

        for( int n = 0; n < nHits[tck2]; n++ )
            newHits.AddHit( hitSets[tck2], n );

        for( int n = 0; n < nHits[tck1]; n++ )
            newHits.AddHit( hitSets[tck1], n );
    }
    else
    {
        // We add the first and invert the second

        for( int n = 0; n < nHits[tck1]; n++ )
            newHits.AddHit( hitSets[tck1], n );

        for( int n = nHits[tck2]-1; n >= 0; n-- )
            newHits.AddHit( hitSets[tck2], n );
    }

    hitSets.erase (hitSets.begin()+tck2);
    hitSets.erase (hitSets.begin()+tck1);
    hitSets.push_back( newHits );
    /* }}} */

    nSubTracks = hitSets.size();

    /* {{{ Debug output 
    cout << "New subtracks : " << nSubTracks << endl;

    cout << "AFTER REMOVAL" << endl;
    cout << "--------" << endl;
    for( int i = 0; i < nSubTracks; i++ )
    {
        cout << "Subset : " << i << endl;
        cout << " Sub hits : " << nHits[i] << endl;

        hitSets[i].PrintHits();
    }
    cout << "--------" << endl;
    GetChar();
    }}} */

    if( nSubTracks > 1 ) ReconnectTracks( hitSets );
}

Int_t TRestTrackReconnectionProcess::GetTrackBranches( TRestHits &h, Double_t mean, Double_t sigma )
{
    Int_t breaks = 0;
    Int_t nHits = h.GetNumberOfHits();

    for( int n = 1; n < nHits; n++ )
        if(  h.GetDistance( n-1, n ) > mean + 2*sigma ) 
                breaks++;
    return breaks;
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::EndProcess()
{
}

//______________________________________________________________________________
void TRestTrackReconnectionProcess::InitFromConfigFile( )
{
}

