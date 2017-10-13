///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsAnalysisProcess.cxx
///
///
///             First implementation of hits analysis process into REST_v2
///             Date : may/2016
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestHitsAnalysisProcess.h"
using namespace std;

ClassImp(TRestHitsAnalysisProcess)
    //______________________________________________________________________________
TRestHitsAnalysisProcess::TRestHitsAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestHitsAnalysisProcess::TRestHitsAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestHitsAnalysisProcess::~TRestHitsAnalysisProcess()
{
    delete fInputHitsEvent;
    delete fOutputHitsEvent;
}

void TRestHitsAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fInputHitsEvent = new TRestHitsEvent();
    fOutputHitsEvent = new TRestHitsEvent();

    fOutputEvent = fOutputHitsEvent;
    fInputEvent = fInputHitsEvent;
}

void TRestHitsAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::BeginOfEventProcess() 
{
    fOutputHitsEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestHitsAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputHitsEvent = (TRestHitsEvent *) evInput;
   
    TString obsName;

    TransferEvent( fOutputHitsEvent, fInputHitsEvent );
    
   
    Double_t energy = fOutputHitsEvent->GetEnergy( );
    TVector3 meanPosition = fOutputHitsEvent->GetMeanPosition();

    Int_t isInsideCylindricalVolume=0;
    if( fOutputHitsEvent->isHitsEventInsideCylinder( fCyl_x0, fCyl_x1, fCyl_R) )
        isInsideCylindricalVolume = 1;

    Int_t nInsideCylindricalVolume = fOutputHitsEvent->GetNumberOfHitsInsideCylinder( fCyl_x0, fCyl_x1, fCyl_R);

    obsName = this->GetName() + (TString) ".isInsideCylindricalVolume";
    fAnalysisTree->SetObservableValue( obsName, isInsideCylindricalVolume );

    obsName = this->GetName() + (TString) ".nInsideCylindricalVolume";
    fAnalysisTree->SetObservableValue( obsName, nInsideCylindricalVolume );

    obsName = this->GetName() + (TString) ".energy";
    fAnalysisTree->SetObservableValue( obsName, energy );

    obsName = this->GetName() + (TString) ".xMean";
    fAnalysisTree->SetObservableValue( obsName, meanPosition.X() );

    obsName = this->GetName() + (TString) ".yMean";
    fAnalysisTree->SetObservableValue( obsName, meanPosition.Y() );

    obsName = this->GetName() + (TString) ".zMean";
    fAnalysisTree->SetObservableValue( obsName, meanPosition.Z() );

    if( GetVerboseLevel() >= REST_Extreme )
    {
        fOutputHitsEvent->PrintEvent( 1000 );
        GetChar();
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::EndOfEventProcess() 
{
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}


//______________________________________________________________________________
void TRestHitsAnalysisProcess::InitFromConfigFile( )
{
   
    fCyl_x0 = Get3DVectorParameterWithUnits( "fiducialCylinder_x0", TVector3(0,0,0) );
    fCyl_x1 = Get3DVectorParameterWithUnits( "fiducialCylinder_x1", TVector3(0,0,0) );

    fCyl_R = GetDblParameterWithUnits( "fiducialCylinder_radius", 1 );
}

