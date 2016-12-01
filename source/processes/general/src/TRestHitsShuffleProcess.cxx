//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsShuffleProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestHitsShuffleProcess.h"
using namespace std;

ClassImp(TRestHitsShuffleProcess)
//______________________________________________________________________________
TRestHitsShuffleProcess::TRestHitsShuffleProcess( )
{
    Initialize();
}

//______________________________________________________________________________
TRestHitsShuffleProcess::TRestHitsShuffleProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestHitsShuffleProcess::~TRestHitsShuffleProcess( )
{
    delete fInputHitsEvent;
    delete fOutputHitsEvent;
}

void TRestHitsShuffleProcess::LoadDefaultConfig( )
{
    SetName( "hitsShuffleProcess" );
    SetTitle( "Default config" );

    fIterations = 100;
}

//______________________________________________________________________________
void TRestHitsShuffleProcess::Initialize( )
{
    SetName("hitsShuffleProcess");

    fInputHitsEvent = new TRestHitsEvent();
    fOutputHitsEvent = new TRestHitsEvent();

    fOutputEvent = fOutputHitsEvent;
    fInputEvent  = fInputHitsEvent;

    fRandom = new TRandom3(0);
}

void TRestHitsShuffleProcess::LoadConfig( std::string cfgFilename, std::string name )
{

    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestHitsShuffleProcess::InitProcess()
{
    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestHitsShuffleProcess::BeginOfEventProcess() 
{
    fOutputHitsEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestHitsShuffleProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputHitsEvent = (TRestHitsEvent *) evInput;

    // Copying the input hits event to the output hits event
    for( int h = 0; h < fInputHitsEvent->GetNumberOfHits(); h++ )
    {
        Double_t x = fInputHitsEvent->GetX( h );
        Double_t y = fInputHitsEvent->GetY( h );
        Double_t z = fInputHitsEvent->GetZ( h );
        Double_t en = fInputHitsEvent->GetEnergy( h );
        fOutputHitsEvent->AddHit( x, y, z, en );
    }

    TRestHits *hits = fOutputHitsEvent->GetHits();

    Int_t nHits = hits->GetNumberOfHits();

    for( int n = 0; n < fIterations; n++ )
    {
        Int_t hit1 = (Int_t) (nHits * fRandom->Uniform(0,1) );
        Int_t hit2 = (Int_t) (nHits * fRandom->Uniform(0,1) );

        hits->SwapHits( hit1, hit2 );
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestHitsShuffleProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestHitsShuffleProcess::EndProcess()
{
}

//______________________________________________________________________________
void TRestHitsShuffleProcess::InitFromConfigFile( )
{
    fIterations = StringToInteger( GetParameter(  "iterations" ) );
}

