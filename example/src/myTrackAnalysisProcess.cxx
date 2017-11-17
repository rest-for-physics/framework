///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myTrackAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : nov/2017
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "myTrackAnalysisProcess.h"
using namespace std;


ClassImp(myTrackAnalysisProcess)
    //______________________________________________________________________________
myTrackAnalysisProcess::myTrackAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
myTrackAnalysisProcess::myTrackAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
myTrackAnalysisProcess::~myTrackAnalysisProcess()
{
    delete fTrackEvent;
}

void myTrackAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void myTrackAnalysisProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fTrackEvent = new TRestTrackEvent();

    fOutputEvent = fTrackEvent;
    fInputEvent = fTrackEvent;
}

void myTrackAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void myTrackAnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();
}

//______________________________________________________________________________
void myTrackAnalysisProcess::BeginOfEventProcess() 
{
}

//______________________________________________________________________________
TRestEvent* myTrackAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    *fTrackEvent =  *(( TRestTrackEvent *) evInput);

    TString obsName;

    Double_t energy = 0;
    for( int tck = 0; tck < fTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fTrackEvent->isTopLevel( tck ) ) continue;

        TRestTrack *t = fTrackEvent->GetTrack( tck );

        energy += t->GetEnergy( );
    }


    cout << "Dummy output. Adding track energy : " << energy << endl;
    obsName = this->GetName() + (TString) ".TracksEnergy";
    fAnalysisTree->SetObservableValue( obsName, energy );

    return fTrackEvent;
}

//______________________________________________________________________________
void myTrackAnalysisProcess::EndOfEventProcess() 
{
}

//______________________________________________________________________________
void myTrackAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void myTrackAnalysisProcess::InitFromConfigFile( )
{
}

