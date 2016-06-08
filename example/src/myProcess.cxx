///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myProcess.cxx
///
///_______________________________________________________________________________


#include "myProcess.h"
using namespace std;

ClassImp(myProcess)
    //______________________________________________________________________________
myProcess::myProcess()
{
    Initialize();
}

//______________________________________________________________________________
/*
myProcess::myProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
}
*/

//______________________________________________________________________________
myProcess::~myProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
}



void myProcess::LoadDefaultConfig( )
{
    SetName( "myProcess-Default" );
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void myProcess::Initialize()
{
    SetName( "myProcess" );

    fInputSignalEvent = new TRestSignalEvent();
    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

}

void myProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}


//______________________________________________________________________________
TRestEvent* myProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

    if( fInputSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;

    if( fInputSignalEvent->GetID() % fMyProcessParameter == 0 ) 
        cout << "My Process. Event ID : " << fInputSignalEvent->GetID() << endl;

    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal *outSignal = fInputSignalEvent->GetSignal( n );

        // Do something with output signal signal

        fOutputSignalEvent->AddSignal( *outSignal );
    }

    return fOutputSignalEvent;
}

void myProcess::InitFromConfigFile( )
{
    fMyProcessParameter = StringToInteger( GetParameter( "showEvery" ) );
}

