///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalDeconvolutionProcess.cxx
///
///             oct 2015:  Javier Galan
///_______________________________________________________________________________


#include "TRestSignalDeconvolutionProcess.h"

#include <TRandom.h>

const int deconvolutionDebug = 1;

ClassImp(TRestSignalDeconvolutionProcess)
    //______________________________________________________________________________
TRestSignalDeconvolutionProcess::TRestSignalDeconvolutionProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalDeconvolutionProcess::TRestSignalDeconvolutionProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfig( "signalDeconvolutionProcess", cfgFileName ) == -1 ) LoadDefaultConfig( );

    // TRestSignalDeconvolutionProcess default constructor
}

//______________________________________________________________________________
TRestSignalDeconvolutionProcess::~TRestSignalDeconvolutionProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestSignalDeconvolutionProcess destructor
    if( deconvolutionDebug ) delete fCanvas;
}

void TRestSignalDeconvolutionProcess::LoadDefaultConfig( )
{
    SetName( "signalDeconvolutionProcess-Default" );
    SetTitle( "Default config" );

    fFreq1 = 0.0112219;
    fFreq2 = 0.0112219 * 1.5;
    fSmoothingPoints = 3;
    fSmearingPoints = 5;
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::Initialize()
{

    fInputSignalEvent = new TRestSignalEvent();

    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

    if( deconvolutionDebug ) fCanvas = new TCanvas("RunCanvas","RunCanvas");
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    cout << __PRETTY_FUNCTION__ << endl;

}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::BeginOfEventProcess() 
{
    cout << "Begin of event process" << endl;
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestSignalDeconvolutionProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

    if( fInputSignalEvent->GetNumberOfSignals() == 0 ) return NULL;

    if( deconvolutionDebug && fInputSignalEvent->GetNumberOfSignals() > 0 )
    {
        fCanvas->cd();
        fInputSignalEvent->DrawEvent();
        fCanvas->Update();
        getchar();
    }

    fOutputSignalEvent->SetEventTime( fInputSignalEvent->GetEventTime() );
    fOutputSignalEvent->SetEventID( fInputSignalEvent->GetEventID() );

    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal smoothSignal;
        TRestSignal difSignal;
        TRestSignal dif2Signal;

        // Smoothing input signal
        fInputSignalEvent->GetSignal(n)->GetSignalSmoothed( &smoothSignal, fSmoothingPoints );

        // Obtainning dV/dt
        smoothSignal.GetDifferentialSignal( &difSignal, fSmearingPoints );
        // Obtainning d2V/dt2
        difSignal.GetDifferentialSignal( &dif2Signal, fSmearingPoints );

        // Applying transform -> difSignal
        difSignal.MultiplySignalBy ( 1/fFreq1 );
        dif2Signal.MultiplySignalBy ( 1/fFreq2/fFreq1 );
        difSignal.SignalAddition ( &smoothSignal );
        difSignal.SignalAddition ( &dif2Signal );

        difSignal.SetSignalID( fInputSignalEvent->GetSignal(n)->GetSignalID() );

        fOutputSignalEvent->AddSignal( difSignal );
    }


    if( deconvolutionDebug && fInputSignalEvent->GetNumberOfSignals() > 0 )
    {
        fCanvas->cd();
        fOutputSignalEvent->DrawEvent();
        fCanvas->Update();
        getchar();
    }

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::InitFromConfigFile( )
{
    cout << __PRETTY_FUNCTION__ << " --> TOBE implemented" << endl;

}

