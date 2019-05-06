///______________________________________________________________________________
///______________________________________________________________________________
///
///
///  RESTSoft : Software for Rare Event Searches with TPCs
///
///  TRestRawSignalAnalysisProcess.cxx
///
///  List of vailable cuts:
///
///  MeanBaseLineCut
///  MeanBaseLineSigmaCut
///  MaxNumberOfSignalsCut
///  MaxNumberOfGoodSignalsCut
///  FullIntegralCut
///  ThresholdIntegralCut
///  PeakTimeDelayCut
///  ADCSaturationCut
///
///  To add cut, write "cut" sections in your rml file:
///
/// \code
/// <TRestRawSignalAnalysisProcess name=""  ... >
///     <parameter name="cutsEnabled" value="true" />
///     <cut name="MeanBaseLineCut" value="(0,4096)" />
/// </TRestRawSignalAnalysisProcess>
///
///  First implementation of raw signal analysis process into REST_v2
///  Created from TRestSignalAnalysisProcess
///  Date : feb/2017
///  Author : J. Galan
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestRawSignalAnalysisProcess.h"
using namespace std;

ClassImp(TRestRawSignalAnalysisProcess)
    //______________________________________________________________________________
    TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess() {
  Initialize();
}

//______________________________________________________________________________
TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess(
    char* cfgFileName) {
  Initialize();

  if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestRawSignalAnalysisProcess::~TRestRawSignalAnalysisProcess() {}

void TRestRawSignalAnalysisProcess::LoadDefaultConfig() {
  SetTitle("Default config");
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::Initialize() {
  SetSectionName(this->ClassName());

  fSignalEvent = new TRestRawSignalEvent();

  fOutputEvent = fSignalEvent;
  fInputEvent = fSignalEvent;

  // fCutsEnabled = false;
  fFirstEventTime = -1;
  fPreviousEventTime.clear();

  fDrawRefresh = 0;
  // fCanvas = NULL;
  fReadout = NULL;
  fChannelsHisto = NULL;

  time(&timeStored);
}

void TRestRawSignalAnalysisProcess::LoadConfig(std::string cfgFilename,
                                               std::string name) {
  if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::InitProcess() {
  fSignalAnalysisObservables = TRestEventProcess::ReadObservables();

  if (!fReadOnly)
    fChannelsHisto = new TH1D("readoutChannelActivity",
                              "readoutChannelActivity", 128, 0, 128);

  fReadout = (TRestReadout*)GetReadoutMetadata();
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::BeginOfEventProcess() {}

//______________________________________________________________________________
TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
  TString obsName;

  // no need for verbose copy now
  fSignalEvent = (TRestRawSignalEvent*)evInput;
  fOutputEvent = fSignalEvent;

  ///////////////////previous usage/////////////////////////
  // fSignalEvent->Initialize();
  // TRestRawSignalEvent *fInputSignalEvent = (TRestRawSignalEvent *)evInput;
  // fSignalEvent->SetID(fInputSignalEvent->GetID());
  // fSignalEvent->SetSubID(fInputSignalEvent->GetSubID());
  // fSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());
  // fSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());

  ////for( int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++
  ///)
  // Int_t N = fInputSignalEvent->GetNumberOfSignals();
  // if (GetVerboseLevel() >= REST_Debug) N = 1;
  // for (int sgnl = 0; sgnl < N; sgnl++)
  //	fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));
  ////////////////////////////////////////////

  // we save some analysis result to class's datamember
  baseline.clear();
  baselinesigma.clear();
  ampsgn_maxmethod.clear();
  ampsgn_intmethod.clear();
  risetime.clear();

  baselinemean = 0;
  baselinesigmamean = 0;
  ampeve_intmethod = 0;
  ampeve_maxmethod = 0;
  risetimemean = 0;
  Double_t maxeve = 0;
  for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
    TRestRawSignal* sgnl = fSignalEvent->GetSignal(s);

    Double_t _bslval =
        sgnl->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
    Double_t _bslsigma =
        sgnl->GetBaseLineSigma(fBaseLineRange.X(), fBaseLineRange.Y(), _bslval);
    Double_t _ampmax = sgnl->GetMaxPeakValue() - _bslval;
    Double_t _ampint = sgnl->GetIntegralWithThreshold(
        (Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y(),
        fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold,
        fNPointsOverThreshold, fSignalThreshold);
    Double_t _risetime =
        (sgnl->GetPointsOverThreshold().size() < 2) ? 0 : sgnl->GetRiseTime();

    baseline[sgnl->GetID()] = (_bslval);
    baselinesigma[sgnl->GetID()] = (_bslsigma);
    ampsgn_intmethod[sgnl->GetID()] = (_ampint);
    ampsgn_maxmethod[sgnl->GetID()] = (_ampmax);
    risetime[sgnl->GetID()] = (_risetime);

    baselinemean += _bslval;
    baselinesigmamean += _bslsigma;
    ampeve_intmethod += _ampint;
    ampeve_maxmethod += _ampmax;
    risetimemean += _risetime;

    Double_t value = sgnl->GetMaxValue();
    if (value > maxeve) maxeve = value;
  }
  baselinemean /= fSignalEvent->GetNumberOfSignals();
  baselinesigmamean /= fSignalEvent->GetNumberOfSignals();
  risetimemean /= fSignalEvent->GetNumberOfSignals();

  if (fFirstEventTime == -1) fFirstEventTime = fSignalEvent->GetTime();

  Double_t secondsFromStart = fSignalEvent->GetTime() - fFirstEventTime;
  obsName = this->GetName() + (TString) "_SecondsFromStart";
  fAnalysisTree->SetObservableValue(obsName, secondsFromStart);

  obsName = this->GetName() + (TString) "_HoursFromStart";
  fAnalysisTree->SetObservableValue(obsName, secondsFromStart / 3600.);

  Double_t evTimeDelay = 0;
  if (fPreviousEventTime.size() > 0)
    evTimeDelay = fSignalEvent->GetTime() - fPreviousEventTime.back();
  obsName = this->GetName() + (TString) "_EventTimeDelay";
  fAnalysisTree->SetObservableValue(obsName, evTimeDelay);

  Double_t meanRate = 0;
  if (fPreviousEventTime.size() == 100)
    meanRate = 100. / (fSignalEvent->GetTime() - fPreviousEventTime.front());

  obsName = this->GetName() + (TString) "_MeanRate_InHz";
  fAnalysisTree->SetObservableValue(obsName, meanRate);

  Double_t baseLineMean =
      fSignalEvent->GetBaseLineAverage(fBaseLineRange.X(), fBaseLineRange.Y());

  obsName = this->GetName() + (TString) "_BaseLineMean";
  fAnalysisTree->SetObservableValue(obsName, baseLineMean);

  Double_t baseLineSigma = fSignalEvent->GetBaseLineSigmaAverage(
      fBaseLineRange.X(), fBaseLineRange.Y());

  obsName = this->GetName() + (TString) "_BaseLineSigmaMean";
  fAnalysisTree->SetObservableValue(obsName, baseLineSigma);

  obsName = this->GetName() + (TString) "_TimeBinsLength";
  Double_t timeDelay = fSignalEvent->GetMaxTime() - fSignalEvent->GetMinTime();
  fAnalysisTree->SetObservableValue(obsName, timeDelay);

  obsName = this->GetName() + (TString) "_NumberOfSignals";
  Double_t nSignals = fSignalEvent->GetNumberOfSignals();
  fAnalysisTree->SetObservableValue(obsName, nSignals);

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // SubstractBaselines
  // After this the signal gets zero-ed, for the following analysis
  // Keep in mind, to add raw signal analysis, we must write code at before
  // This is where most of the problems occur
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  fSignalEvent->SubstractBaselines(fBaseLineRange.X(), fBaseLineRange.Y());

  Int_t from = (Int_t)fIntegralRange.X();
  Int_t to = (Int_t)fIntegralRange.Y();
  Double_t fullIntegral = fSignalEvent->GetIntegral(from, to);
  obsName = this->GetName() + (TString) "_FullIntegral";
  fAnalysisTree->SetObservableValue(obsName, fullIntegral);

  Double_t thrIntegral = fSignalEvent->GetIntegralWithThreshold(
      from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold,
      fNPointsOverThreshold, fSignalThreshold);
  obsName = this->GetName() + (TString) "_ThresholdIntegral";
  fAnalysisTree->SetObservableValue(obsName, thrIntegral);

  Double_t riseSlope = fSignalEvent->GetRiseSlope();
  obsName = this->GetName() + (TString) "_RiseSlopeAvg";
  fAnalysisTree->SetObservableValue(obsName, riseSlope);

  Double_t slopeIntegral = fSignalEvent->GetSlopeIntegral();
  obsName = this->GetName() + (TString) "_SlopeIntegral";
  fAnalysisTree->SetObservableValue(obsName, slopeIntegral);

  Double_t rateOfChange = riseSlope / slopeIntegral;
  if (slopeIntegral == 0) rateOfChange = 0;
  obsName = this->GetName() + (TString) "_RateOfChangeAvg";
  fAnalysisTree->SetObservableValue(obsName, rateOfChange);

  Double_t riseTime = fSignalEvent->GetRiseTime();
  obsName = this->GetName() + (TString) "_RiseTimeAvg";
  fAnalysisTree->SetObservableValue(obsName, riseTime);

  Double_t tripleMaxIntegral = fSignalEvent->GetTripleMaxIntegral();
  obsName = this->GetName() + (TString) "_TripleMaxIntegral";
  fAnalysisTree->SetObservableValue(obsName, tripleMaxIntegral);

  Double_t integralRatio =
      (fullIntegral - thrIntegral) / (fullIntegral + thrIntegral);
  obsName = this->GetName() + (TString) "_IntegralBalance";
  fAnalysisTree->SetObservableValue(obsName, integralRatio);

  Double_t maxValue = 0;
  Double_t minValue = 1.e6;
  Double_t maxValueIntegral = 0;

  Double_t minPeakTime = 1000;  // TODO sustitute this for something better
  Double_t maxPeakTime = 0;
  Double_t peakTimeAverage = 0;

  Int_t nGoodSignals = 0;
  int xsum = 0;
  int ysum = 0;
  for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
    TRestRawSignal* sgnl = fSignalEvent->GetSignal(s);
    if (sgnl->GetIntegralWithThreshold(
            from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold,
            fNPointsOverThreshold, fSignalThreshold) > 0) {
      Double_t value = fSignalEvent->GetSignal(s)->GetMaxValue();
      if (value > maxValue) maxValue = value;
      if (value < minValue) minValue = value;
      maxValueIntegral += value;

      Double_t peakBin = sgnl->GetMaxPeakBin();
      peakTimeAverage += peakBin;

      if (minPeakTime > peakBin) minPeakTime = peakBin;
      if (maxPeakTime < peakBin) maxPeakTime = peakBin;

      nGoodSignals++;

      // Adding signal to the channel activity histogram
      if (!fReadOnly && fReadout != NULL) {
        TRestReadoutModule* mod = &(*fReadout)[0][0];
        for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
          Int_t readoutChannel =
              mod->DaqToReadoutChannel(fSignalEvent->GetSignal(s)->GetID());
          fChannelsHisto->Fill(readoutChannel);
        }

        auto x = fReadout->GetX(sgnl->GetID());
        auto y = fReadout->GetY(sgnl->GetID());

        if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
          if (!TMath::IsNaN(x)) {
            xsum += sgnl->fThresholdIntegral;
          } else if (!TMath::IsNaN(y)) {
            ysum += sgnl->fThresholdIntegral;
          }
        }
      }
    }
  }

  this->SetObservableValue("xEnergySum", xsum);
  this->SetObservableValue("yEnergySum", ysum);

  obsName = this->GetName() + (TString) "_MinPeakTime";
  fAnalysisTree->SetObservableValue(obsName, minPeakTime);

  if (nGoodSignals > 0) peakTimeAverage /= nGoodSignals;

  Double_t ampIntRatio = thrIntegral / maxValueIntegral;
  if (maxValueIntegral == 0) ampIntRatio = 0;
  obsName = this->GetName() + (TString) "_AmplitudeIntegralRatio";
  fAnalysisTree->SetObservableValue(obsName, ampIntRatio);

  obsName = this->GetName() + (TString) "_NumberOfGoodSignals";
  fAnalysisTree->SetObservableValue(obsName, nGoodSignals);

  obsName = this->GetName() + (TString) "_MinPeakAmplitude";
  fAnalysisTree->SetObservableValue(obsName, minValue);

  obsName = this->GetName() + (TString) "_MaxPeakAmplitude";
  fAnalysisTree->SetObservableValue(obsName, maxValue);

  obsName = this->GetName() + (TString) "_PeakAmplitudeIntegral";
  fAnalysisTree->SetObservableValue(obsName, maxValueIntegral);

  Double_t amplitudeRatio = maxValueIntegral / maxValue;
  if (maxValue == 0) amplitudeRatio = 0;
  obsName = this->GetName() + (TString) "_AmplitudeRatio";
  fAnalysisTree->SetObservableValue(obsName, amplitudeRatio);

  obsName = this->GetName() + (TString) "_MaxPeakTime";
  fAnalysisTree->SetObservableValue(obsName, maxPeakTime);

  obsName = this->GetName() + (TString) "_MinPeakTime";
  fAnalysisTree->SetObservableValue(obsName, minPeakTime);

  Double_t peakTimeDelay = maxPeakTime - minPeakTime;
  obsName = this->GetName() + (TString) "_MaxPeakTimeDelay";
  fAnalysisTree->SetObservableValue(obsName, peakTimeDelay);

  obsName = this->GetName() + (TString) "_AveragePeakTime";
  fAnalysisTree->SetObservableValue(obsName, peakTimeAverage);

  // Cuts
  if (fCuts.size() > 0) {
    auto iter = fCuts.begin();
    while (iter != fCuts.end()) {
      if (iter->first == "meanBaseLineCut")
        if (baseLineMean > iter->second.Y() || baseLineMean < iter->second.X())
          return NULL;
      if (iter->first == "meanBaseLineSigmaCut")
        if (baseLineSigma > iter->second.Y() ||
            baseLineSigma < iter->second.X())
          return NULL;
      if (iter->first == "maxNumberOfSignalsCut")
        if (nSignals > iter->second.Y() || nSignals < iter->second.X())
          return NULL;
      if (iter->first == "maxNumberOfGoodSignalsCut")
        if (nGoodSignals > iter->second.Y() || nGoodSignals < iter->second.X())
          return NULL;
      if (iter->first == "fullIntegralCut")
        if (fullIntegral > iter->second.Y() || fullIntegral < iter->second.X())
          return NULL;
      if (iter->first == "thresholdIntegralCut")
        if (thrIntegral > iter->second.Y() || thrIntegral < iter->second.X())
          return NULL;
      if (iter->first == "peakTimeDelayCut")
        if (peakTimeDelay > iter->second.Y() ||
            peakTimeDelay < iter->second.X())
          return NULL;
      if (iter->first == "ADCSaturationCut")
        if (maxeve > iter->second.Y() || maxeve < iter->second.X()) return NULL;

      iter++;
    }

    // if (nSignals < fMaxNumberOfSignalsCut.X() || nSignals >
    // fMaxNumberOfSignalsCut.Y())  return NULL; if (nGoodSignals <
    // fMaxNumberOfGoodSignalsCut.X() || nGoodSignals >
    // fMaxNumberOfGoodSignalsCut.Y()) return NULL; if (thrIntegral <
    // fThresholdIntegralCut.X() || thrIntegral > fThresholdIntegralCut.Y())
    // return NULL; if (fullIntegral < fFullIntegralCut.X() || fullIntegral >
    // fFullIntegralCut.Y()) return NULL; if (peakTimeDelay <
    // fPeakTimeDelayCut.X() || peakTimeDelay > fPeakTimeDelayCut.Y()) return
    // NULL;
  }

  if (GetVerboseLevel() >= REST_Debug) {
    fAnalysisTree->PrintObservables(this);
  }

  return fSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::EndOfEventProcess() {
  fPreviousEventTime.push_back(fSignalEvent->GetTimeStamp());
  if (fPreviousEventTime.size() > 100)
    fPreviousEventTime.erase(fPreviousEventTime.begin());
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::EndProcess() {
  // Function to be executed once at the end of the process
  // (after all events have been processed)

  // Start by calling the EndProcess function of the abstract class.
  // Comment this if you don't want it.
  // TRestEventProcess::EndProcess();

  if (!fReadOnly) fChannelsHisto->Write();
}

TPad* TRestRawSignalAnalysisProcess::DrawObservables() {
  TPad* pad = new TPad("Signal", " ", 0, 0, 1, 1);
  // fDrawingObjects.push_back( (TObject *) pad );
  pad->cd();

  TPaveText* txt = new TPaveText(.05, .1, .95, .8);
  //   fDrawingObjects.push_back( (TObject *) txt );

  txt->AddText(" ");
  for (unsigned int i = 0; i < fSignalAnalysisObservables.size(); i++) {
    Int_t id = fAnalysisTree->GetObservableID(this->GetName() + (TString) "_" +
                                              fSignalAnalysisObservables[i]);
    TString valueStr;
    valueStr.Form(" : %lf", fAnalysisTree->GetObservableValue(id));
    TString sentence = (TString)fSignalAnalysisObservables[i] + valueStr;
    txt->AddText(sentence);
  }
  txt->AddText(" ");

  txt->Draw();

  return pad;
}

TPad* TRestRawSignalAnalysisProcess::DrawSignal(Int_t signal) {
  TPad* pad = new TPad("Signal", " ", 0, 0, 1, 1);
  pad->cd();

  fDrawingObjects.push_back((TObject*)pad);

  TGraph* gr = new TGraph();
  fDrawingObjects.push_back((TObject*)gr);

  TRestRawSignal* sgnl = fSignalEvent->GetSignal(signal);

  for (int n = 0; n < sgnl->GetNumberOfPoints(); n++)
    gr->SetPoint(n, n, sgnl->GetData(n));

  gr->Draw("AC*");

  TGraph* gr2 = new TGraph();
  fDrawingObjects.push_back((TObject*)gr2);

  gr2->SetLineWidth(2);
  gr2->SetLineColor(2);

  for (int n = fBaseLineRange.X(); n < fBaseLineRange.Y(); n++)
    gr2->SetPoint(n - fBaseLineRange.X(), n, sgnl->GetData(n));

  gr2->Draw("CP");

  vector<Int_t> pOver = sgnl->GetPointsOverThreshold();

  TGraph* gr3[5];
  Int_t nGraphs = 0;
  gr3[nGraphs] = new TGraph();
  fDrawingObjects.push_back((TObject*)gr3[nGraphs]);
  gr3[nGraphs]->SetLineWidth(2);
  gr3[nGraphs]->SetLineColor(3);
  Int_t point = 0;
  Int_t nPoints = pOver.size();
  for (int n = 0; n < nPoints; n++) {
    gr3[nGraphs]->SetPoint(point, pOver[n], sgnl->GetData(pOver[n]));
    point++;
    if (n + 1 < nPoints && pOver[n + 1] - pOver[n] > 1) {
      gr3[nGraphs]->Draw("CP");
      nGraphs++;
      if (nGraphs > 4) cout << "Ngraphs : " << nGraphs << endl;
      point = 0;
      gr3[nGraphs] = new TGraph();
      fDrawingObjects.push_back((TObject*)gr3[nGraphs]);
      gr3[nGraphs]->SetLineWidth(2);
      gr3[nGraphs]->SetLineColor(3);
    }
  }

  if (nPoints > 0) gr3[nGraphs]->Draw("CP");

  /*
  TLegend *leg = new TLegend(.6,.7,.9,.9);
  fDrawingObjects.push_back( (TObject *) leg );
  leg->AddEntry( gr2, (TString) "Baseline" );
  leg->Draw("same");
  */

  return pad;
}

//______________________________________________________________________________
void TRestRawSignalAnalysisProcess::InitFromConfigFile() {
  fDrawRefresh = StringToDouble(GetParameter("refreshEvery", "0"));

  fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
  fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
  fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
  fNPointsOverThreshold =
      StringToInteger(GetParameter("pointsOverThreshold", "5"));
  fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "5"));

  /*fMeanBaseLineCutRange = StringTo2DVector( GetParameter(
"meanBaseLineCutRange", "(0,4096)") ); fMeanBaseLineSigmaCutRange =
StringTo2DVector( GetParameter( "meanBaseLineSigmaCutRange", "(0,4096)") );
fMaxNumberOfSignalsCut = StringTo2DVector( GetParameter(
"maxNumberOfSignalsCut", "(0,20)" ) ); fMaxNumberOfGoodSignalsCut =
StringTo2DVector( GetParameter( "maxNumberOfGoodSignalsCut", "(0,20)" ) );

fFullIntegralCut  = StringTo2DVector( GetParameter( "fullIntegralCut",
"(0,100000)" ) ); fThresholdIntegralCut  = StringTo2DVector( GetParameter(
"thresholdIntegralCut", "(0,100000)" ) );

fPeakTimeDelayCut  = StringTo2DVector( GetParameter( "peakTimeDelayCut",
"(0,20)" ) ); if( GetParameter( "cutsEnabled", "false" ) == "true" )
fCutsEnabled = true;*/
}
