///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAddSignalNoiseProcess.cxx
///
///             February 2016: Javier Gracia
///_______________________________________________________________________________


#include "TRestAddSignalNoiseProcess.h"
using namespace std;

#include <TRestFFT.h>

#include <TFile.h>


ClassImp(TRestAddSignalNoiseProcess)
    //______________________________________________________________________________
TRestAddSignalNoiseProcess::TRestAddSignalNoiseProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestAddSignalNoiseProcess::TRestAddSignalNoiseProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();
    // TRestAddSignalNoiseProcess default constructor
}

//______________________________________________________________________________
TRestAddSignalNoiseProcess::~TRestAddSignalNoiseProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestAddSignalNoiseProcess destructor

}



void TRestAddSignalNoiseProcess::LoadDefaultConfig( )
{
    SetName( "addSignalNoiseProcess-Default" );
    SetTitle( "Default config" );

    fNoiseLevel = 1;
}

//______________________________________________________________________________
void TRestAddSignalNoiseProcess::Initialize()
{
    SetName( "addSignalNoiseProcess" );

    fNoiseLevel = 1;

    fInputSignalEvent = new TRestSignalEvent();
    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

}

void TRestAddSignalNoiseProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestAddSignalNoiseProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
void TRestAddSignalNoiseProcess::BeginOfEventProcess() 
{
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestAddSignalNoiseProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

	//cout<<"Number of signals "<< fInputSignalEvent->GetNumberOfSignals()<< endl;

    if( fInputSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;


    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal noiseSignal;

        // Asign ID and add noise    
        fInputSignalEvent->GetSignal(n)->GetWhiteNoiseSignal( &noiseSignal, fNoiseLevel );
        noiseSignal.SetSignalID( fInputSignalEvent->GetSignal(n)->GetSignalID() );

        fOutputSignalEvent->AddSignal( noiseSignal );
    }

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestAddSignalNoiseProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestAddSignalNoiseProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestAddSignalNoiseProcess::InitFromConfigFile( )
{
    fNoiseLevel = StringToDouble( GetParameter( "noiseLevel" ) );
}

