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
/// The TRestSignalZeroSuppresionProcess identifies the points that are over
/// threshold from a TRestRawSignalEvent. The resulting points will be
/// transported to a TRestSignalEvent that will be returned as output event
/// type of this process. The data points at TRestSignalEvent will be
/// attributed physical time units related to the sampling rate of the raw
/// signal received as input.
///
/// TODO Write more details here, input parameters, conditions, etc.
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-January: Conception and implementation of signal zero suppression
/// process.
///               Javier Galan
///
/// \class      TRestSignalZeroSuppresion
/// \author     Javier Galan
/// \author     Kaixiang Ni
///
/// <hr>
///

// #include <TRestDetectorSetup.h> // Not used now but it should serve to get sampling rate of rawSignal

#include "TRestSignalZeroSuppresionProcess.h"
#include <numeric>
using namespace std;

const double cmTomm = 10.;

ClassImp(TRestSignalZeroSuppresionProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestSignalZeroSuppresionProcess::TRestSignalZeroSuppresionProcess() { Initialize(); }

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
TRestSignalZeroSuppresionProcess::TRestSignalZeroSuppresionProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestSignalZeroSuppresionProcess::~TRestSignalZeroSuppresionProcess() {
    delete fRawSignalEvent;
    delete fSignalEvent;
}

///////////////////////////////////////////////
/// \brief Method to load the default config in absence of RML input
///
void TRestSignalZeroSuppresionProcess::LoadDefaultConfig() {
    SetName("signalZeroSuppresionProcess-Default");
    SetTitle("Default config");

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    TVector2 fBaseLineRange = TVector2(10, 90);
    fPointThreshold = 2.;
    fSignalThreshold = 2.;
    fNPointsOverThreshold = 10;
    fSampling = 0.1;
    fBaseLineCorrection = false;
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
void TRestSignalZeroSuppresionProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Input/output event types declaration and section name.
///
void TRestSignalZeroSuppresionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fRawSignalEvent = new TRestRawSignalEvent();
    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fRawSignalEvent;
    fOutputEvent = fSignalEvent;
}

///////////////////////////////////////////////
/// \brief Process initialization. It identifies if baseline has been
/// already zero-ed by a previous process.
///
/// Personal note (TODO): Should be the baseline zero-ed by
/// TRestRawSignalAnalysisProcess? It is that necessary?
///
void TRestSignalZeroSuppresionProcess::InitProcess() {
    // !!!!!!!!!!!! BASELINE CORRECTION !!!!!!!!!!!!!!
    // TRestRawSignalAnalysisProcess subtracts baseline. Baseline is double value,
    // but data points in TRestRawSignalAnalysisProcess is only short integer
    // type. So we need to correct this by adding decimal part back.

    fBaseLineCorrection = false;
    for (int i = 0; i < fFriendlyProcesses.size(); i++) {
        if ((string)fFriendlyProcesses[i]->ClassName() == "TRestRawSignalAnalysisProcess") {
            fBaseLineCorrection = true;

            // setting parameters to the same as TRestRawSignalAnalysisProcess
            fBaseLineRange = StringTo2DVector(fFriendlyProcesses[i]->GetParameter("baseLineRange", "(5,55)"));
            fIntegralRange =
                StringTo2DVector(fFriendlyProcesses[i]->GetParameter("integralRange", "(10,500)"));
            fPointThreshold = StringToDouble(fFriendlyProcesses[i]->GetParameter("pointThreshold", "2"));
            fNPointsOverThreshold =
                StringToInteger(fFriendlyProcesses[i]->GetParameter("pointsOverThreshold", "5"));
            fSignalThreshold = StringToDouble(fFriendlyProcesses[i]->GetParameter("signalThreshold", "5"));
        }
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestSignalZeroSuppresionProcess::ProcessEvent(TRestEvent* evInput) {
    fRawSignalEvent = (TRestRawSignalEvent*)evInput;

    /* I think we don't need that now, right?
        fSignalEvent->SetID(fRawSignalEvent->GetID());
        fSignalEvent->SetSubID(fRawSignalEvent->GetSubID());
        fSignalEvent->SetTimeStamp(fRawSignalEvent->GetTimeStamp());
        fSignalEvent->SetSubEventTag(fRawSignalEvent->GetSubEventTag());
            */

    Int_t numberOfSignals = fRawSignalEvent->GetNumberOfSignals();

    Double_t totalIntegral = 0;
    Double_t rejectedSignal = 0;
    for (int n = 0; n < numberOfSignals; n++) {
        TRestRawSignal* s = fRawSignalEvent->GetSignal(n);
        TRestSignal sgn;

        sgn.SetID(s->GetID());
        double baseline = s->GetBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
        double baselinerms = s->GetBaseLineSigma(fBaseLineRange.X(), fBaseLineRange.Y());
        for (int i = fIntegralRange.X(); i < fIntegralRange.Y(); i++) {
            if (s->GetData(i) > baseline + fPointThreshold * baselinerms) {
                int pos = i;
                vector<double> pulse;
                pulse.push_back(s->GetData(i));
                i++;
                int flatN = 0;
                while (i < fIntegralRange.Y() && s->GetData(i) > baseline + fPointThreshold * baselinerms) {
                    if (TMath::Abs(s->GetData(i) - s->GetData(i - 1)) > fPointThreshold * baselinerms) {
                        flatN = 0;
                    } else {
                        flatN++;
                    }

                    if (flatN < fNPointsFlatThreshold) {
                        pulse.push_back(s->GetData(i));
                        i++;
                    } else {
                        break;
                    }
                }
                if (pulse.size() >= fNPointsOverThreshold) {
                    // auto stdev = TMath::StdDev(begin(pulse), end(pulse));
                    // calculate stdev
                    double mean = std::accumulate(pulse.begin(), pulse.end(), 0.0) / pulse.size();
                    double sq_sum = std::inner_product(pulse.begin(), pulse.end(), pulse.begin(), 0.0);
                    double stdev = std::sqrt(sq_sum / pulse.size() - mean * mean);

                    if (stdev > fSignalThreshold * baselinerms) {
                        for (int j = pos; j < i; j++) {
                            if (fBaseLineCorrection) {
                                if (baseline >= 0 && baseline < 1) {
                                    sgn.NewPoint(
                                        j * fSampling,
                                        (Double_t)s->GetData(j) - baseline);  // convert timeBin(no unit) to
                                                                              // time(unit: us)
                                } else {
                                    cout << "REST Error! baseline(" << baseline
                                         << ") is without [0,1), check your code!" << endl;
                                }
                            } else {
                                sgn.NewPoint(j * fSampling, s->GetData(j));
                            }
                        }
                    }
                }
            }
        }

        if (sgn.GetNumberOfPoints() > 0) {
            fSignalEvent->AddSignal(sgn);
            totalIntegral += sgn.GetIntegral();
        } else {
            rejectedSignal++;
        }

        // TRestRawSignal *sgnl = fRawSignalEvent->GetSignal(i);
        //      Double_t integral = sgnl->GetIntegralWithThreshold(
        //      fIntegralRange.X(), fIntegralRange.Y(),
        //              fBaseLineRange.X(), fBaseLineRange.Y(), fPointThreshold,
        //              fNPointsOverThreshold, fSignalThreshold );
        // Double_t baseline = sgnl->GetBaseLine(fBaseLineRange.X(),
        // fBaseLineRange.Y());

        //      if( integral > 0 )
        //      {
        //          totalIntegral += integral;

        //          vector <Int_t> poinsOver = sgnl->GetPointsOverThreshold();

        //	TRestSignal outSignal;
        //	outSignal.SetID(sgnl->GetID());
        //	for (unsigned int n = 0; n < poinsOver.size(); n++) {
        //		if (fBaseLineCorrection) {
        //			if (baseline >= 0 && baseline < 1) {
        //				outSignal.NewPoint(poinsOver[n],
        //(Double_t)sgnl->GetData(poinsOver[n]) - baseline);
        //			}
        //			else
        //			{
        //				cout << "REST Error! baseline(" << baseline << ") is
        // without [0,1), check your code!" << endl;
        //			}
        //		}
        //		else
        //		{
        //			outSignal.NewPoint(poinsOver[n],
        // sgnl->GetData(poinsOver[n]));
        //		}
        //	}

        //	fSignalEvent->AddSignal(outSignal);
        //      }
    }

    if (this->GetVerboseLevel() >= REST_Extreme) {
        fSignalEvent->PrintEvent();
    }

    debug << "TRestSignalZeroSuppresionProcess. Signals added : " << fSignalEvent->GetNumberOfSignals()
          << endl;
    debug << "TRestSignalZeroSuppresionProcess. Signals rejected : " << rejectedSignal << endl;
    debug << "TRestSignalZeroSuppresionProcess. Threshold integral : " << totalIntegral << endl;

    if (fSignalEvent->GetNumberOfSignals() <= 0) return NULL;

    return fSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestSignalZeroSuppresionProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestSignalZeroSuppresionProcess metadata section
///
void TRestSignalZeroSuppresionProcess::InitFromConfigFile() {
    // keep up with TRestRawSignalAnalysisProcess
    fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
    fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
    fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
    fNPointsOverThreshold = StringToInteger(GetParameter("pointsOverThreshold", "5"));
    fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "5"));

    // introduced to prevent daq abnormal response: flat high signal tail
    fNPointsFlatThreshold = StringToInteger(GetParameter("pointsFlatThreshold", "512"));

    fSampling = GetDblParameterWithUnits("sampling", 0.1);
}
