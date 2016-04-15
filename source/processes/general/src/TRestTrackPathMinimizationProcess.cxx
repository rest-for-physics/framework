///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackPathMinimizationProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
//
///_______________________________________________________________________________

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

    fEnergyThreshold = 0.5;
}

//______________________________________________________________________________
void TRestTrackPathMinimizationProcess::Initialize( )
{
    SetName("trackPathMinimizationProcess");

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fOutputEvent = fOutputTrackEvent;
    fInputEvent  = fInputTrackEvent;
}

void TRestTrackPathMinimizationProcess::LoadConfig( string cfgFilename )
{

    if( LoadConfigFromFile( cfgFilename ) == -1 ) LoadDefaultConfig( );

    if( fEnergyThreshold <= 0 || fEnergyThreshold >= 1 )
    {
        cout << "REST WARNING : Energy threshold should be a value between 0 and 1." << endl;
        fEnergyThreshold = 0.5;
        cout << "Setting energy threshold to : " << fEnergyThreshold << endl;
    }

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
    cout << __PRETTY_FUNCTION__ << endl;
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

    /* Debug output  */
    cout << "Number of tracks : " << fInputTrackEvent->GetNumberOfTracks() << endl;
    cout << "*****************************" << endl;

    // Copying the input tracks to the output track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 

    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fInputTrackEvent->isTopLevel( tck ) ) continue;
        Int_t tckId = fInputTrackEvent->GetTrack( tck )->GetTrackID();

        TRestVolumeHits *hits = fInputTrackEvent->GetTrack(tck)->GetVolumeHits();
        Int_t nHits = hits->GetNumberOfHits();
        hits->SortByEnergy();

        /* {{{ Debug output
        cout << "Input hits" << endl;
        Int_t pId = fInputTrackEvent->GetTrack( tck )->GetParentID();
        cout << "Track : " << tck << " TrackID : " << tckId << " ParentID : " << pId << endl;
        cout << "-----------------" << endl;
        hits->PrintHits();
        cout << "-----------------" << endl;
        getchar();
        }}} */

        Double_t energy = 0.;
        Int_t n = 0;
        while( n < nHits && energy < fEnergyThreshold * hits->GetTotalEnergy() )
        {
            energy += hits->GetEnergy(n);
            n++;
        }
        n++;

        cout << "Number of nodes that contain the " << fEnergyThreshold * 100 << "\% of the energy : " << n << endl;

        if( n < 3 ) n = 2;
        if( n > nHits-1 ) n = nHits-1;

        // Swaping hit
        if( n > 1 )
        {
            Double_t maxDistance = hits->GetDistance2( 0, 1 );
            Int_t hitToSwap = 1;
            for( int i = 2; i <= n; i++ )
            {
                if( maxDistance < hits->GetDistance2( 0, i ) )
                {
                    maxDistance = hits->GetDistance2( 0, i );
                    hitToSwap = i;
                }
            }

            hits->SwapHits( hitToSwap, nHits-1 );
        }

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

        Int_t rval = 0;
        if( nHits > 3 )
        {
            if( rval == 0 && x[0] == 0 )
            {
                cout << "Minimizing track in Y" << endl;
                rval = TrackMinimization_2D( yInt, zInt, nHits, bestPath );
            }

            if( rval == 0 && y[0] == 0 )
            {
                cout << "Minimizing track in X" << endl;
                rval = TrackMinimization_2D( xInt, zInt, nHits, bestPath );
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

        /* Debug output 
        cout << "Output hits" << endl;
        cout << "-----------------" << endl;
        bestTrack.GetVolumeHits()->PrintHits();
        cout << "-----------------" << endl;
        getchar();
        */

        fOutputTrackEvent->AddTrack( &bestTrack );
    }

 //   fOutputTrackEvent->SetLevels();

    /* Debug output 
    fOutputTrackEvent->PrintOnlyTracks();
    fOutputTrackEvent->PrintEvent();
    getchar();
    */

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
    fEnergyThreshold = StringToDouble( GetParameter( "energyThreshold" ) );
    fMaxNodes = StringToDouble( GetParameter( "maxNodes" ) );
}

