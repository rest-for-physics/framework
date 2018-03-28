///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalAnalysisProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalAnalysisProcess
///             Date : feb/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include <TPaveText.h>
#include <TLegend.h>

#include "TRestRawSignalAnalysisProcess.h"
using namespace std;

ClassImp(TRestRawSignalAnalysisProcess)
//______________________________________________________________________________
TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestRawSignalAnalysisProcess::~TRestRawSignalAnalysisProcess()
{
    delete fSignalEvent;
}

void TRestRawSignalAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fSignalEvent = new TRestRawSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;

    fCutsEnabled = false;
    fFirstEventTime = -1;
    fPreviousEventTime.clear();

    fDrawRefresh = 0;

    time(&timeStored);
}

void TRestRawSignalAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::InitProcess()
{
    fSignalAnalysisObservables = TRestEventProcess::ReadObservables();
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::BeginOfEventProcess() 
{
    fSignalEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    TRestRawSignalEvent *fInputSignalEvent = (TRestRawSignalEvent *) evInput;

    /// Copying the signal event to the output event

    fSignalEvent->SetID( fInputSignalEvent->GetID() );
    fSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    //for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++ )
    Int_t N = fInputSignalEvent->GetNumberOfSignals();
    if( GetVerboseLevel() >= REST_Debug ) N = 1;
    for( int sgnl = 0; sgnl < N; sgnl++ )
        fSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( sgnl ) );
    /////////////////////////////////////////////////

    if( fFirstEventTime == -1 )
        fFirstEventTime = fSignalEvent->GetTime( );

    TString obsName;

    Double_t secondsFromStart = fSignalEvent->GetTime() - fFirstEventTime;
    if( secondsFromStart < 0 ) secondsFromStart = 0;
    obsName = this->GetName() + (TString) ".SecondsFromStart";
    fAnalysisTree->SetObservableValue( obsName, secondsFromStart );

    obsName = this->GetName() + (TString) ".HoursFromStart";
    fAnalysisTree->SetObservableValue( obsName, secondsFromStart/3600. );

    Double_t evTimeDelay = 0;
    if( fPreviousEventTime.size() > 0 )
        evTimeDelay = fSignalEvent->GetTime() - fPreviousEventTime.back();
    obsName = this->GetName() + (TString) ".EventTimeDelay";
    fAnalysisTree->SetObservableValue( obsName, evTimeDelay );

    Double_t meanRate = 0;
    if( fPreviousEventTime.size() == 100 )
        meanRate = 100. / (fSignalEvent->GetTime()-fPreviousEventTime.front());

    obsName = this->GetName() + (TString) ".MeanRate_InHz";
    fAnalysisTree->SetObservableValue( obsName, meanRate );

    Double_t baseLineMean = fSignalEvent->GetBaseLineAverage( fBaseLineRange.X(), fBaseLineRange.Y() );

    obsName = this->GetName() + (TString) ".BaseLineMean";
    fAnalysisTree->SetObservableValue( obsName, baseLineMean );

    Double_t tailLineMean = fSignalEvent->GetBaseLineAverage( fTailLineRange.X(), fTailLineRange.Y() );

    obsName = this->GetName() + (TString) ".TailLineMean";
    fAnalysisTree->SetObservableValue( obsName, tailLineMean );

    // Mean base line cut
    if( fCutsEnabled )
        if( baseLineMean > fMeanBaseLineCutRange.Y() || baseLineMean < fMeanBaseLineCutRange.X() ) return NULL; 

    /////////////////////////////////////////////////

    Double_t baseLineSigma = fSignalEvent->GetBaseLineSigmaAverage( fBaseLineRange.X(), fBaseLineRange.Y() );

    obsName = this->GetName() + (TString) ".BaseLineSigmaMean";
    fAnalysisTree->SetObservableValue( obsName, baseLineSigma );

    // Base line noise cut
    if( fCutsEnabled )
        if( baseLineSigma > fMeanBaseLineSigmaCutRange.Y() || baseLineSigma < fMeanBaseLineSigmaCutRange.X() ) return NULL;

    /////////////////////////////////////////////////

    obsName = this->GetName() + (TString) ".TimeBinsLength";
    Double_t timeDelay = fSignalEvent->GetMaxTime() - fSignalEvent->GetMinTime();
    fAnalysisTree->SetObservableValue( obsName, timeDelay );

    /////////////////////////////////////////////////

    obsName = this->GetName() + (TString) ".NumberOfSignals";
    Double_t nSignals = fSignalEvent->GetNumberOfSignals();
    fAnalysisTree->SetObservableValue( obsName, nSignals );

    // Signals are zero-ed
    fSignalEvent->SubstractBaselines( fBaseLineRange.X(), fBaseLineRange.Y() );

    Int_t from = (Int_t) fAnalysisRange.X();
    Int_t to = (Int_t) fAnalysisRange.Y();
    Double_t fullIntegral = fSignalEvent->GetIntegral( from, to );
    obsName = this->GetName() + (TString) ".FullIntegral";
    fAnalysisTree->SetObservableValue( obsName, fullIntegral );

    fSignalEvent->SetTailPoints( fTailPoints );

    Double_t thrIntegral = fSignalEvent->GetIntegralWithThreshold( from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold );
    obsName = this->GetName() + (TString) ".ThresholdIntegral";
    fAnalysisTree->SetObservableValue( obsName, thrIntegral );

    Double_t riseSlope = fSignalEvent->GetRiseSlope();
    obsName = this->GetName() + (TString) ".RiseSlopeAvg";
    fAnalysisTree->SetObservableValue( obsName, riseSlope );

    Double_t slopeIntegral = fSignalEvent->GetSlopeIntegral();
    obsName = this->GetName() + (TString) ".SlopeIntegral";
    fAnalysisTree->SetObservableValue( obsName, slopeIntegral );

    Double_t rateOfChange = riseSlope/slopeIntegral;
    if( slopeIntegral == 0 ) rateOfChange = 0;
    obsName = this->GetName() + (TString) ".RateOfChangeAvg";
    fAnalysisTree->SetObservableValue( obsName, rateOfChange );

    Double_t riseTime = fSignalEvent->GetRiseTime();
    obsName = this->GetName() + (TString) ".RiseTimeAvg";
    fAnalysisTree->SetObservableValue( obsName, riseTime );

    Double_t tripleMaxIntegral = fSignalEvent->GetTripleMaxIntegral();
    obsName = this->GetName() + (TString) ".TripleMaxIntegral";
    fAnalysisTree->SetObservableValue( obsName, tripleMaxIntegral );

    Double_t integralRatio = (fullIntegral-thrIntegral)/(fullIntegral+thrIntegral);
    obsName = this->GetName() + (TString) ".IntegralBalance";
    fAnalysisTree->SetObservableValue( obsName, integralRatio );

    Double_t maxValue = 0;
    Double_t minValue = 1.e6;
    Double_t maxValueIntegral = 0;

    Double_t minPeakTime = 5000;
    Double_t maxPeakTime = 0;
    Double_t peakTimeAverage = 0;

    Int_t nGoodSignals = 0;
    for( int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++ )
    {
        TRestRawSignal *sgnl = fSignalEvent->GetSignal( s );
        if( sgnl->GetIntegralWithThreshold( from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold, fNPointsOverThreshold, fSignalThreshold ) > 0 )
        {
            Double_t value = fSignalEvent->GetSignal(s)->GetMaxValue( from, to );
            if( value > maxValue ) maxValue = value;
            if( value < minValue ) minValue = value;
            maxValueIntegral += value;

            Double_t peakBin = sgnl->GetMaxPeakBin( from, to );
            peakTimeAverage += peakBin;

            if( minPeakTime > peakBin ) minPeakTime = peakBin;
            if( maxPeakTime < peakBin ) maxPeakTime = peakBin;

            nGoodSignals++;
        }
    }

    obsName = this->GetName() + (TString) ".MinPeakTime";
    fAnalysisTree->SetObservableValue( obsName, minPeakTime );

    if( nGoodSignals > 0 ) peakTimeAverage /= nGoodSignals;

    Double_t ampIntRatio = thrIntegral/maxValueIntegral;
    if( maxValueIntegral == 0 ) ampIntRatio = 0;
    obsName = this->GetName() + (TString) ".AmplitudeIntegralRatio";
    fAnalysisTree->SetObservableValue( obsName, ampIntRatio );

    obsName = this->GetName() + (TString) ".NumberOfGoodSignals";
    fAnalysisTree->SetObservableValue( obsName, nGoodSignals );

    obsName = this->GetName() + (TString) ".MinPeakAmplitude";
    fAnalysisTree->SetObservableValue( obsName, minValue );

    obsName = this->GetName() + (TString) ".MaxPeakAmplitude";
    fAnalysisTree->SetObservableValue( obsName, maxValue );

    obsName = this->GetName() + (TString) ".PeakAmplitudeIntegral";
    fAnalysisTree->SetObservableValue( obsName, maxValueIntegral );

    Double_t amplitudeRatio = maxValueIntegral/maxValue;
    if( maxValue == 0 ) amplitudeRatio = 0;
    obsName = this->GetName() + (TString) ".AmplitudeRatio";
    fAnalysisTree->SetObservableValue( obsName, amplitudeRatio );

    obsName = this->GetName() + (TString) ".MaxPeakTime";
    fAnalysisTree->SetObservableValue( obsName, maxPeakTime );

    obsName = this->GetName() + (TString) ".MinPeakTime";
    fAnalysisTree->SetObservableValue( obsName, minPeakTime );

    Double_t peakTimeDelay = maxPeakTime - minPeakTime;
    obsName = this->GetName() + (TString) ".MaxPeakTimeDelay";
    fAnalysisTree->SetObservableValue( obsName, peakTimeDelay );

    obsName = this->GetName() + (TString) ".AveragePeakTime";
    fAnalysisTree->SetObservableValue( obsName, peakTimeAverage );


    // Cuts
    if( fCutsEnabled )
    {
        if( nSignals < fMaxNumberOfSignalsCut.X() || nSignals > fMaxNumberOfSignalsCut.Y() )  return NULL;
        if( nGoodSignals < fMaxNumberOfGoodSignalsCut.X() || nGoodSignals > fMaxNumberOfGoodSignalsCut.Y() ) return NULL;
        if( thrIntegral < fThresholdIntegralCut.X() || thrIntegral > fThresholdIntegralCut.Y() ) return NULL;
        if( fullIntegral < fFullIntegralCut.X() || fullIntegral > fFullIntegralCut.Y() ) return NULL;
        if( peakTimeDelay < fPeakTimeDelayCut.X() || peakTimeDelay > fPeakTimeDelayCut.Y() ) return NULL;
        if( maxValue < fPeakMaxAmplitudeCut ) return NULL;
    }

    if( GetVerboseLevel() >= REST_Debug ) 
        fAnalysisTree->PrintObservables();

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::EndOfEventProcess() 
{
    fPreviousEventTime.push_back( fSignalEvent->GetTimeStamp() );
    if( fPreviousEventTime.size() > 100 ) fPreviousEventTime.erase( fPreviousEventTime.begin() );
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::InitFromConfigFile( )
{
    fDrawRefresh = StringToDouble( GetParameter( "refreshEvery", "0" ) );

    fBaseLineRange = StringTo2DVector( GetParameter( "baseLineRange", "(5,55)") );
    fTailLineRange = StringTo2DVector( GetParameter( "tailLineRange", "(450,500)") );
    fAnalysisRange = StringTo2DVector( GetParameter( "analysisRange", "(10,500)") );
    fPointThreshold = StringToDouble( GetParameter( "pointThreshold", 2 ) );
    fNPointsOverThreshold = StringToInteger( GetParameter( "pointsOverThreshold", 5 ) );
    fSignalThreshold = StringToDouble( GetParameter( "signalThreshold", 5 ) );

    fMeanBaseLineCutRange = StringTo2DVector( GetParameter( "meanBaseLineCutRange", "(0,4096)") );
    fMeanBaseLineSigmaCutRange = StringTo2DVector( GetParameter( "meanBaseLineSigmaCutRange", "(0,4096)") );
    fMaxNumberOfSignalsCut = StringTo2DVector( GetParameter( "maxNumberOfSignalsCut", "(0,20)" ) );
    fMaxNumberOfGoodSignalsCut = StringTo2DVector( GetParameter( "maxNumberOfGoodSignalsCut", "(0,20)" ) );

    fFullIntegralCut  = StringTo2DVector( GetParameter( "fullIntegralCut", "(0,100000)" ) );
    fThresholdIntegralCut  = StringTo2DVector( GetParameter( "thresholdIntegralCut", "(0,100000)" ) );

    fPeakMaxAmplitudeCut  = StringToInteger( GetParameter( "maxAmplitudeCut", "50)" ) );

    fPeakTimeDelayCut  = StringTo2DVector( GetParameter( "peakTimeDelayCut", "(0,20)" ) );
    if( GetParameter( "cutsEnabled", "false" ) == "true" ) fCutsEnabled = true;

    fTailPoints = StringToInteger( GetParameter( "tailPoints", "0" ) );

}

