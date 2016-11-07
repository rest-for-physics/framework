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
}

//______________________________________________________________________________
TRestGeant4AnalysisProcess::~TRestGeant4AnalysisProcess()
{
    delete fG4Event;
}

void TRestGeant4AnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::Initialize()
{
    SetName( "geant4AnalysisProcess" );

    fG4Event = new TRestG4Event();
    ///fOutputG4Event = new TRestG4Event();

    fOutputEvent = fG4Event;
    fInputEvent = fG4Event;
}

void TRestGeant4AnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();

    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::BeginOfEventProcess() 
{
}

//______________________________________________________________________________
TRestEvent* TRestGeant4AnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    *fG4Event =  *(( TRestG4Event *) evInput);

    TString obsName;

    Double_t energy = fG4Event->GetEnergyDepositedInVolume( 0 );
    obsName = this->GetName() + (TString) ".gasEnergyDeposit_InKeV";
    fAnalysisTree->SetObservableValue( obsName, energy );

    obsName = this->GetName() + (TString) ".photoelectric";
    if ( fG4Event->isPhotoElectric( ) ) { fAnalysisTree->SetObservableValue( obsName, 1 ); }
    else { fAnalysisTree->SetObservableValue( obsName, 0 ); }

    obsName = this->GetName() + (TString) ".compton";
    if ( fG4Event->isCompton( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".bremstralung";
    if ( fG4Event->isBremstralung( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    cout << "Event : " << fG4Event->GetID() << " Tracks : " << fG4Event->GetNumberOfTracks() << endl;

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

