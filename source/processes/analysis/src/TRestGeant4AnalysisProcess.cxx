///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4AnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestGeant4AnalysisProcess.h"
using namespace std;


ClassImp(TRestGeant4AnalysisProcess)
    //______________________________________________________________________________
TRestGeant4AnalysisProcess::TRestGeant4AnalysisProcess()
{
    Initialize();

}

//______________________________________________________________________________
TRestGeant4AnalysisProcess::TRestGeant4AnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );

    PrintMetadata();

    // TRestGeant4AnalysisProcess default constructor
}

//______________________________________________________________________________
TRestGeant4AnalysisProcess::~TRestGeant4AnalysisProcess()
{
    delete fG4Event;
    // TRestGeant4AnalysisProcess destructor
}

void TRestGeant4AnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );

    // TOBE implemented
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::Initialize()
{
    SetName( "geant4AnalysisProcess" );

    fG4Event = new TRestG4Event();

    fOutputEvent = fG4Event;
    fInputEvent = fG4Event;
}

void TRestGeant4AnalysisProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) ) LoadDefaultConfig( );

    PrintMetadata();
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.

    cout << __PRETTY_FUNCTION__ << endl;

    vector <string> obsList = GetObservablesList( );

    for( unsigned int n = 0; n < obsList.size(); n++ )
        fAnalysisTree->AddObservable( obsList[n] );

    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );

}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::BeginOfEventProcess() 
{

}

//______________________________________________________________________________
TRestEvent* TRestGeant4AnalysisProcess::ProcessEvent( TRestEvent *evInput )
{

    TRestG4Event *g4Event = (TRestG4Event *) evInput;

    Double_t energy = g4Event->GetEnergyDepositedInVolume( 0 );
    fAnalysisTree->SetObservableValue( "gasEnergyDeposit_InKeV", energy );

    if ( g4Event->isPhotoElectric( ) ) { fAnalysisTree->SetObservableValue( "photoelectric", 1 ); }
    else { fAnalysisTree->SetObservableValue( "photoelectric", 0 ); }

    if ( g4Event->isCompton( ) ) fAnalysisTree->SetObservableValue( "compton", 1 );
    else fAnalysisTree->SetObservableValue( "compton", 0 );

    if ( g4Event->isBremstralung( ) ) fAnalysisTree->SetObservableValue( "bremstralung", 1 );
    else fAnalysisTree->SetObservableValue( "bremstralung", 0 );

    cout << "Event : " << g4Event->GetID() << " Tracks : " << g4Event->GetNumberOfTracks() << endl;

    return fG4Event;
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::InitFromConfigFile( )
{

}

