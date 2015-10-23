///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToSignalProcess.cxx
///
///             oct 2015:  Javier Galan
///_______________________________________________________________________________


#include "TRestHitsToSignalProcess.h"

#include <TRandom.h>

ClassImp(TRestHitsToSignalProcess)
//______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess()
{
    Initialize();

}

//______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfig( "hitsToSignalProcess", cfgFileName ) == -1 ) LoadDefaultConfig( );

    fReadout = new TRestReadout( cfgFileName );

   // TRestHitsToSignalProcess default constructor
}

//______________________________________________________________________________
TRestHitsToSignalProcess::~TRestHitsToSignalProcess()
{
    if( fReadout != NULL ) delete fReadout;

    delete fHitsEvent;
    delete fSignalEvent;
   // TRestHitsToSignalProcess destructor
}

void TRestHitsToSignalProcess::LoadDefaultConfig( )
{
    SetName( "hitsToSignalProcess" );
    SetTitle( "Default config" );


    fSampling = 1;

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::Initialize()
{

    fHitsEvent = new TRestHitsEvent();

    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fHitsEvent;
    fOutputEvent = fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    if( fReadout == NULL ) cout << "REST ERRORRRR : Readout has not been initialized" << endl;

    cout << __PRETTY_FUNCTION__ << endl;

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::BeginOfEventProcess() 
{
    cout << "Begin of event process" << endl;
    fSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestHitsToSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    fHitsEvent = (TRestHitsEvent *) evInput;

    cout << "Event ID : " << fHitsEvent->GetEventID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;

    for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ )
    {
        cout << " X : " << fHitsEvent->GetX( hit ) << endl;
        cout << " Y : " << fHitsEvent->GetY( hit ) << endl;
        cout << " Z : " << fHitsEvent->GetZ( hit ) << endl;
        cout << " E : " << fHitsEvent->GetEnergy( hit ) << endl;

    }

    /*
    TRandom *rnd = new TRandom();

    TRestG4Event *g4Event = (TRestG4Event *) evInput;

    Double_t ionizationThreshold = fGas->GetIonizationPotential();
    Double_t longlDiffCoeff = fGas->GetLongitudinalDiffusion( fElectricField );
    Double_t transDiffCoeff = fGas->GetTransversalDiffusion( fElectricField );

    // Get info from G4Event and process
    for( int trk = 0; trk < g4Event->GetNumberOfTracks(); trk++ )
    {
        Int_t nHits = g4Event->GetTrack(trk)->GetNumberOfHits();
        if ( nHits > 0 )
        {
            for( int n = 0; n < nHits; n++ )
            {
                TRestHits *hits = g4Event->GetTrack(trk)->GetHits();

                Double_t eDep = hits->GetEnergy(n);

                if( eDep > 0 )
                {
                    const Double_t x = hits->GetX(n);
                    const Double_t y = hits->GetY(n);
                    const Double_t z = hits->GetZ(n);

                    if ( z > fMinPosition && z < fMaxPosition )
                    {
                        Double_t xDiff, yDiff, zDiff;

                        Int_t numberOfElectrons =  rnd->Poisson( eDep*1000./ionizationThreshold );
                        while( numberOfElectrons > 0 )
                        {
                            numberOfElectrons--;

                            Double_t driftDistance = z - fCathodePosition; 
                            if( driftDistance < 0 ) driftDistance = -driftDistance;

                            Double_t longHitDiffusion = TMath::Sqrt( driftDistance ) * longlDiffCoeff;

                            Double_t transHitDiffusion = TMath::Sqrt( driftDistance ) * transDiffCoeff;

                            xDiff = x + rnd->Gaus( 0, transHitDiffusion );

                            yDiff = y + rnd->Gaus( 0, transHitDiffusion );

                            zDiff = z + rnd->Gaus( 0, longHitDiffusion );

                            fHitsEvent->AddHit( xDiff, yDiff, zDiff, 1. );
                        }

                    }
                }
            }

        }


    }


    if( fHitsEvent->GetNumberOfHits() == 0 ) return NULL;

    cout << "Event : " << g4Event->GetEventID() << " Tracks : " << g4Event->GetNumberOfTracks() << " electrons : " << fHitsEvent->GetNumberOfHits() << endl;

    */
    return fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndProcess()
{
   // Function to be executed once at the end of the process 
   // (after all events have been processed)

   //Start by calling the EndProcess function of the abstract class. 
   //Comment this if you don't want it.
   //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitFromConfigFile( )
{
    /*
    fCathodePosition = StringToDouble( GetParameter( "cathodePosition" ) );
    fAnodePosition = StringToDouble( GetParameter( "anodePosition" ) );
    fElectricField = StringToDouble( GetParameter( "electricField" ) );

    cout << " cathode : " << fCathodePosition << endl;
    cout << " anode : " << fAnodePosition << endl;
    cout << " eField : " << fElectricField << endl;


    if( fCathodePosition > fAnodePosition ) { fMaxPosition = fCathodePosition; fMinPosition = fAnodePosition; }
    else { fMinPosition = fCathodePosition; fMaxPosition = fAnodePosition; } 
    */

}
