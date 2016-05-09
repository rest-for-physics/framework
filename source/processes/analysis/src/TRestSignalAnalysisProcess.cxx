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

    fCutsEnabled = false;
    fFirstEventTime = -1;
    fPreviousEventTime.clear();
}

void TRestSignalAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();

    fChannelsHisto = new TH1D( "readoutChannelActivity", "readoutChannelActivity", 128, 0, 128 );

    fReadout = (TRestReadout*) GetReadoutMetadata();
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::BeginOfEventProcess() 
{
    fSignalEvent->Initialize();

}

//______________________________________________________________________________
TRestEvent* TRestSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    TString obsName;

    TRestSignalEvent *fInputSignalEvent = (TRestSignalEvent *) evInput;

    /// Copying the signal event to the output event

    fSignalEvent->SetID( fInputSignalEvent->GetID() );
    fSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    fSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    fSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++ )
        fSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( sgnl ) );
    /////////////////////////////////////////////////

    if( fFirstEventTime == -1 )
        fFirstEventTime = fInputSignalEvent->GetTime( );


    Double_t secondsFromStart = fInputSignalEvent->GetTime() - fFirstEventTime;
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

    Int_t from = (Int_t) fIntegralRange.X();
    Int_t to = (Int_t) fIntegralRange.Y();
    Double_t fullIntegral = fSignalEvent->GetIntegral( from, to );
    obsName = this->GetName() + (TString) ".FullIntegral";
    fAnalysisTree->SetObservableValue( obsName, fullIntegral );


    Double_t thrIntegral = fSignalEvent->GetIntegralWithThreshold( from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fThreshold, fNPointsOverThreshold, fMinPeakAmplitude );
    obsName = this->GetName() + (TString) ".ThresholdIntegral";
    fAnalysisTree->SetObservableValue( obsName, thrIntegral );

    Double_t integralRatio = (fullIntegral-thrIntegral)/(fullIntegral+thrIntegral);
    obsName = this->GetName() + (TString) ".IntegralRatio";
    fAnalysisTree->SetObservableValue( obsName, integralRatio );

    Double_t maxValue = 0;
    Double_t maxValueIntegral = 0;

    Double_t minPeakTime = 1000; // TODO sustitute this for something better
    Double_t maxPeakTime = 0;
    Double_t peakTimeAverage = 0;

    Int_t nGoodSignals = 0;
    for( int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++ )
    {
        TRestSignal *sgnl = fSignalEvent->GetSignal( s );
        if( sgnl->GetIntegralWithThreshold( from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fThreshold, fNPointsOverThreshold, fMinPeakAmplitude ) > 0 )
        {
            Double_t value = fSignalEvent->GetSignal(s)->GetMaxValue();
            if( value > maxValue ) maxValue = value;
            maxValueIntegral += value;

            Double_t peakTime = sgnl->GetMaxPeakTime();
            peakTimeAverage += peakTime;

            if( minPeakTime > peakTime ) minPeakTime = peakTime;
            if( maxPeakTime < peakTime ) maxPeakTime = peakTime;

            nGoodSignals++;
        }
    }

    if( nGoodSignals > 0 ) peakTimeAverage /= nGoodSignals;

    obsName = this->GetName() + (TString) ".NumberOfGoodSignals";
    fAnalysisTree->SetObservableValue( obsName, nGoodSignals );

    obsName = this->GetName() + (TString) ".MaxPeakAmplitude";
    fAnalysisTree->SetObservableValue( obsName, maxValue );

    obsName = this->GetName() + (TString) ".PeakAmplitudeIntegral";
    fAnalysisTree->SetObservableValue( obsName, maxValueIntegral );

    Double_t amplitudeRatio = maxValueIntegral/maxValue;
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
    }

    TRestReadoutModule *mod = fReadout->GetReadoutPlane(0)->GetModule(0);
    for( int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++ )
    {
        Int_t readoutChannel = mod->DaqToReadoutChannel( fSignalEvent->GetSignal(s)->GetID() );
        fChannelsHisto->Fill( readoutChannel );

    }

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::EndOfEventProcess() 
{
    fPreviousEventTime.push_back( fSignalEvent->GetTimeStamp() );
    if( fPreviousEventTime.size() > 100 ) fPreviousEventTime.erase( fPreviousEventTime.begin() );
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
    fChannelsHisto->Write();
}

//______________________________________________________________________________
void TRestSignalAnalysisProcess::InitFromConfigFile( )
{
    fBaseLineRange = StringTo2DVector( GetParameter( "baseLineRange", "(5,55)") );
    fIntegralRange = StringTo2DVector( GetParameter( "integralRange", "(10,500)") );
    fThreshold = StringToDouble( GetParameter( "threshold", 5 ) );
    fNPointsOverThreshold = StringToInteger( GetParameter( "pointsOverThreshold", 5 ) );
    fMinPeakAmplitude = StringToDouble( GetParameter( "minPeakAmplitude", 50 ) );

    fMeanBaseLineCutRange = StringTo2DVector( GetParameter( "meanBaseLineCutRange", "(0,4096)") );
    fMeanBaseLineSigmaCutRange = StringTo2DVector( GetParameter( "meanBaseLineSigmaCutRange", "(0,4096)") );
    fMaxNumberOfSignalsCut = StringTo2DVector( GetParameter( "maxNumberOfSignalsCut", "(0,20)" ) );
    fMaxNumberOfGoodSignalsCut = StringTo2DVector( GetParameter( "maxNumberOfGoodSignalsCut", "(0,20)" ) );

    fFullIntegralCut  = StringTo2DVector( GetParameter( "fullIntegralCut", "(0,100000)" ) );
    fThresholdIntegralCut  = StringTo2DVector( GetParameter( "thresholdIntegralCut", "(0,100000)" ) );

    fPeakTimeDelayCut  = StringTo2DVector( GetParameter( "peakTimeDelayCut", "(0,20)" ) );
    if( GetParameter( "cutsEnabled" ) == "true" ) fCutsEnabled = true;


}

