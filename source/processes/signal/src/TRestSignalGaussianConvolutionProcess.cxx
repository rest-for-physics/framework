///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalGaussianConvolutionProcess.cxx
///
///             February 2016: Javier Gracia
///_______________________________________________________________________________


#include "TRestSignalGaussianConvolutionProcess.h"
using namespace std;

#include <TFile.h>


ClassImp(TRestSignalGaussianConvolutionProcess)
    //______________________________________________________________________________
TRestSignalGaussianConvolutionProcess::TRestSignalGaussianConvolutionProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalGaussianConvolutionProcess::TRestSignalGaussianConvolutionProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();
    // TRestSignalGaussianConvolutionProcess default constructor
}

//______________________________________________________________________________
TRestSignalGaussianConvolutionProcess::~TRestSignalGaussianConvolutionProcess()
{
    delete fOutputSignalEvent;
    delete fInputSignalEvent;
    // TRestSignalGaussianConvolutionProcess destructor

}



void TRestSignalGaussianConvolutionProcess::LoadDefaultConfig( )
{
    SetName( "gaussianConvolutionProcess-Default" );
    SetTitle( "Default config" );

    fSigma  = 100.;
}

//______________________________________________________________________________
void TRestSignalGaussianConvolutionProcess::Initialize()
{
    SetName( "gaussianConvolutionProcess" );

    fSigma = 100.;

    fInputSignalEvent = new TRestSignalEvent();
    fOutputSignalEvent = new TRestSignalEvent();

    fInputEvent = fInputSignalEvent;
    fOutputEvent = fOutputSignalEvent;

}

void TRestSignalGaussianConvolutionProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestSignalGaussianConvolutionProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestSignalGaussianConvolutionProcess::BeginOfEventProcess() 
{
    fOutputSignalEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestSignalGaussianConvolutionProcess::ProcessEvent( TRestEvent *evInput )
{

    fInputSignalEvent = (TRestSignalEvent *) evInput;

    cout << "*****************" << endl;	
    cout << "New Event "<< endl;
    cout<< "Number of signals " << fInputSignalEvent->GetNumberOfSignals() << endl;

    if( fInputSignalEvent->GetNumberOfSignals() <= 0 ) return NULL;


    for( int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++ ) 
    {
        TRestSignal convSignal;

        // Asign ID and do convolution
   
        fInputSignalEvent->GetSignal(n)->GetSignalGaussianConvolution( &convSignal, fSigma );
        convSignal.SetSignalID( fInputSignalEvent->GetSignal(n)->GetSignalID() );

        fOutputSignalEvent->AddSignal( convSignal );

    }

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestSignalGaussianConvolutionProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalGaussianConvolutionProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalGaussianConvolutionProcess::InitFromConfigFile( )
{
    fSigma = GetDblParameterWithUnits( "sigma" );
}

