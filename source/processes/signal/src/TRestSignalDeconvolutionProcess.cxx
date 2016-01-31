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
using namespace std;

#include <TRestFFT.h>

#include <TFile.h>

TRestFFT *originalFFT;
TRestFFT *cleanFFT;

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

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();
    // TRestSignalDeconvolutionProcess default constructor
}

//______________________________________________________________________________
TRestSignalDeconvolutionProcess::~TRestSignalDeconvolutionProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestSignalDeconvolutionProcess destructor

    delete cleanFFT;
    delete originalFFT;
}

void TRestSignalDeconvolutionProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) ) LoadDefaultConfig( );

    PrintMetadata();
}

void TRestSignalDeconvolutionProcess::LoadDefaultConfig( )
{
    SetName( "signalDeconvolutionProcess-Default" );
    SetTitle( "Default config" );

    fFreq1 = 11.1063e-2/M_PI/1.5;
    fFreq2 = 14.8001e-2/M_PI/1.5;

    fSmoothingPoints = 3;
    fSmearingPoints = 5;

    fBaseLineStart = 32;
    fBaseLineEnd = 64;

    fFFTStart = 32;
    fFFTEnd = 32;

    fCutFrequency = 12;

    fResponseFilename = "AGET_Response_100MHz_Gain0x2_Shaping0x7.root";
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::Initialize()
{
    SetName( "signalDeconvolutionProcess" );
    fInputSignalEvent = new TRestSignalEvent();

    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

    originalFFT = new TRestFFT();
    cleanFFT = new TRestFFT();
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    TRestSignal *responseSignal = new TRestSignal();

    TString fullPathName = (TString) getenv("REST_PATH") + "/inputData/signal/" + fResponseFilename;

    TFile *f = new TFile( fullPathName );

    responseSignal = (TRestSignal *) f->Get( "signal Response" );

    f->Close();

    TRestSignal smoothSignal, difSignal, dif2Signal;

    // Smoothing response signal
    responseSignal->GetSignalSmoothed( &smoothSignal, fSmoothingPoints );

    // Obtainning dV/dt
    smoothSignal.GetDifferentialSignal( &difSignal, fSmearingPoints );
    // Obtainning d2V/dt2
    difSignal.GetDifferentialSignal( &dif2Signal, fSmearingPoints );

    // Applying transform -> difSignal
    difSignal.MultiplySignalBy ( 1/fFreq1 );
    dif2Signal.MultiplySignalBy ( 1/fFreq2/fFreq1 );
    difSignal.SignalAddition ( &smoothSignal );
    difSignal.SignalAddition ( &dif2Signal );

    originalFFT->ForwardSignalFFT( &difSignal, fFFTStart, fFFTEnd );

    // Exponential convolution for signal tail-cleaning
    Double_t baseline = difSignal.GetAverage( fBaseLineStart, fBaseLineEnd );
    Double_t maxIndex = difSignal.GetMaxIndex();
    Double_t width = difSignal.GetMaxPeakWidth();
    difSignal.ExponentialConvolution( maxIndex+width/2., width, baseline );

    cleanFFT->ForwardSignalFFT( &difSignal, fFFTStart, fFFTEnd );
    cleanFFT->DivideBy( originalFFT );

    delete responseSignal;
}

//______________________________________________________________________________
void TRestSignalDeconvolutionProcess::BeginOfEventProcess() 
{
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestSignalDeconvolutionProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

    if( fInputSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;

    fOutputSignalEvent->SetEventTime( fInputSignalEvent->GetEventTime() );
    fOutputSignalEvent->SetEventID( fInputSignalEvent->GetEventID() );

    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal smoothSignal, difSignal, dif2Signal;

        // Smoothing input signal
        fInputSignalEvent->GetSignal(n)->GetSignalSmoothed( &smoothSignal, fSmoothingPoints );
        difSignal.SetSignalID( fInputSignalEvent->GetSignal(n)->GetSignalID() );

        // Obtainning dV(t)/dt
        smoothSignal.GetDifferentialSignal( &difSignal, fSmearingPoints );
        // Obtainning d2V(t)/dt2
        difSignal.GetDifferentialSignal( &dif2Signal, fSmearingPoints );

        // Applying second order deconvolution -> difSignal
        difSignal.MultiplySignalBy ( 1/fFreq1 );
        dif2Signal.MultiplySignalBy ( 1/fFreq2/fFreq1 );
        difSignal.SignalAddition ( &smoothSignal );
        difSignal.SignalAddition ( &dif2Signal );

        // Applying response
        TRestFFT *mySignalFFT = new TRestFFT();
        mySignalFFT->ForwardSignalFFT( &difSignal, fFFTStart, fFFTEnd );

        mySignalFFT->MultiplyBy( cleanFFT, 0, fCutFrequency );

        // Removing baseline
        mySignalFFT->RemoveBaseline( );

        // Recovering time signal
        mySignalFFT->BackwardFFT();
        mySignalFFT->GetSignal( &difSignal );

        fOutputSignalEvent->AddSignal( difSignal );

        delete mySignalFFT;
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
    fFreq1 = StringToDouble( GetParameter( "frequency1" ) );
    fFreq2 = StringToDouble( GetParameter( "frequency2" ) );

    fSmoothingPoints = StringToInteger( GetParameter( "smoothingPoints" ) );
    fSmearingPoints = StringToInteger( GetParameter( "smearingPoints" ) );

    fBaseLineStart = StringToInteger( GetParameter( "baselineStart" ) );
    fBaseLineEnd = StringToInteger( GetParameter( "baselineEnd" ) );

    fFFTStart = StringToInteger( GetParameter( "nFFTStart" ) );
    fFFTEnd = StringToInteger( GetParameter( "nFFTEnd" ) );

    fCutFrequency = StringToInteger( GetParameter( "cutFrequency" ) );

    fResponseFilename = GetParameter( "responseFile" );
}

