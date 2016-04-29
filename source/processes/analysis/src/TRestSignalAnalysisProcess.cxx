///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : apr/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestSignalAnalysisProcess.h"
using namespace std;


ClassImp(TRestSignalAnalysisProcess)
    //______________________________________________________________________________
TRestSignalAnalysisProcess::TRestSignalAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalAnalysisProcess::TRestSignalAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestSignalAnalysisProcess::~TRestSignalAnalysisProcess()
{
    delete fSignalEvent;
}

void TRestSignalAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::Initialize()
{
    SetName( "signalAnalysisProcess" );

    fSignalEvent = new TRestSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;
}

void TRestSignalAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::BeginOfEventProcess() 
{

}

//______________________________________________________________________________
TRestEvent* TRestSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{

    fSignalEvent = (TRestSignalEvent *) evInput;

    // TODO we must do this in each readout plane? 
    Double_t timeDelay = fSignalEvent->GetMaxTime() - fSignalEvent->GetMinTime();
    fAnalysisTree->SetObservableValue( "signalTimeBinsLength", timeDelay );

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::InitFromConfigFile( )
{

}

