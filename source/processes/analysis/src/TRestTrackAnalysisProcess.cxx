///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestTrackAnalysisProcess.h"
using namespace std;


ClassImp(TRestTrackAnalysisProcess)
    //______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::~TRestTrackAnalysisProcess()
{
    delete fTrackEvent;
}

void TRestTrackAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::Initialize()
{
    SetName( "trackAnalysisProcess" );

    fTrackEvent = new TRestTrackEvent();

    fOutputEvent = fTrackEvent;
    fInputEvent = fTrackEvent;

    fCutsEnabled = false;
}

void TRestTrackAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::BeginOfEventProcess() 
{
    fTrackEvent->Initialize( );
}

//______________________________________________________________________________
TRestEvent* TRestTrackAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    TRestTrackEvent *fInputTrackEvent = (TRestTrackEvent *) evInput;

    /// Copying the signal event to the output event
    
    fTrackEvent->SetID( fInputTrackEvent->GetID() );
    fTrackEvent->SetSubID( fInputTrackEvent->GetSubID() );
    fTrackEvent->SetTimeStamp( fInputTrackEvent->GetTimeStamp() );
    fTrackEvent->SetSubEventTag( fInputTrackEvent->GetSubEventTag() );


    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fTrackEvent->AddTrack( fInputTrackEvent->GetTrack( tck ) );
    /////////////////////////////////////////////////

    TString obsName;

    Double_t tckLenX = 0;
    Double_t tckLenY = 0;
    Int_t nTracksX = 0;
    Int_t nTracksY = 0;
    for( int tck = 0; tck < fTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fTrackEvent->isTopLevel( tck ) ) continue;

        TRestTrack *t = fTrackEvent->GetTrack( tck );

        if( t->isXZ() )
        {
            if( t->GetTrackLength() >= 0 )
            {
                nTracksX++;
                tckLenX += t->GetTrackLength();
            }
        }

        if( t->isYZ() )
        {
            if( t->GetTrackLength() >= 0 )
            {
                nTracksY++;
                tckLenY += t->GetTrackLength();
            }
        }
    }

    Double_t evTimeDelay = 0;
    if( fPreviousEventTime.size() > 0 )
        evTimeDelay = fTrackEvent->GetTime() - fPreviousEventTime.back();
    obsName = this->GetName() + (TString) ".EventTimeDelay";
    fAnalysisTree->SetObservableValue( obsName, evTimeDelay );

    Double_t meanRate = 0;
    if( fPreviousEventTime.size() == 100 )
        meanRate = 100. / (fTrackEvent->GetTime()-fPreviousEventTime.front());

    obsName = this->GetName() + (TString) ".MeanRate_InHz";
    fAnalysisTree->SetObservableValue( obsName, meanRate );

    fTrackEvent->SetNumberOfXTracks( nTracksX );
    fTrackEvent->SetNumberOfYTracks( nTracksY );

    if( nTracksX < fNTracksXCut.X() || nTracksX > fNTracksXCut.Y() ) return NULL;
    if( nTracksY < fNTracksYCut.X() || nTracksY > fNTracksYCut.Y() ) return NULL;

    Double_t x = 0, y = 0;

    TRestTrack *tX = fTrackEvent->GetMaxEnergyTrackInX();
    if( tX != NULL )
	    x = tX->GetMeanPosition().X();

    TRestTrack *tY = fTrackEvent->GetMaxEnergyTrackInY();
    if( tY != NULL )
	    y = tY->GetMeanPosition().Y();

    obsName = this->GetName() + (TString) ".xMean";
    fAnalysisTree->SetObservableValue( obsName, x );

    obsName = this->GetName() + (TString) ".yMean";
    fAnalysisTree->SetObservableValue( obsName, y );

    obsName = this->GetName() + (TString) ".LengthX";
    fAnalysisTree->SetObservableValue( obsName, tckLenX );

    obsName = this->GetName() + (TString) ".LengthY";
    fAnalysisTree->SetObservableValue( obsName, tckLenY );

    obsName = this->GetName() + (TString) ".nTracksX";
    fAnalysisTree->SetObservableValue( obsName, nTracksX );

    obsName = this->GetName() + (TString) ".nTracksY";
    fAnalysisTree->SetObservableValue( obsName, nTracksY );

    return fTrackEvent;
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndOfEventProcess() 
{
    fPreviousEventTime.push_back( fTrackEvent->GetTimeStamp() );
    if( fPreviousEventTime.size() > 100 ) fPreviousEventTime.erase( fPreviousEventTime.begin() );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitFromConfigFile( )
{
    fNTracksXCut = StringTo2DVector( GetParameter( "nTracksXCut", "(1,10)") );
    fNTracksYCut = StringTo2DVector( GetParameter( "nTracksYCut", "(1,10)") );

    if( GetParameter( "cutsEnabled" ) == "true" ) fCutsEnabled = true;
}

