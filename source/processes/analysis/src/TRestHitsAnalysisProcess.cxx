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

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
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
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
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
    TString obsName;

    *fOutputHitsEvent =  *(( TRestHitsEvent *) evInput);

    Double_t energy = fOutputHitsEvent->GetEnergy( );
    TVector3 meanPosition = fOutputHitsEvent->GetMeanPosition();

    obsName = this->GetName() + (TString) ".energy";
    fAnalysisTree->SetObservableValue( obsName, energy );

    obsName = this->GetName() + (TString) ".xMean";
    fAnalysisTree->SetObservableValue( obsName, meanPosition.X() );

    obsName = this->GetName() + (TString) ".yMean";
    fAnalysisTree->SetObservableValue( obsName, meanPosition.Y() );

    obsName = this->GetName() + (TString) ".zMean";
    fAnalysisTree->SetObservableValue( obsName, meanPosition.Z() );

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
    /*
    fDrawRefresh = StringToDouble( GetParameter( "refreshEvery", "0" ) );

    fBaseLineRange = StringTo2DVector( GetParameter( "baseLineRange", "(5,55)") );
    fIntegralRange = StringTo2DVector( GetParameter( "integralRange", "(10,500)") );
    fPointThreshold = StringToDouble( GetParameter( "pointThreshold", 2 ) );
    fNPointsOverThreshold = StringToInteger( GetParameter( "pointsOverThreshold", 5 ) );
    fSignalThreshold = StringToDouble( GetParameter( "signalThreshold", 5 ) );

    fMeanBaseLineCutRange = StringTo2DVector( GetParameter( "meanBaseLineCutRange", "(0,4096)") );
    fMeanBaseLineSigmaCutRange = StringTo2DVector( GetParameter( "meanBaseLineSigmaCutRange", "(0,4096)") );
    fMaxNumberOfSignalsCut = StringTo2DVector( GetParameter( "maxNumberOfSignalsCut", "(0,20)" ) );
    fMaxNumberOfGoodSignalsCut = StringTo2DVector( GetParameter( "maxNumberOfGoodSignalsCut", "(0,20)" ) );

    fFullIntegralCut  = StringTo2DVector( GetParameter( "fullIntegralCut", "(0,100000)" ) );
    fThresholdIntegralCut  = StringTo2DVector( GetParameter( "thresholdIntegralCut", "(0,100000)" ) );

    fPeakTimeDelayCut  = StringTo2DVector( GetParameter( "peakTimeDelayCut", "(0,20)" ) );
    if( GetParameter( "cutsEnabled" ) == "true" ) fCutsEnabled = true;
    */


}

