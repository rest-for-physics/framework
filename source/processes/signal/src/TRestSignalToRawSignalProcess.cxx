///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalToRawSignalProcess.cxx
///
///             February 2016: Javier Gracia
///_______________________________________________________________________________


#include "TRestSignalToRawSignalProcess.h"
using namespace std;

#include <TRestFFT.h>

#include <TFile.h>


ClassImp(TRestSignalToRawSignalProcess)
    //______________________________________________________________________________
TRestSignalToRawSignalProcess::TRestSignalToRawSignalProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalToRawSignalProcess::TRestSignalToRawSignalProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();
    // TRestSignalToRawSignalProcess default constructor
}

//______________________________________________________________________________
TRestSignalToRawSignalProcess::~TRestSignalToRawSignalProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestSignalToRawSignalProcess destructor

}



void TRestSignalToRawSignalProcess::LoadDefaultConfig( )
{
    SetName( "addSignalNoiseProcess-Default" );
    SetTitle( "Default config" );

}

//______________________________________________________________________________
void TRestSignalToRawSignalProcess::Initialize()
{
    SetName( "signalToRawSignalProcess" );

    fInputSignalEvent = new TRestSignalEvent();
    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

}

void TRestSignalToRawSignalProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestSignalToRawSignalProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestSignalToRawSignalProcess::BeginOfEventProcess() 
{
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestSignalToRawSignalProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

	//cout << "Number of signals " << fInputSignalEvent->GetNumberOfSignals() << endl;

    if( fInputSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;

    Int_t dominantSignal = -1;
    Double_t maxPeak = 0;
    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        if ( fInputSignalEvent->GetSignal(n)->GetMaxPeakValue() > maxPeak )
        {
            maxPeak = fInputSignalEvent->GetSignal(n)->GetMaxPeakValue();
            dominantSignal = n;
        }
    }

    Int_t startTime = 0;


    TRestSignal *sgnl = fInputSignalEvent->GetSignal( dominantSignal );
    maxPeak /= 20.;
    Int_t continueLoop = 1;
    for( int n = 0; n < sgnl->GetNumberOfPoints() && continueLoop; n++ )
    {
        if( sgnl->GetData(n) > maxPeak )
        {
            startTime = sgnl->GetTime( n );
            continueLoop = 0;
        }
    }


    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal rawSignal;
        rawSignal.Initialize();
        for( int i = 0; i < fNBins; i++ )
            rawSignal.NewPoint( i, 0 );

        rawSignal.SetSignalID( fInputSignalEvent->GetSignal(n)->GetSignalID() );

        TRestSignal *iSgl = fInputSignalEvent->GetSignal(n);
        for( int i = 0; i < iSgl->GetNumberOfPoints(); i++ )
        {
            Double_t timeShifted = iSgl->GetTime(i) + fTriggerTime - startTime;
            if( timeShifted >= 0 && timeShifted < fNBins )
                rawSignal.IncreaseTimeBinBy( timeShifted, iSgl->GetData( i ) );
        }

        fOutputSignalEvent->AddSignal( rawSignal );
    }

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestSignalToRawSignalProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalToRawSignalProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalToRawSignalProcess::InitFromConfigFile( )
{
    fTriggerTime = StringToInteger( GetParameter( "triggerTime" ) );
    fNBins = StringToInteger( GetParameter( "nBins" ) );
}

