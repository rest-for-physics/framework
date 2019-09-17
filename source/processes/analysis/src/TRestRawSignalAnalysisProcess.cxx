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
///
/// ### Observables
///
/// Number of signals and base line:
///
/// * **NumberOfSignals**: Number of pulses recorded in an event.
/// * **NumberOfGoodSignals**: Number of pulses recorded in an event that gets
/// over the threshold. It counts the signal if GetIntegralWithThreshold (fSignal) > 0.
/// * **BaseLineMean**: Average of the base line of the pulses of the event.
/// * **BaseLineSigmaMean**: Average of the standard deviation from the base line in
/// the pulses of the event.
///
/// Integrals and energy estimations:
///
/// * **FullIntegral**: Add the integral of all pulses of the event. The integral of
/// a pulse is the sum of all samples.
/// * **TripleMaxIntegral**: Add the highest value of each pulse with the previous
/// sample and the next, and then add this amount for all pulses in the event. It
/// is an estimation of the deposited energy.
/// * **ThresholdIntegral**: Add the integral of all the pulses in the event that pass
/// the threshold.
/// * **SlopeIntegral**: Add the integral of the rising part of each pulse of the event.
/// * **RiseSlopeAvg**: Add the SlopeIntegral of all pulses in the event that pass the
/// GetThresholdIntegralValue() > 0 condition and divides it by the number of signals
/// that pass the cut.
/// * **IntegralBalance**: (fullIntegral - thrIntegral) / (fullIntegral + thrIntegral)
/// Balance between FullIntegral and ThresholdIntegral.
/// * **RateOfChangeAvg**: RiseSlopeAvg/SlopeIntegral. The first value takes only the
/// events that pass the threshold cut, SopeIntegral takes all the events.
/// * **xEnergySum**: Add the ThresholdIntegral of all signals recorded in X direction.
/// * **yEnergySum**: Add the ThresholdIntegral of all signals recorded in Y direction.
///
///
/// Time observables:
///
/// * **SecondsFromStart**: It takes the time of the event and subtracts the time of
/// the first event.
/// * **HoursFromStart**: SecondsFromStart divided by 3600.
/// * **EventTimeDelay**: It counts the time from the previous event to the present one.
/// * **MeanRate_InHz**: It records the mean rate of the last 100 events. It divides
/// 100 by the time in seconds between the first and the last.
/// * **TimeBinsLength**: MaxTime of the event - MinTime of the event. The functions
/// GetMaxTime() and GetMinTime() take the number of points of the signal (MinTime=0
/// MaxTime=fSignal[0].GetNumberOfPoints()).
/// * **RiseTimeAvg**: Add GetRiseTime(fSignal) for all signals that satisfy the
/// GetThresholdIntegralValue() > 0 condition and divide it by the number of signals
/// that pass this cut. GetRiseTime(fSignal) provides the number of bins between the
/// fist sample that pass the threshold and the maximum of the peak.
///
/// Peak amplitude observables:
///
/// * **AmplitudeIntegralRatio**: ThresholdIntegral/maxValueIntegral. This is the sum
/// of the integral of all the pulses in the event that pass the threshold divided by
/// the sum of the maximum point of these pulses.
/// * **MinPeakAmplitude**: Minimum value between the maximum points of the pulses that
/// pass the threshold in the event.
/// * **MaxPeakAmplitude**: Maximum value between the maximum points of the pulses that
/// pass the threshold in the event.
/// * **PeakAmplitudeIntegral**: Sum of all maximum points of the pulses that pass the
/// threshold in the event.
/// * **AmplitudeRatio**: PeakAmplitudeIntegral/ MaxPeakAmplitude. Sum of all maximum
/// points divided by the maximum between the maximum points of the pulses.
///
/// Peak time observables:
///
/// * **MaxPeakTime**: Highest bin for the maximum point of a pulse that pass the
/// threshold in the event.
/// * **MinPeakTime**: Smallest bin for the maximum point of a pulse that pass the
/// threshold in the event.
/// * **MaxPeakTimeDelay**: MaxPeakTime-MinPeakTime. Time between the earliest peak and
/// the latest peak between the pulses that pass the threshold in the event.
/// * **AveragePeakTime**: For all pulses that pass the threshold, add the bin of their
/// maximums and divide this amount by the number of signals that pass the threshold.
///
/// Observables for individual signal info:
///
/// * **risetime**: Map the ID of each signal in the event with its rise time.
/// GetRiseTime() gives the number of bins between the first sample that pass the
/// threshold and the maximum point of the peak.
/// * **risetimemean**: Add the rise time of all pulses in the event and divide it by
/// the number of pulses.
/// * **baseline**: Map the ID of each signal in the event with its base line. It
/// computes the base line adding the samples in a certain range and dividing it by
/// the number of samples.
/// * **baselinemean**: Add the base line of all pulses in the event and divide it by
/// the number of pulses.
/// * **baselinesigma**: Map the ID of each signal in the event with its base line's
/// standard deviation. Standard deviation computed as the squared root of the sum of
/// the squared differences between the samples and the base line, divided by the number
/// of samples in the range used to compute the base line.
/// * **baselinesigmamean**: Add the base line's standard deviation of all pulses in
/// the event and divide it by the number of pulses.
/// * **ampsgn_maxmethod**: Map the ID of each signal in the event with the amplitude
/// of its peak. Amplitude = MaxPeakValue-BaseLine.
/// * **ampeve_maxmethod**: Add the amplitude of all pulses in the event.
/// Amplitude = MaxPeakValue-BaseLine.
/// * **ampsgn_intmethod**: Map the ID of each signal in the event with the threshold
/// integral of its peak. GetIntegralWithThreshold() adds the samples of a pulse
/// that get over the threshold. A certain number of samples must pass the threshold
/// to be taken into account.
/// * **ampeve_intmethod**: Add the threshold integral of all pulses in the event.
/// GetIntegralWithThreshold () adds the samples of a pulse that get over the threshold.
/// A certain number of samples must pass the threshold to be taken into account.
///
///
///
/// List of vailable cuts: (obsolete)
///
/// * **MeanBaseLineCut**
/// * **MeanBaseLineSigmaCut**
/// * **MaxNumberOfSignalsCut**
/// * **MaxNumberOfGoodSignalsCut**
/// * **FullIntegralCut**
/// * **ThresholdIntegralCut**
/// * **PeakTimeDelayCut**
/// * **ADCSaturationCut**
///
///  To add cut, write "cut" sections in your rml file:
///
/// \code
/// <TRestRawSignalAnalysisProcess name=""  ... >
///     <parameter name="cutsEnabled" value="true" />
///     <cut name="MeanBaseLineCut" value="(0,4096)" />
/// </TRestRawSignalAnalysisProcess>
/// \endcode
///
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-February: First implementation of raw signal analysis process into REST_v2.
///                Created from TRestSignalAnalysisProcess
///
/// \class      TRestRawSignalAnalysisProcess
/// \author     Javier Galan
///
///______________________________________________________________________________
///
//////////////////////////////////////////////////////////////////////////

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestRawSignalAnalysisProcess.h"
using namespace std;

ClassImp(TRestRawSignalAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestRawSignalAnalysisProcess::TRestRawSignalAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalAnalysisProcess::~TRestRawSignalAnalysisProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = new TRestRawSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;

    fFirstEventTime = -1;
    fPreviousEventTime.clear();

    fReadout = NULL;
    fChannelsHisto = NULL;

    time(&timeStored);
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestRawSignalAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestRawSignalAnalysisProcess::InitProcess() {
    fSignalAnalysisObservables = TRestEventProcess::ReadObservables();

    if (!fReadOnly)
        fChannelsHisto = new TH1D("readoutChannelActivity", "readoutChannelActivity", 128, 0, 128);

    fReadout = (TRestReadout*)GetReadoutMetadata();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
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

    // we save some complex typed analysis result
    map<int, Double_t> baseline;
    Double_t baselinemean;
    map<int, Double_t> baselinesigma;
    Double_t baselinesigmamean;
    map<int, Double_t> ampsgn_maxmethod;
    Double_t ampeve_maxmethod;
    map<int, Double_t> ampsgn_intmethod;
    Double_t ampeve_intmethod;
    map<int, Double_t> risetime;
    Double_t risetimemean;

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

        Double_t _bslval = sgnl->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
        Double_t _bslsigma = sgnl->GetBaseLineSigma(fBaseLineRange.X(), fBaseLineRange.Y(), _bslval);
        Double_t _ampmax = sgnl->GetMaxPeakValue() - _bslval;
        Double_t _ampint = sgnl->GetIntegralWithThreshold(
            (Int_t)fIntegralRange.X(), (Int_t)fIntegralRange.Y(), fBaseLineRange.X(), fBaseLineRange.Y(),
            fPointThreshold, fNPointsOverThreshold, fSignalThreshold);
        Double_t _risetime = (sgnl->GetPointsOverThreshold().size() < 2) ? 0 : sgnl->GetRiseTime();

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

    SetObservableValue("risetime", risetime);
    SetObservableValue("risetimemean", risetimemean);
    SetObservableValue("baseline", baseline);
    SetObservableValue("baselinemean", baselinemean);
    SetObservableValue("baselinesigma", baselinesigma);
    SetObservableValue("baselinesigmamean", baselinesigmamean);
    SetObservableValue("ampsgn_maxmethod", ampsgn_maxmethod);
    SetObservableValue("ampeve_maxmethod", ampeve_maxmethod);
    SetObservableValue("ampsgn_intmethod", ampsgn_intmethod);
    SetObservableValue("ampeve_intmethod", ampeve_intmethod);

    if (fFirstEventTime == -1) fFirstEventTime = fSignalEvent->GetTime();

    Double_t secondsFromStart = fSignalEvent->GetTime() - fFirstEventTime;
    SetObservableValue("SecondsFromStart", secondsFromStart);
    SetObservableValue("HoursFromStart", secondsFromStart / 3600.);

    Double_t evTimeDelay = 0;
    if (fPreviousEventTime.size() > 0) evTimeDelay = fSignalEvent->GetTime() - fPreviousEventTime.back();
    SetObservableValue("EventTimeDelay", evTimeDelay);

    Double_t meanRate = 0;
    if (fPreviousEventTime.size() == 100)
        meanRate = 100. / (fSignalEvent->GetTime() - fPreviousEventTime.front());
    SetObservableValue("MeanRate_InHz", meanRate);

    Double_t baseLineMean = fSignalEvent->GetBaseLineAverage(fBaseLineRange.X(), fBaseLineRange.Y());
    SetObservableValue("BaseLineMean", baseLineMean);

    Double_t baseLineSigma = fSignalEvent->GetBaseLineSigmaAverage(fBaseLineRange.X(), fBaseLineRange.Y());
    SetObservableValue("BaseLineSigmaMean", baseLineSigma);

    Double_t timeDelay = fSignalEvent->GetMaxTime() - fSignalEvent->GetMinTime();
    SetObservableValue("TimeBinsLength", timeDelay);

    Double_t nSignals = fSignalEvent->GetNumberOfSignals();
    SetObservableValue("NumberOfSignals", nSignals);

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
    SetObservableValue("FullIntegral", fullIntegral);

    Double_t thrIntegral =
        fSignalEvent->GetIntegralWithThreshold(from, to, fBaseLineRange.X(), fBaseLineRange.Y(),
                                               fPointThreshold, fNPointsOverThreshold, fSignalThreshold);
    SetObservableValue("ThresholdIntegral", thrIntegral);

    Double_t riseSlope = fSignalEvent->GetRiseSlope();
    SetObservableValue("RiseSlopeAvg", riseSlope);

    Double_t slopeIntegral = fSignalEvent->GetSlopeIntegral();
    SetObservableValue("SlopeIntegral", slopeIntegral);

    Double_t rateOfChange = riseSlope / slopeIntegral;
    if (slopeIntegral == 0) rateOfChange = 0;
    SetObservableValue("RateOfChangeAvg", rateOfChange);

    Double_t riseTime = fSignalEvent->GetRiseTime();
    SetObservableValue("RiseTimeAvg", riseTime);

    Double_t tripleMaxIntegral = fSignalEvent->GetTripleMaxIntegral();
    SetObservableValue("TripleMaxIntegral", tripleMaxIntegral);

    Double_t integralRatio = (fullIntegral - thrIntegral) / (fullIntegral + thrIntegral);
    SetObservableValue("IntegralBalance", integralRatio);

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
        if (sgnl->GetIntegralWithThreshold(from, to, fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold,
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
                    Int_t readoutChannel = mod->DaqToReadoutChannel(fSignalEvent->GetSignal(s)->GetID());
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

    SetObservableValue("xEnergySum", xsum);
    SetObservableValue("yEnergySum", ysum);

    if (nGoodSignals > 0) peakTimeAverage /= nGoodSignals;

    Double_t ampIntRatio = thrIntegral / maxValueIntegral;
    if (maxValueIntegral == 0) ampIntRatio = 0;

    SetObservableValue("AmplitudeIntegralRatio", ampIntRatio);
    SetObservableValue("NumberOfGoodSignals", nGoodSignals);
    SetObservableValue("MinPeakAmplitude", minValue);
    SetObservableValue("MaxPeakAmplitude", maxValue);
    SetObservableValue("PeakAmplitudeIntegral", maxValueIntegral);

    Double_t amplitudeRatio = maxValueIntegral / maxValue;
    if (maxValue == 0) amplitudeRatio = 0;

    SetObservableValue("AmplitudeRatio", amplitudeRatio);
    SetObservableValue("MaxPeakTime", maxPeakTime);
    SetObservableValue("MinPeakTime", minPeakTime);

    Double_t peakTimeDelay = maxPeakTime - minPeakTime;

    SetObservableValue("MaxPeakTimeDelay", peakTimeDelay);
    SetObservableValue("AveragePeakTime", peakTimeAverage);

    if (GetVerboseLevel() >= REST_Debug) {
        fAnalysisTree->PrintObservables(this);
    }

    return fSignalEvent;
}

///////////////////////////////////////////////
/// \brief Re-implementation of TRestEventProcess::EndOfEventProcess. This method
/// is in charge of maintain an internal timestamp vector used in the process to
/// determine, i.e., the event rate.
///
void TRestRawSignalAnalysisProcess::EndOfEventProcess(TRestEvent* evInput) {
    TRestEventProcess::EndOfEventProcess(evInput);

    fPreviousEventTime.push_back(fSignalEvent->GetTimeStamp());
    if (fPreviousEventTime.size() > 100) fPreviousEventTime.erase(fPreviousEventTime.begin());
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed. This method will write the channels histogram.
///
void TRestRawSignalAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();

    if (!fReadOnly) fChannelsHisto->Write();
}

/* Commented DrawObservables
TPad* TRestRawSignalAnalysisProcess::DrawObservables() {
    TPad* pad = new TPad("Signal", " ", 0, 0, 1, 1);
    // fDrawingObjects.push_back( (TObject *) pad );
    pad->cd();

    TPaveText* txt = new TPaveText(.05, .1, .95, .8);
    //   fDrawingObjects.push_back( (TObject *) txt );

    txt->AddText(" ");
    for (unsigned int i = 0; i < fSignalAnalysisObservables.size(); i++) {
        Int_t id =
            fAnalysisTree->GetObservableID(this->GetName() + (TString) "" + fSignalAnalysisObservables[i]);
        TString valueStr;
        valueStr.Form(" : %lf", fAnalysisTree->GetObservableValue(id));
        TString sentence = (TString)fSignalAnalysisObservables[i] + valueStr;
        txt->AddText(sentence);
    }
    txt->AddText(" ");

    txt->Draw();

    return pad;
}
*/

///////////////////////////////////////////////
/// \brief Function to read input parameters.
///
void TRestRawSignalAnalysisProcess::InitFromConfigFile() {
    fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
    fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
    fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
    fNPointsOverThreshold = StringToInteger(GetParameter("pointsOverThreshold", "5"));
    fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "5"));
}
