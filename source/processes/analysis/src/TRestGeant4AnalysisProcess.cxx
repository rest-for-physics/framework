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
    delete fOutputG4Event;
    delete fInputG4Event;
}

void TRestGeant4AnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestGeant4AnalysisProcess::Initialize()
{
  
    SetSectionName( this->ClassName() );

    fInputG4Event = new TRestG4Event();
    fOutputG4Event = new TRestG4Event();

    fOutputEvent = fOutputG4Event;
    fInputEvent = fInputG4Event;
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
    {  
        if( fObservables[i].find( "VolumeEDep" ) != string::npos )
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

        if( fObservables[i].find( "MeanPos" ) != string::npos )
        {
            TString volName2 = fObservables[i].substr( 0, fObservables[i].length() - 8 ).c_str();
            std::string dirId = fObservables[i].substr( fObservables[i].length() - 1,1 ).c_str();

            Int_t volId2 = fG4Metadata->GetActiveVolumeID( volName2 );
            if( volId2 >= 0 )
            {
                fMeanPosObservables.push_back( fObservables[i] );
                fVolumeID2.push_back( volId2 );
                fDirID.push_back( dirId );
            }

            if( volId2 == -1 )
            {
                cout << endl;
                cout << "??????????????????????????????????????????????????" << endl;
                cout << "REST warning : TRestGeant4AnalysisProcess." << endl;
                cout << "------------------------------------------" << endl;
                cout << endl;
                cout << " Volume " << volName2 << " is not an active volume" << endl;
                cout << endl;
                cout << "List of active volumes : " << endl;
                cout << "------------------------ " << endl;

                for( int n = 0; n < fG4Metadata->GetNumberOfActiveVolumes( ); n++ )
                    cout << "Volume " << n << " : " << fG4Metadata->GetActiveVolumeName( n ) << endl;
                cout << "??????????????????????????????????????????????????" << endl;
                cout << endl;
            }

            if( (dirId  !="X") && (dirId !="Y") && (dirId !="Z") )
            {
                cout << endl;
                cout << "??????????????????????????????????????????????????" << endl;
                cout << "REST warning : TRestGeant4AnalysisProcess." << endl;
                cout << "------------------------------------------" << endl;
                cout << endl;
                cout << " Direction " << dirId << " is not valid" << endl;
                cout << " Only X, Y or Z accepted" << endl;
                cout << endl;
            }
        }

        if( fObservables[i].find( "TracksCounter" ) != string::npos )
        {
            TString partName = fObservables[i].substr( 0, fObservables[i].length() - 12 ).c_str();

            fTrackCounterObservables.push_back( fObservables[i] );
            fParticleTrackCounter.push_back( (string) partName );
        }

        if( fObservables[i].find( "TracksEDep" ) != string::npos )
        {
            TString partName = fObservables[i].substr( 0, fObservables[i].length() - 10 ).c_str();

            fTracksEDepObservables.push_back( fObservables[i] );
            fParticleTrackEdep.push_back( (string) partName );
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
    *fOutputG4Event =  *(( TRestG4Event *) evInput);

    TString obsName;

    Double_t energy = fOutputG4Event->GetSensitiveVolumeEnergy( );

    if( energy < fLowEnergyCut ) return NULL;
    if( fHighEnergyCut > 0 && energy > fHighEnergyCut ) return NULL;

    Double_t energyTotal = fOutputG4Event->GetTotalDepositedEnergy();
    obsName = this->GetName() + (TString) ".totalEdep";
    fAnalysisTree->SetObservableValue( obsName, energyTotal );

    obsName = this->GetName() + (TString) ".photoelectric";
    if ( fOutputG4Event->isPhotoElectric( ) ) { fAnalysisTree->SetObservableValue( obsName, 1 ); }
    else { fAnalysisTree->SetObservableValue( obsName, 0 ); }

    obsName = this->GetName() + (TString) ".compton";
    if ( fOutputG4Event->isCompton( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".bremstralung";
    if ( fOutputG4Event->isBremstralung( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".hadElastic";
    if ( fOutputG4Event->ishadElastic( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".neutronInelastic";
    if ( fOutputG4Event->isneutronInelastic( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".nCapture";
    if ( fOutputG4Event->isnCapture( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    obsName = this->GetName() + (TString) ".hIoni";
    if ( fOutputG4Event->ishIoni( ) ) fAnalysisTree->SetObservableValue( obsName, 1 );
    else fAnalysisTree->SetObservableValue( obsName, 0 );

    for( unsigned int n = 0; n < fParticleTrackCounter.size(); n++ )
    {
        Int_t nT = fOutputG4Event->GetNumberOfTracksForParticle( fParticleTrackCounter[n] );
        TString obsName = fTrackCounterObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, nT );
    }

    for( unsigned int n = 0; n < fTracksEDepObservables.size(); n++ )
    {
        Double_t energy = fOutputG4Event->GetEnergyDepositedByParticle( fParticleTrackEdep[n] );
        TString obsName = fTracksEDepObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, energy );
    }

    for( unsigned int n = 0; n < fEnergyInObservables.size(); n++ )
    {
        Double_t en = fOutputG4Event->GetEnergyDepositedInVolume( fVolumeID[n] );
        TString obsName = fEnergyInObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, en );
    }

    for( unsigned int n = 0; n < fMeanPosObservables.size(); n++ )
    {
        TString obsName = fMeanPosObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;

        Double_t mpos=0;
        if(fDirID[n]==(TString)"X")
            mpos = fOutputG4Event->GetMeanPositionInVolume(fVolumeID2[n]).X();  

        else if(fDirID[n]==(TString)"Y")
            mpos = fOutputG4Event->GetMeanPositionInVolume(fVolumeID2[n]).Y(); 

        else if(fDirID[n]==(TString)"Z")
            mpos = fOutputG4Event->GetMeanPositionInVolume(fVolumeID2[n]).Z(); 

        fAnalysisTree->SetObservableValue( obsName, mpos );
    }

    //cout << "Event : " << fOutputG4Event->GetID() << " G4 Tracks : " << fOutputG4Event->GetNumberOfTracks() << endl;
    return fOutputG4Event;
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

