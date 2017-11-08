/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
/// The TRestGeant4AnalysisProcess allows to extract valuable information
/// from a TRestG4Event which contains the information of energy deposits
/// from a Geant4 MonteCarlo simulation. TRestG4Event stores hits which 
/// include the physical process and geometry volume id where the 
/// interaction took place. This process accesses the Geant4 event data 
/// and metadata information and it will add the observables defined by 
/// the user to the analysisTree.
/// 
/// This process receives two optional parameters to define the energy
/// range  of the events to be processed. Only the events that are within
/// the range `(lowEnergyCut, highEnergyCut)` will be further 
/// considered. The events outside this energy range will be rejected, and
/// will be not processed in future steps. The processing continues 
/// immediately to the next event.
///
/// The following lines of code ilustrate how to implement these 
/// parameters inside the TRestGeant4AnalysisProcess metadata section.
/// 
/// \code 
///    // Only events between 100keV-1MeV will be further considered
///    <parameter name="lowEnergyCut" value="100" units="keV" > 
///    <parameter name="highEnergyCut" value="1" units="MeV" > 
/// \endcode
///
/// \note If these parameters are not defined the low and/or high energy
/// cuts will be just ignored.
///
/// This process includes generic observables by using a common pattern
/// inside the observable name. These observables require to be completed
/// with the name of a volume used in the GDML geometry, or the name
/// of a particle following Geant4 conventions. 
///
/// The following list describes the generic observables that can be used
/// in this analysis process.
///
/// * **VolumeEDep**: This observable will register the total 
/// energy deposited in a particular volume of the geometry. We
/// must specify the volume name before the keyword `volumeEDep`.
/// \code
///    // gasVolumeEDep will register the total energy deposited at the gas volume  
///    <observable name="gasVolumeEDep" value="ON"
///            description="Energy deposited in the gas volume in keV" />
///
///    // vesselVolumeEDep will register the total energy deposited at the vessel volume  
///    <observable name="vesselVolumeEDep" value="ON"
///            description="Energy deposited in the vessel volume in keV" />
/// \endcode
///
/// * **MeanPos**: This observable will register the mean position of 
/// the hits that have been registered in a particular volume of the
/// geometry. We must specify the volume name before the keyword `MeanPos`,
/// and after the keyword the axis we want to register.
/// \code
///    // It will register the mean position in X at the gas volume
///    <observable name="gasMeanPosX" value="ON"
///            description="Mean hits position in the gas volume (X-axis)" />
///
///    // It will register the mean position in Y at the gas volume
///    <observable name="gasMeanPosY" value="ON"
///            description="Mean hits position in the gas volume (Y-axis)" />
/// \endcode
///
/// * **TracksCounter**: This observable will register the number of
/// tracks of a given Geant4 particle found at each event. The keyword
/// `TracksCounter` must be preceded by the name of the particle.
/// \code
///    // It will register the number of neutron tracks per event
///    <observable name="neutronTrackCounter" value="ON"
///            description="Number of neutron tracks found in the event" />
///
///    // It will register the number of gamma tracks per event
///    <observable name="gammaTrackCounter" value="ON"
///            description="Number of gamma tracks found in the event" />
/// \endcode
///
/// * **TracksEDep**: This observable will register the total energy
/// deposited by a given particle. The keyword `TracksEDep` must be
/// preceded by the name of the particle.
/// \code
///    // It will register the number of neutron tracks per event
///    <observable name="neutronTrackCounter" value="ON"
///            description="Energy deposited in the gas volume in keV" />
///
///    // It will register the number of neutron tracks per event
///    <observable name="gammaTrackCounter" value="ON"
///            description="Energy deposited in the gas volume in keV" />
/// \endcode
///
/// \warning It is important to notice that the keyword used for these
/// observables is case sensitive!
///
/// In addition to these generic observables we have also an
/// observable to measure the total energy deposited in all volumes.
/// \code
///    // It will register the number of neutron tracks per event
///    <observable name="totalEdep" value="ON"
///            description="Total event energy registered in keV" />
/// \endcode
/// 
/// There are also observables allowing to identify the events where
/// certain physical processes took place. This will allow, for 
/// example, to select event populations where particular physics 
/// processes happened, or to determine the probability of a certain 
/// physics process to occur in our simulation.
///
/// The following list defines the physics processes observables
/// that can be defined in this analysis process. These observables
/// only register if the process happened or not, by associating
/// them a value, 1 or 0.
///
/// * **photoelectric**: It is 1 if a gamma was absorbed by a 
/// photoelectric process, 0 otherwise.
/// * **compton**:  It is 1 if a gamma was scattered by a
/// compton scattering process, 0 otherwise.
/// * **bremstralung**: It is 1 if gamma radiation was 
/// produced by a bremstralung process, 0 otherwise.
/// * **hadElastic**: It is 1 if hadron elastic scattering 
/// took place in that event, 0 otherwise.
/// * **neutronInelastic**: It is 1 if neutron inelastic
/// scattering took place inside the event, 0 otherwise.
/// * **nCapture**: It is 1 if a neutron capture process 
/// happened, 0 otherwise.
/// * **hIoni**: It is 1 if a hadron ionization process took
/// place in the event, 0 otherwise.
///
/// The following code ilustrates the addition of a physics 
/// process observable.
/// 
/// \code
///    <observable name="bremstralung" value="ON"
///        description="Its value will be 1 if the event contains a bremstralung physics process." />
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-March: First implementation of Geant4 analysis process into REST_v2.
///             Javier Galan
///
/// 2017-October: Generic upgrades to add particle and volume observables.
///               Gloria Luzon and Javier Galan
///
/// \class      TRestGeant4AnalysisProcess
/// \author     Javier Galan
/// \author     Gloria Luzon
///
/// <hr>
///
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
#include "TRestGeant4AnalysisProcess.h"
using namespace std;


ClassImp(TRestGeant4AnalysisProcess)

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestGeant4AnalysisProcess::TRestGeant4AnalysisProcess()
{
    Initialize();
}

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
/// 
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or relative.
///
/// The default behaviour is that the config file must be specified with 
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestGeant4AnalysisProcess::TRestGeant4AnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

///////////////////////////////////////////////
/// \brief Default destructor 
/// 
TRestGeant4AnalysisProcess::~TRestGeant4AnalysisProcess()
{
    delete fOutputG4Event;
    delete fInputG4Event;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
/// 
void TRestGeant4AnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the section name
/// 
void TRestGeant4AnalysisProcess::Initialize()
{
  
    SetSectionName( this->ClassName() );

    fInputG4Event = new TRestG4Event();
    fOutputG4Event = new TRestG4Event();

    fOutputEvent = fOutputG4Event;
    fInputEvent = fInputG4Event;
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration file.
/// 
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the 
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestGeant4AnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

///////////////////////////////////////////////
/// \brief Process initialization. Observable names are interpreted and auxiliar observable
/// members, related to VolumeEdep, MeanPos, TracksCounter, TrackEDep observables 
/// defined in TRestGeant4AnalysisProcess are filled at this stage.
/// 
void TRestGeant4AnalysisProcess::InitProcess()
{
    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );

    std::vector <string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for( unsigned int i = 0; i < fObservables.size(); i++ )
    {  
        if( fObservables[i].find( "VolumeEDep" ) != string::npos )
        {
            TString volName = fObservables[i].substr( 0, fObservables[i].length() - 10 ).c_str();

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
            TString partName = fObservables[i].substr( 0, fObservables[i].length() - 13 ).c_str();

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

///////////////////////////////////////////////
/// \brief Function to include required initialization before each event starts to process.
/// 
void TRestGeant4AnalysisProcess::BeginOfEventProcess() 
{
}

///////////////////////////////////////////////
/// \brief The main processing event function
/// 
TRestEvent* TRestGeant4AnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    *fOutputG4Event =  *(( TRestG4Event *) evInput);

    TString obsName;

    Double_t energy = fOutputG4Event->GetSensitiveVolumeEnergy( );

    if( GetVerboseLevel() >= REST_Debug )
    {
        cout << "----------------------------" << endl;
        cout << "TRestG4Event : " << fOutputG4Event->GetID() << endl;
        cout << "Sensitive volume Energy : " << energy << endl;
        cout << "Total energy : " << fOutputG4Event->GetTotalDepositedEnergy() << endl;;
    }

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

    if( GetVerboseLevel() >= REST_Debug )
    {
        cout << "G4 Tracks : " << fOutputG4Event->GetNumberOfTracks() << endl;
        cout << "----------------------------" << endl;
    }
    return fOutputG4Event;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after each event has been processed.
/// 
void TRestGeant4AnalysisProcess::EndOfEventProcess() 
{

}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been processed.
/// 
void TRestGeant4AnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML TRestGeant4AnalysisProcess metadata section
/// 
void TRestGeant4AnalysisProcess::InitFromConfigFile( )
{
    fLowEnergyCut = GetDblParameterWithUnits ( "lowEnergyCut", 0 );
    fHighEnergyCut = GetDblParameterWithUnits ( "highEnergyCut", 0 );
}

