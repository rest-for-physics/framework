///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestElectronDiffusionProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestElectronDiffusionProcess
///             How to use: replace TRestElectronDiffusionProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestElectronDiffusionProcess.h"
using namespace std;

#include <TRandom.h>

ClassImp(TRestElectronDiffusionProcess)
//______________________________________________________________________________
TRestElectronDiffusionProcess::TRestElectronDiffusionProcess()
{
    Initialize();

}

//______________________________________________________________________________
TRestElectronDiffusionProcess::TRestElectronDiffusionProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );

    PrintMetadata();
    fGas = new TRestGas( cfgFileName );

   // TRestElectronDiffusionProcess default constructor
}

//______________________________________________________________________________
TRestElectronDiffusionProcess::~TRestElectronDiffusionProcess()
{
    if( fGas != NULL ) delete fGas;

    delete fHitsEvent;
    delete fG4Event;
   // TRestElectronDiffusionProcess destructor
}

void TRestElectronDiffusionProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );

    fCathodePosition = -1000;
    fAnodePosition = 0;
    fElectricField = 1000;
    fAttachment = 0;

    // TOBE implemented
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::Initialize()
{
    SetName( "electronDiffusionProcess" );

    fCathodePosition = 0;
    fAnodePosition = 0;
    fElectricField = 0;
    fAttachment = 0;

    fHitsEvent = new TRestHitsEvent();

    fG4Event = new TRestG4Event();

    fOutputEvent = fHitsEvent;
    fInputEvent = fG4Event;
}

void TRestElectronDiffusionProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) ) LoadDefaultConfig( );

    PrintMetadata();
    fGas = new TRestGas( cfgFilename.c_str() );
    fGas->PrintMetadata( );
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    if( fGas == NULL ) cout << "REST ERRORRRR : Gas has not been initialized" << endl;

    cout << __PRETTY_FUNCTION__ << endl;

}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::BeginOfEventProcess() 
{
    cout << "Begin of event process" << endl;
    fHitsEvent->Initialize(); 

    fOutputEvent->SetEventID( fInputEvent->GetEventID() );
}

//______________________________________________________________________________
TRestEvent* TRestElectronDiffusionProcess::ProcessEvent( TRestEvent *evInput )
{

    TRandom *rnd = new TRandom();

    TRestG4Event *g4Event = (TRestG4Event *) evInput;

    Double_t ionizationThreshold = fGas->GetIonizationPotential();
    Double_t longlDiffCoeff = fGas->GetLongitudinalDiffusion( fElectricField ); // (cm)^1/2
    Double_t transDiffCoeff = fGas->GetTransversalDiffusion( fElectricField ); // (cm)^1/2

    Int_t isAttached;

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

                            Double_t longHitDiffusion = 10. * TMath::Sqrt( driftDistance/10. ) * longlDiffCoeff; //mm

                            Double_t transHitDiffusion = 10. * TMath::Sqrt( driftDistance/10. ) * transDiffCoeff; //mm

			 if (fAttachment)
			 	isAttached =  (rnd->Uniform(0,1) > pow(1-fAttachment, driftDistance/10. ) );
			 else
				isAttached = 0;

			 if ( isAttached == 0)
			 {
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


    }


    if( fHitsEvent->GetNumberOfHits() == 0 ) return NULL;

    cout << "Event : " << g4Event->GetEventID() << " Tracks : " << g4Event->GetNumberOfTracks() << " electrons : " << fHitsEvent->GetNumberOfHits() << endl;

    return fHitsEvent;
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::EndProcess()
{
   // Function to be executed once at the end of the process 
   // (after all events have been processed)

   //Start by calling the EndProcess function of the abstract class. 
   //Comment this if you don't want it.
   //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::InitFromConfigFile( )
{
    fCathodePosition = StringToDouble( GetParameter( "cathodePosition" ) );
    fAnodePosition = StringToDouble( GetParameter( "anodePosition" ) );
    fElectricField = StringToDouble( GetParameter( "electricField" ) );
    fAttachment = StringToDouble( GetParameter( "attachment" ) );

    if( fCathodePosition > fAnodePosition ) { fMaxPosition = fCathodePosition; fMinPosition = fAnodePosition; }
    else { fMinPosition = fCathodePosition; fMaxPosition = fAnodePosition; } 

}
