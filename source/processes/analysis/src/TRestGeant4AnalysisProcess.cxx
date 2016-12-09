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
    SetSectionName( this->ClassName() );

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
    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );

    std::vector <string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();


    for( unsigned int i = 0; i < fObservables.size(); i++ )
        if( fObservables[i].find( "EDep" ) != string::npos )
        {
            TString volName = fObservables[i].substr( 0, fObservables[i].length() - 4 ).c_str();

            Int_t volId = fG4Metadata->GetActiveVolumeID( volName );
            if( volId >= 0 )
            {
                fEnergyInObservables.push_back( fObservables[i] );
                fVolumeID.push_back( volId );
            }

            if( volId == -1 )
            {
                cout << endl;
                cout << "??????????????????????????????????????????????????" << endl;
                cout << "REST warning : TRestGeant4AnalysisProcess." << endl;
                cout << "------------------------------------------" << endl;
                cout << endl;
                cout << " Volume " << volName << " is not an active volume" << endl;
                cout << endl;
                cout << "List of active volumes : " << endl;
                cout << "------------------------ " << endl;

                for( int n = 0; n < fG4Metadata->GetNumberOfActiveVolumes( ); n++ )
                    cout << "Volume " << n << " : " << fG4Metadata->GetActiveVolumeName( n ) << endl;
                cout << "??????????????????????????????????????????????????" << endl;
                cout << endl;
            }
        }
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

    Double_t energy = fG4Event->GetSensitiveVolumeEnergy( );

    if( energy < fLowEnergyCut ) return NULL;
    if( fHighEnergyCut > 0 && energy > fHighEnergyCut ) return NULL;

    Double_t energyTotal = fG4Event->GetTotalDepositedEnergy();
    obsName = this->GetName() + (TString) ".totalEdep";
    fAnalysisTree->SetObservableValue( obsName, energyTotal );

    obsName = this->GetName() + (TString) ".photoelectric";
    if ( fG4Event->isPhotoElectric( ) ) { fAnalysisTree->SetObservableValue( obsName, 1 ); }
    else { fAnalysisTree->SetObservableValue( obsName, 0 ); }

    obsName = this->GetName() + (TString) ".compton";
    if ( fG4Event->isCompton( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".bremstralung";
    if ( fG4Event->isBremstralung( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".hadElastic";
    if ( fG4Event->ishadElastic( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".neutronInelastic";
    if ( fG4Event->isneutronInelastic( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".nCapture";
    if ( fG4Event->isnCapture( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".hIoni";
    if ( fG4Event->ishIoni( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".alpha";
    if ( fG4Event->isAlpha( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".neutron";
    if ( fG4Event->isNeutron( ) ) fAnalysisTree->SetObservableValue(obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".Ar";
    if ( fG4Event->isArgon( ) ) fAnalysisTree->SetObservableValue(obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".Xe";
    if ( fG4Event->isXenon( ) ) fAnalysisTree->SetObservableValue(obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".Ne";
    if ( fG4Event->isNeon( ) ) fAnalysisTree->SetObservableValue(obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    for( unsigned int n = 0; n < fEnergyInObservables.size(); n++ )
    {
        Double_t en = fG4Event->GetEnergyDepositedInVolume( fVolumeID[n] );
        TString obsName = fEnergyInObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, en );
    }

    cout << "Event : " << fG4Event->GetID() << " G4 Tracks : " << fG4Event->GetNumberOfTracks() << endl;
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
    fLowEnergyCut = GetDblParameterWithUnits ( "lowEnergyCut", 0 );
    fHighEnergyCut = GetDblParameterWithUnits ( "highEnergyCut", 0 );
}

