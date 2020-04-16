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
/// THIS DOCUMENTATION NEEDS REVISION
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

    fSignalEvent = NULL;

    fFirstEventTime = -1;
    fPreviousEventTime.clear();

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
    // fSignalAnalysisObservables = TRestEventProcess::ReadObservables();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    // no need for verbose copy now
    fSignalEvent = (TRestRawSignalEvent*)evInput;

    ///////////////////previous usage/////////////////////////
    // fSignalEvent->Initialize();
    // TRestRawSignalEvent *fInputSignalEvent = (TRestRawSignalEvent *)evInput;
    // fSignalEvent->SetID(fInputSignalEvent->GetID());
    // fSignalEvent->SetSubID(fInputSignalEvent->GetSubID());
    // fSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());
    // fSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());

    // Int_t N = fInputSignalEvent->GetNumberOfSignals();
    // if (GetVerboseLevel() >= REST_Debug) N = 1;
    // for (int sgnl = 0; sgnl < N; sgnl++)
    //	fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));
    ////////////////////////////////////////////

    // we save some complex typed analysis result
    map<int, Double_t> baseline;
    // Double_t baselinemean;
    map<int, Double_t> baselinesigma;
    // Double_t baselinesigmamean;
    map<int, Double_t> ampsgn_maxmethod;
    // Double_t ampeve_maxmethod;
    map<int, Double_t> ampsgn_intmethod;
    // Double_t ampeve_intmethod;
    map<int, Double_t> risetime;
    // Double_t risetimemean;

    baseline.clear();
    baselinesigma.clear();
    ampsgn_maxmethod.clear();
    ampsgn_intmethod.clear();
    risetime.clear();

    /*
baselinemean = 0;
baselinesigmamean = 0;
ampeve_intmethod = 0;
ampeve_maxmethod = 0;
risetimemean = 0;
Double_t maxeve = 0;
    */
    Int_t nGoodSignals = 0;

    /// We define (or re-define) the baseline range and calculation range of our raw-signals.
    // This will affect the calculation of observables, but not the stored TRestRawSignal data.
    fSignalEvent->SetBaseLineRange(fBaseLineRange);
    fSignalEvent->SetRange(fIntegralRange);

    for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
        TRestRawSignal* sgnl = fSignalEvent->GetSignal(s);

        /// Important call we need to initialize the points over threshold in a TRestRawSignal
        sgnl->InitializePointsOverThreshold(TVector2(fPointThreshold, fSignalThreshold),
                                            fNPointsOverThreshold);

        // We do not want that signals that are not identified as such contribute to define our
        // observables
        if (sgnl->GetPointsOverThreshold().size() < 2) continue;

        nGoodSignals++;

        /* We skip now intermediate variables
Double_t _bslval = sgnl->GetBaseLine();
Double_t _bslsigma = sgnl->GetBaseLineSigma();
Double_t _ampmax = sgnl->GetMaxPeakValue();
Double_t _ampint = sgnl->GetThresholdIntegral();
Double_t _risetime = sgnl->GetRiseTime(); */

        // Now TRestRawSignal returns directly baseline substracted values
        baseline[sgnl->GetID()] = sgnl->GetBaseLine();
        baselinesigma[sgnl->GetID()] = sgnl->GetBaseLineSigma();
        ampsgn_intmethod[sgnl->GetID()] = sgnl->GetThresholdIntegral();
        ampsgn_maxmethod[sgnl->GetID()] = sgnl->GetMaxPeakValue();
        risetime[sgnl->GetID()] = sgnl->GetRiseTime();

        /* These observables were already being calculated later on
baselinemean += sgnl->GetBaseLine();
baselinesigmamean += sgnl->GetBaseLineSigma();
ampeve_intmethod += sgnl->GetThresholdIntegral();
ampeve_maxmethod += sgnl->GetMaxPeakValue();
risetimemean += sgnl->GetRiseTime();
        */

        //  Double_t value = sgnl->GetMaxValue();
        //  if (value > maxeve) maxeve = value; // Not used?
    }

    // baselinemean /= fSignalEvent->GetNumberOfSignals();
    // baselinesigmamean /= fSignalEvent->GetNumberOfSignals();
    // risetimemean /= fSignalEvent->GetNumberOfSignals();

    // I just comment this. It should be cleaned up soon.
    // The only new observables that remain are map variables.
    // We can adopt lower case naming for map variables. But Double_t variables naming convention was already
    // fixed. In future case insensitive?
    SetObservableValue("risetime_map", risetime);
    // SetObservableValue("risetimemean", risetimemean); // Repeated observable : RiseTimeAvg
    SetObservableValue("baseline_map", baseline);
    // SetObservableValue("baselinemean", baselinemean); // Repeated observable: BaseLineMean
    SetObservableValue("baselinesigma_map", baselinesigma);
    // SetObservableValue("baselinesigmamean", baselinesigmamean);  //Repeated observable: BaseLineSigmaMean

    // A name like : max_amplitude (or max_amplitude_map) would be a name more straight forward to understand
    SetObservableValue("max_amplitude_map", ampsgn_maxmethod);

    // SetObservableValue("ampeve_maxmethod", ampeve_maxmethod); // Repeated : MaxPeakAmplitudeIntegral

    // This observable is a map of the threshold integral of each pulse. The given name is not intuitive to
    // me. It should be just something like : thr_integral  ---> then lower case tells me that is a map. If
    // not we should use some convention like : thr_integral_map
    SetObservableValue("thr_integral_map", ampsgn_intmethod);

    //   SetObservableValue("ampeve_intmethod", ampeve_intmethod);  // Repeated observable : ThresholdIntegral

    /* /////////////////////////////////////////////////////////////////////////////////// */
    // These observables should probably go into an independent process to register the rate at any
    // given moment of the data processing chain : TRestRateAnalysisProcess? */
    if (fFirstEventTime == -1) fFirstEventTime = fSignalEvent->GetTime();

    Double_t secondsFromStart = fSignalEvent->GetTime() - fFirstEventTime;
    SetObservableValue("SecondsFromStart", secondsFromStart);
    SetObservableValue("HoursFromStart", secondsFromStart / 3600.);

    Double_t evTimeDelay = 0;
    if (fPreviousEventTime.size() > 0) evTimeDelay = fSignalEvent->GetTime() - fPreviousEventTime.back();
    SetObservableValue("EventTimeDelay", evTimeDelay);

    Double_t meanRate = 0;
    if (fPreviousEventTime.size() == 10)
        meanRate = 10. / (fSignalEvent->GetTime() - fPreviousEventTime.front());
    SetObservableValue("MeanRate_InHz", meanRate);
    /* /////////////////////////////////////////////////////////////////////////////////// */

    Double_t baseLineMean = fSignalEvent->GetBaseLineAverage();
    SetObservableValue("BaseLineMean", baseLineMean);

    Double_t baseLineSigma = fSignalEvent->GetBaseLineSigmaAverage();
    SetObservableValue("BaseLineSigmaMean", baseLineSigma);

    Double_t timeDelay = fSignalEvent->GetMaxTime() - fSignalEvent->GetMinTime();
    SetObservableValue("TimeBinsLength", timeDelay);

    Int_t nSignals = fSignalEvent->GetNumberOfSignals();
    SetObservableValue("NumberOfSignals", nSignals);
    SetObservableValue("NumberOfGoodSignals", nGoodSignals);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // SubstractBaselines
    // After this the signal gets zero-ed, for the following analysis
    // Keep in mind, to add raw signal analysis, we must write code at before
    // This is where most of the problems occur
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Javier: I believe we should not substract baseline in the analysis process then ...
    // ... of course we need to consider baseline substraction for each observable. TRestRawSignal methods
    // should do that internally. I have updated that to be like that, but we need to be with open eyes for
    // some period.
    // Baseline substraction will always happen when we transfer a TRestRawSignal to TRestSignal
    //
    // We do not substract baselines then now, as it was done before
    //
    // fSignalEvent->SubstractBaselines(fBaseLineRange.X(), fBaseLineRange.Y());
    //
    // Methods in TRestRawSignal have been updated to consider baseline.
    // TRestRawSignal now implements that internally. We need to define the baseline range, and the range
    // where calculations take place. All we need is to call at some point to the following methods.
    //
    // TRestRawSignalEvent::SetBaseLineRange and TRestRawSignalEvent::SetRange.
    //
    // Then, if any method accepts a different range it will be given in the method name,
    // for example: GetIntegralInRange( Int_t startBin, Int_t endBin );
    //

    Double_t fullIntegral = fSignalEvent->GetIntegral();
    SetObservableValue("FullIntegral", fullIntegral);

    Double_t thrIntegral = fSignalEvent->GetThresholdIntegral();
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

    for (int s = 0; s < fSignalEvent->GetNumberOfSignals(); s++) {
        TRestRawSignal* sgnl = fSignalEvent->GetSignal(s);
        if (sgnl->GetPointsOverThreshold().size() > 1) {
            Double_t value = fSignalEvent->GetSignal(s)->GetMaxValue();
            maxValueIntegral += value;

            if (value > maxValue) maxValue = value;
            if (value < minValue) minValue = value;

            Double_t peakBin = sgnl->GetMaxPeakBin();
            peakTimeAverage += peakBin;

            if (minPeakTime > peakBin) minPeakTime = peakBin;
            if (maxPeakTime < peakBin) maxPeakTime = peakBin;
        }
    }

    if (nGoodSignals > 0) peakTimeAverage /= nGoodSignals;

    Double_t ampIntRatio = thrIntegral / maxValueIntegral;
    if (maxValueIntegral == 0) ampIntRatio = 0;

    SetObservableValue("AmplitudeIntegralRatio", ampIntRatio);
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

    fPreviousEventTime.push_back(fSignalEvent->GetTimeStamp());
    if (fPreviousEventTime.size() > 10) fPreviousEventTime.erase(fPreviousEventTime.begin());

    // If no good signals are identified the event will be not registered.
    if (nGoodSignals == 0) return NULL;

    return fSignalEvent;
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
