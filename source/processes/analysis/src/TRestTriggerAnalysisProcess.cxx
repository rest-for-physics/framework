///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTriggerAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : apr/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestTriggerAnalysisProcess.h"
using namespace std;


ClassImp(TRestTriggerAnalysisProcess)
    //______________________________________________________________________________
TRestTriggerAnalysisProcess::TRestTriggerAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestTriggerAnalysisProcess::TRestTriggerAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestTriggerAnalysisProcess::~TRestTriggerAnalysisProcess()
{
    delete fSignalEvent;
}

void TRestTriggerAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestTriggerAnalysisProcess::Initialize()
{
    SetName( "triggerAnalysisProcess" );

    fSignalEvent = new TRestSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;

    fW = 0;
}

void TRestTriggerAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestTriggerAnalysisProcess::InitProcess()
{
    std::vector <string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for( unsigned int i = 0; i < fObservables.size(); i++ )
        if( fObservables[i].find( "Integral_Eth") != string::npos )
            fIntegralObservables.push_back( fObservables[i] );

    for( unsigned int i = 0; i < fIntegralObservables.size(); i++ )
    {
        Double_t th = atof ( fIntegralObservables[i].substr( 12, fIntegralObservables[i].length() - 12 ).c_str() );
        fThreshold.push_back( th );
    }

    TRestGas *gas = (TRestGas * ) GetGasMetadata(); 
    fW = gas->GetWvalue();
}

//______________________________________________________________________________
void TRestTriggerAnalysisProcess::BeginOfEventProcess() 
{
    fSignalEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestTriggerAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    TString obsName;

    TRestSignalEvent *fInputSignalEvent = (TRestSignalEvent *) evInput;

    /// Copying the signal event to the output event
    fSignalEvent->SetID( fInputSignalEvent->GetID() );
    fSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++ )
        fSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( sgnl ) );
    /////////////////////////////////////////////////

    vector <Double_t> integral;

    for( unsigned int i = 0; i < fIntegralObservables.size(); i++ )
        integral.push_back(0);

    TRestSignal *sgnl = fSignalEvent->GetSignal( 0 );

    Int_t maxT = sgnl->GetMaxTime();
    Int_t minT = sgnl->GetMinTime();

    unsigned int counter = 0;
    unsigned int nObs = fIntegralObservables.size();
    for( int i = minT-256; i <= maxT && counter < nObs ; i++ )
    {
        Double_t en = sgnl->GetIntegralWithTime( i, i+256 ) * fW / 1000.;

        for( unsigned int n = 0; n < nObs; n++ )
            if ( integral[n] == 0 && en > fThreshold[n] )
                integral[n] = sgnl->GetIntegralWithTime( i, i+512 );

        // Break condition
        counter = 0;
        for( unsigned int n = 0; n < nObs; n++ )
            if( integral[n] > 0 ) counter++;
    }

    
    for( unsigned int i = 0; i < nObs ; i++ )
    {
        obsName = this->GetName() + (TString) "." + fIntegralObservables[i];
        fAnalysisTree->SetObservableValue( obsName, integral[i] * fW / 1000. );
    }

    Double_t full = sgnl->GetIntegralWithTime( minT-1, maxT+1 );

    obsName = this->GetName() + (TString) ".RawIntegral";
    fAnalysisTree->SetObservableValue( obsName, full * fW / 1000. );


    return fSignalEvent;
}

//______________________________________________________________________________
void TRestTriggerAnalysisProcess::EndOfEventProcess() 
{
}

//______________________________________________________________________________
void TRestTriggerAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestTriggerAnalysisProcess::InitFromConfigFile( )
{


}

