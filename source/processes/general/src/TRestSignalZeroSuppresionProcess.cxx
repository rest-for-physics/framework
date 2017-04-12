///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalZeroSuppresionProcess.cxx
///
///             jan 2016:  Javier Galan
///_______________________________________________________________________________

#include <TRestDetectorSetup.h>

#include "TRestSignalZeroSuppresionProcess.h"
using namespace std;

const double cmTomm = 10.;

ClassImp(TRestSignalZeroSuppresionProcess)
    //______________________________________________________________________________
TRestSignalZeroSuppresionProcess::TRestSignalZeroSuppresionProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalZeroSuppresionProcess::TRestSignalZeroSuppresionProcess( char *cfgFileName )
{
    Initialize();

    LoadConfig( cfgFileName );

    // TRestSignalZeroSuppresionProcess default constructor
}

//______________________________________________________________________________
TRestSignalZeroSuppresionProcess::~TRestSignalZeroSuppresionProcess()
{
    delete fRawSignalEvent;
    delete fSignalEvent;
    // TRestSignalZeroSuppresionProcess destructor
}

void TRestSignalZeroSuppresionProcess::LoadDefaultConfig( )
{
    SetName( "signalZeroSuppresionProcess-Default" );
    SetTitle( "Default config" );

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    TVector2 fBaseLineRange = TVector2( 10, 90);
    fPointThreshold = 2.;
    fSignalThreshold = 2.;
    fNPointsOverThreshold = 10;

}

void TRestSignalZeroSuppresionProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );

}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fRawSignalEvent = new TRestRawSignalEvent();
    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fRawSignalEvent;
    fOutputEvent = fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::BeginOfEventProcess() 
{
    fSignalEvent->Initialize(); 
}


//______________________________________________________________________________
TRestEvent* TRestSignalZeroSuppresionProcess::ProcessEvent( TRestEvent *evInput )
{
    fRawSignalEvent = (TRestRawSignalEvent *) evInput;

    if( GetVerboseLevel() >= REST_Debug )
        fSignalEvent->PrintEvent();

    fSignalEvent->SetID( fRawSignalEvent->GetID() );
    fSignalEvent->SetSubID( fRawSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fRawSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fRawSignalEvent->GetSubEventTag() );

    Int_t numberOfSignals = fRawSignalEvent->GetNumberOfSignals();

    Double_t totalIntegral = 0;
    for( int i = 0; i < numberOfSignals; i++ )
    {
        TRestRawSignal *sgnl = fRawSignalEvent->GetSignal( i );
        Int_t signalID = sgnl->GetSignalID();

        Double_t integral = sgnl->GetIntegralWithThreshold( 0, sgnl->GetNumberOfPoints(),
                fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold );

        if( integral > 0 )
        {
            totalIntegral += integral;

            vector <Int_t> poinsOver = sgnl->GetPointsOverThreshold();

            TRestSignal outSignal;
            outSignal.SetID( signalID );
            for( unsigned int n = 0; n < poinsOver.size(); n++ )
                outSignal.NewPoint( poinsOver[n], sgnl->GetData( poinsOver[n] ) );

            fSignalEvent->AddSignal( outSignal );
        }
    }

    if( this->GetVerboseLevel() >= REST_Debug ) 
    {
        fSignalEvent->PrintEvent();
        cout << "TRestSignalZeroSuppresionProcess. Signals added : " << fSignalEvent->GetNumberOfSignals() << endl;
        cout << "TRestSignalZeroSuppresionProcess. Threshold integral : " << totalIntegral << endl;
        GetChar();
    }

    if( fSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalZeroSuppresionProcess::InitFromConfigFile( )
{
    fBaseLineRange = StringTo2DVector( GetParameter( "baseLineRange", "(5,55)") );
    fPointThreshold = StringToDouble( GetParameter( "pointThreshold", 2 ) );
    fNPointsOverThreshold = StringToInteger( GetParameter( "pointsOverThreshold", 5 ) );
    fSignalThreshold = StringToDouble( GetParameter( "signalThreshold", 5 ) );
}

