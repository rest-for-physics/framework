///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalToSignalProcess.cxx
///
///             February 2016: Javier Gracia
///_______________________________________________________________________________


#include "TRestRawSignalToSignalProcess.h"
using namespace std;


ClassImp(TRestRawSignalToSignalProcess)
    //______________________________________________________________________________
TRestRawSignalToSignalProcess::TRestRawSignalToSignalProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestRawSignalToSignalProcess::TRestRawSignalToSignalProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();
    // TRestRawSignalToSignalProcess default constructor
}

//______________________________________________________________________________
TRestRawSignalToSignalProcess::~TRestRawSignalToSignalProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestRawSignalToSignalProcess destructor
}

void TRestRawSignalToSignalProcess::LoadDefaultConfig( )
{
    SetName( "rawSignalToSignal-Default" );
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestRawSignalToSignalProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fInputSignalEvent = new TRestRawSignalEvent();
    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;
}

void TRestRawSignalToSignalProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestRawSignalToSignalProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

}

//______________________________________________________________________________
void TRestRawSignalToSignalProcess::BeginOfEventProcess() 
{
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalToSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputSignalEvent = (TRestRawSignalEvent *) evInput;

    /// Copying the signal event to the output event

    fOutputSignalEvent->SetID( fInputSignalEvent->GetID() );
    fOutputSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    fOutputSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    fOutputSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ )
    {
	TRestSignal sgnl;
	sgnl.Initialize();
	TRestRawSignal *rawSgnl = fInputSignalEvent->GetSignal( n );
	sgnl.SetID( rawSgnl->GetID() );
	for( int p = 0; p < rawSgnl->GetNumberOfPoints(); p++ )
		sgnl.NewPoint( p, rawSgnl->GetData(p) );

	fOutputSignalEvent->AddSignal( sgnl );
    }
    /////////////////////////////////////////////////

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalToSignalProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestRawSignalToSignalProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestRawSignalToSignalProcess::InitFromConfigFile( )
{
}

