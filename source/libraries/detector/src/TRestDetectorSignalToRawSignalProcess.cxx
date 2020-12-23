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
/// TRestDetectorSignalToRawSignalProcess transforms a
/// TRestDetectorSignalEvent into a TRestRawSignalEvent. The TRestDetectorSignalEvent
/// contains signal data built with arbitrary times and their corresponding
/// data values (time, data). The data inside a TRestRawSignal losses
/// precision on the time definition, and it is just a data array with a
/// fixed number of data points. Therefore, the time between two consecutive
/// data points in a raw signal event must be kept constant.
///
/// This process produces the sampling of a TRestDetectorSignalEvent into a
/// TRestRawSignalEvent. TRestDetectorSignal contains Float_t data values, while
/// TResRawSignal contains Short_t values. Thats why there might be some
/// information loss when transferring the signal data to the raw-signal data.
/// To minimize the impact, the maximum data value of the output signals should
/// be high enough, and adjusted to the maximum value of a Short_t, being
/// this value 32768. The *gain* parameter may serve to re-adjust the
/// amplitude of the output data array.
///
/// \warning If the value assigned to a data point in the output rawsignal
/// event exceeds 32768 it will cause an overflow, and the event data will
/// be corrupted. If the verboseLevel of the process is warning, an output
/// message will prevent the user. The event status will be invalid.
///
/// The input signal contains arbitrary times expressed in microseconds.
/// In order to produce the binning, a time window must be defined. The
/// parameter *triggerMode* will allow to define how we choose the time
/// start (corresponding to the bin 0 in the raw signal), and time end
/// (corresponding to the last bin in the raw signal).
///
/// The trigger mode will fix the time the signal starts, while the
/// *sampling* time parameter (in microseconds) and the number of points
/// per signal, *Npoints*, will fix the time end. A *triggerDelay*
/// parameter allows to shift the time measured in number of samples, from
/// the definition obtained using the *triggerMode* parameter.
///
/// The following list describes the different parameters that can be
/// used in this process.
///
/// * **sampling**: It is the sampling time of the resulting raw signal
/// output data. Time units must be specified (ns, us, ms)".
///
/// * **Npoints**: The number of points of the resulting raw signals.
///
/// * **triggerMode**: It defines how the start time is fixed. The
/// different options are:
///
///   - *firstDeposit*: The first time deposit found in the event
///     will correspond to the bin 0.
///   - *integralThreshold*: An integral window with size **Npoints/2**
///     will start to scan the input signal event from the first time
///     deposit. The time at which the value of this integral is above
///     the value provided at the **integralThreshold** parameter will
///     be defined as the center of the acquisition window.
///
/// * **integralThreshold**: It defines the value to be used in the
///     triggerThreshold method. This parameter is not used otherwise.
///
///
/// \htmlonly <style>div.image img[src="trigger.png"]{width:500px;}</style> \endhtmlonly
///
/// The following figure shows the integralThreshold trigger definition for a NLDBD
/// event. tStart and tEnd define the acquision window, centered on the time
/// when the signal integral is above the threshold defined. tStart has been
/// shifted by a triggerDelay = 60 samples * 200ns
///
/// ![An ilustration of the trigger definition](trigger.png)
///
/// * **triggerDelay**: The time start obtained by the trigger mode
/// definition can be shifted using this parameter. The shift is
/// measured in number of bins from the output signal.
///
/// * **gain**: Each data point from the resulting raw signal will be
/// multiplied by this factor before performing the conversion to
/// Short_t. Each value in the raw output signal should be between
/// -32768 and 32768, resulting event data will be corrupted otherwise.
/// The state of the event will be set to false fOk=false.
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-November: First implementation of signal to rawsignal conversion.
///             Javier Galan
///
/// \class      TRestDetectorSignalToRawSignalProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestDetectorSignalToRawSignalProcess.h"
using namespace std;

ClassImp(TRestDetectorSignalToRawSignalProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorSignalToRawSignalProcess::TRestDetectorSignalToRawSignalProcess() { Initialize(); }

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
TRestDetectorSignalToRawSignalProcess::TRestDetectorSignalToRawSignalProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorSignalToRawSignalProcess::~TRestDetectorSignalToRawSignalProcess() { delete fOutputRawSignalEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorSignalToRawSignalProcess::LoadDefaultConfig() {
    SetName("signalToRawSignalProcess-Default");
    SetTitle("Default config");

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    fSampling = 1;
    fNPoints = 512;
    fTriggerMode = "firstDeposit";
    fTriggerDelay = 50;
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
void TRestDetectorSignalToRawSignalProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestDetectorSignalToRawSignalProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputSignalEvent = NULL;
    fOutputRawSignalEvent = new TRestRawSignalEvent();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorSignalToRawSignalProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestDetectorSignalEvent*)evInput;

    if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;

    if (GetVerboseLevel() >= REST_Debug) fOutputRawSignalEvent->PrintEvent();

    fOutputRawSignalEvent->SetID(fInputSignalEvent->GetID());
    fOutputRawSignalEvent->SetSubID(fInputSignalEvent->GetSubID());
    fOutputRawSignalEvent->SetTimeStamp(fInputSignalEvent->GetTimeStamp());
    fOutputRawSignalEvent->SetSubEventTag(fInputSignalEvent->GetSubEventTag());

    // The time event window is defined (tStart, tEnd)
    Double_t tStart = std::numeric_limits<double>::quiet_NaN();
    Double_t tEnd = 10000;
    if (fTriggerMode == "firstDeposit") {
        tStart = fInputSignalEvent->GetMinTime() - fTriggerDelay * fSampling;
        tEnd = fInputSignalEvent->GetMinTime() + (fNPoints - fTriggerDelay) * fSampling;
    } else if (fTriggerMode == "integralThreshold") {
        Double_t maxT = fInputSignalEvent->GetMaxTime();
        Double_t minT = fInputSignalEvent->GetMinTime();

        for (Double_t t = minT - fNPoints * fSampling; t <= maxT + fNPoints * fSampling; t = t + 0.5) {
            Double_t en = fInputSignalEvent->GetIntegralWithTime(t, t + (fSampling * fNPoints) / 2.);

            if (en > fIntegralThreshold) {
                tStart = t - fTriggerDelay * fSampling;
                tEnd = t + (fNPoints - fTriggerDelay) * fSampling;
            }
        }
    } else {
        if (GetVerboseLevel() >= REST_Warning) {
            cout << "REST WARNING. TRestDetectorSignalToRawSignalProcess. fTriggerMode not "
                    "recognized!"
                 << endl;
            cout << "Setting fTriggerMode to firstDeposit" << endl;
        }

        fTriggerMode = "firstDeposit";

        tStart = fInputSignalEvent->GetMinTime() - fTriggerDelay * fSampling;
        tEnd = fInputSignalEvent->GetMinTime() + (fNPoints - fTriggerDelay) * fSampling;
    }

    if (std::isnan(tStart)) {
        if (GetVerboseLevel() >= REST_Warning) {
            cout << endl;
            cout << "REST WARNING. TRestDetectorSignalToRawSignalProcess. tStart was not "
                    "defined."
                 << endl;
            cout << "Setting tStart = 0. tEnd = fNPoints * fSampling" << endl;
            cout << endl;
        }

        tEnd = fNPoints * fSampling;
    }

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "tStart : " << tStart << " us " << endl;
        cout << "tEnd : " << tEnd << " us " << endl;
    }

    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        Double_t sData[fNPoints];
        for (int i = 0; i < fNPoints; i++) sData[i] = 0;

        TRestDetectorSignal* sgnl = fInputSignalEvent->GetSignal(n);
        Int_t signalID = sgnl->GetSignalID();

        for (int m = 0; m < sgnl->GetNumberOfPoints(); m++) {
            Double_t t = sgnl->GetTime(m);
            Double_t d = sgnl->GetData(m);

            if (GetVerboseLevel() >= REST_Debug && n < 3 && m < 5)
                cout << "Signal : " << n << " Sample : " << m << " T : " << t << " Data : " << d << endl;

            if (t > tStart && t < tEnd) {
                // convert physical time (in us) to timeBin
                Int_t timeBin = (Int_t)round((t - tStart) / fSampling);

                if (GetVerboseLevel() >= REST_Warning)
                    if (timeBin < 0 || timeBin > fNPoints) {
                        cout << "Time bin out of range!!! bin value : " << timeBin << endl;
                        timeBin = 0;
                    }

                debug << "Adding data : " << sgnl->GetData(m) << " to Time Bin : " << timeBin << endl;
                sData[timeBin] += fGain * sgnl->GetData(m);
            }
        }

        if (GetVerboseLevel() >= REST_Warning)
            for (int x = 0; x < fNPoints; x++)
                if (sData[x] < -32768. || sData[x] > 32768.)
                    cout << "REST Warning : data is outside short range : " << sData[x] << endl;

        TRestRawSignal rSgnl;
        rSgnl.SetSignalID(signalID);
        for (int x = 0; x < fNPoints; x++) {
            if (sData[x] < -32768. || sData[x] > 32768.) fOutputRawSignalEvent->SetOK(false);

            Short_t value = (Short_t)round(sData[x]);
            rSgnl.AddPoint(value);
        }

        if (GetVerboseLevel() >= REST_Debug) rSgnl.Print();
        debug << "Adding signal to raw signal event" << endl;
        fOutputRawSignalEvent->AddSignal(rSgnl);
    }

    debug << "TRestDetectorSignalToRawSignalProcess. Returning event with N signals "
          << fOutputRawSignalEvent->GetNumberOfSignals() << endl;

    return fOutputRawSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestDetectorSignalToRawSignalProcess metadata section
///
void TRestDetectorSignalToRawSignalProcess::InitFromConfigFile() {
    fSampling = GetDblParameterWithUnits("sampling");
    fNPoints = StringToInteger(GetParameter("Npoints", "512"));
    fTriggerMode = GetParameter("triggerMode", "firstDeposit");
    fTriggerDelay = StringToInteger(GetParameter("triggerDelay", 100));
    fGain = StringToDouble(GetParameter("gain", "100"));
    fIntegralThreshold = StringToDouble(GetParameter("integralThreshold", "1229"));
}
