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

    fOutputTrackEvent->SetEventID( fInputTrackEvent->GetEventID() );
    fOutputTrackEvent->SetEventTime( fInputTrackEvent->GetEventTime() );

    /* Debug output  */
    cout << "Event ID : " << fInputTrackEvent->GetEventID() << endl;
    cout << "Number of tracks : " << fInputTrackEvent->GetNumberOfTracks() << endl;
    cout << "*****************************" << endl;

    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
        /* Debug output 
        cout << "=======" << endl;
        */
        /*
        fInputTrackEvent->GetTrack(tck)->GetVolumeHits()->PrintHits();
        */

        TRestVolumeHits *hits = fInputTrackEvent->GetTrack(tck)->GetVolumeHits();
        Int_t nHits = hits->GetNumberOfHits();
        hits->SortByEnergy();

        cout << "Track " << tck << " hits : " << hits->GetNumberOfHits( ) << endl;

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
                if( maxDistance < hits->GetDistance2( 0, n ) )
                {
                    maxDistance = hits->GetDistance2( 0, n );
                    hitToSwap = i;
                }

            hits->SwapHits( hitToSwap, nHits-1 );
        }

 //       fInputTrackEvent->GetTrack(tck)->GetVolumeHits()->PrintHits();

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

        if( nHits > 3 )
        {
            if( x[0] == 0 )
            {
                cout << "Minimizing track in Y" << endl;
                TrackMinimization_2D( yInt, zInt, nHits, bestPath );
            }

            if( y[0] == 0 )
            {
                cout << "Minimizing track in X" << endl;
                TrackMinimization_2D( xInt, zInt, nHits, bestPath );
            }
        }

        TRestVolumeHits bestHitsOrder;

        for( int i = 0; i < nHits; i++ )
        {
            Int_t n = bestPath[i];

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
    fEnergyThreshold = StringToDouble( GetParameter( "energyThreshold" ) );
    fMaxNodes = StringToDouble( GetParameter( "maxNodes" ) );
}

