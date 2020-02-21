///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSingleChannelAnalysisProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include "TRestSingleChannelAnalysisProcess.h"

#include <TLegend.h>
#include <TPaveText.h>
using namespace std;

ClassImp(TRestSingleChannelAnalysisProcess)
    //______________________________________________________________________________
    TRestSingleChannelAnalysisProcess::TRestSingleChannelAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestSingleChannelAnalysisProcess::~TRestSingleChannelAnalysisProcess() {}

//______________________________________________________________________________
void TRestSingleChannelAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = new TRestSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;

    fReadout = NULL;
}

//______________________________________________________________________________
void TRestSingleChannelAnalysisProcess::InitProcess() {
    fReadout = GetMetadata<TRestReadout>();
    fCalib = GetMetadata<TRestCalibration>();
    if (fReadout != NULL) {
        auto readout = *fReadout;
        for (int i = 0; i < readout.GetNumberOfReadoutPlanes(); i++) {
            auto plane = readout[i];
            for (int j = 0; j < plane.GetNumberOfModules(); j++) {
                auto mod = plane[j];
                for (int k = 0; k < mod.GetNumberOfChannels(); k++) {
                    auto channel = mod[k];
                    fChannelGain[channel.GetDaqID()] = 1;
                }
            }
        }
    } 

    if (fApplyGainCorrection) {
        if (fCalib != NULL) {
            auto iter = fChannelGain.begin();
            while (iter != fChannelGain.end()) {
                if (fCalib->fChannelGain.count(iter->first) == 0) {
                    ferr << "in consistent gain mapping and readout definition!" << endl;
                    ferr << "channel: " << iter->first << " not fount in mapping file!" << endl;
                    abort();
                }
                iter++;
            }

        } else {
            ferr << "You must set a TRestCalibration metadata object to apply gain correction!" << endl;
            abort();
        }
    }

    if (HasFriend("TRestRawSignalAnalysisProcess")) {
        calculateself = false;
    } else {
        calculateself = true;
    }
}

//______________________________________________________________________________
TRestEvent* TRestSingleChannelAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestSignalEvent*)evInput;
    fOutputEvent = fSignalEvent;

    double nan = numeric_limits<double>::quiet_NaN();

    map<int, Double_t> sAna_max_amplitude_map;
    map<int, Double_t> sAna_thr_integral_map;
    Double_t sAna_PeakAmplitudeIntegral = 0;
    Double_t sAna_ThresholdIntegral = 0;
    Double_t new_PeakAmplitudeIntegral = 0;
    Double_t new_ThresholdIntegral = 0;
    if (!calculateself) {
        sAna_max_amplitude_map =
            fAnalysisTree->GetObservableValue<map<int, Double_t>>("sAna_max_amplitude_map");
        sAna_thr_integral_map =
            fAnalysisTree->GetObservableValue<map<int, Double_t>>("sAna_thr_integral_map");
        sAna_PeakAmplitudeIntegral =
            fAnalysisTree->GetObservableValue<Double_t>("sAna_PeakAmplitudeIntegral");
        sAna_ThresholdIntegral = fAnalysisTree->GetObservableValue<Double_t>("sAna_ThresholdIntegral");
    } else {
        for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
            TRestSignal* sgn = fSignalEvent->GetSignal(i);
            sAna_max_amplitude_map[sgn->GetSignalID()] = sgn->GetMaxPeakValue();
            sAna_thr_integral_map[sgn->GetSignalID()] = sgn->GetIntegral();
            sAna_PeakAmplitudeIntegral += sgn->GetMaxPeakValue();
            sAna_ThresholdIntegral += sgn->GetIntegral();
        }
    }

    if (fCreateGainMap) {
        if ((sAna_ThresholdIntegral > fThrIntegralCutRange.X() &&
             sAna_ThresholdIntegral < fThrIntegralCutRange.Y()) ||
            (sAna_PeakAmplitudeIntegral > fAmpIntegralCutRange.X() &&
             sAna_PeakAmplitudeIntegral < fAmpIntegralCutRange.Y())) {
            // if within energy cut range
            int n1 = 0;
            auto iter1 = sAna_max_amplitude_map.begin();
            while (iter1 != sAna_max_amplitude_map.end()) {
                fChannelAmpIntegralSum[iter1->first] += iter1->second;
                n1++;
                iter1++;
            }

            int n2 = 0;
            auto iter2 = sAna_thr_integral_map.begin();
            while (iter2 != sAna_thr_integral_map.end()) {
                fChannelThrIntegralSum[iter2->first] += iter2->second;
                n2++;
                iter2++;
            }

            if (n1 != n2) {
                cout << "error! inconsistent map!" << endl;
            }

            iter1 = sAna_max_amplitude_map.begin();
            while (iter1 != sAna_max_amplitude_map.end()) {
                fChannelCounts[iter1->first]++;
                iter1++;
            }
        }

        new_PeakAmplitudeIntegral = sAna_PeakAmplitudeIntegral;
        new_ThresholdIntegral = sAna_ThresholdIntegral;
    }

    else if (fApplyGainCorrection) {
        auto iter1 = sAna_max_amplitude_map.begin();
        while (iter1 != sAna_max_amplitude_map.end()) {
            if (fCalib->fChannelGain.count(iter1->first)) {
                iter1->second *= fCalib->fChannelGain[iter1->first];
            }
            new_PeakAmplitudeIntegral += iter1->second;
            iter1++;
        }

        auto iter2 = sAna_thr_integral_map.begin();
        while (iter2 != sAna_thr_integral_map.end()) {
            if (fCalib->fChannelGain.count(iter2->first)) {
                iter2->second *= fCalib->fChannelGain[iter2->first];
            }
            new_ThresholdIntegral += iter2->second;
            iter2++;
        }
    }

    SetObservableValue("ChnCorr_AmpInt", new_PeakAmplitudeIntegral);
    SetObservableValue("ChnCorr_ThrInt", new_ThresholdIntegral);

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSingleChannelAnalysisProcess::EndProcess() {
    if (fCreateGainMap) {
        double sum = 0;
        double n = 0;
        map<int, double> fChannelAverage;
        auto iter2 = fChannelThrIntegralSum.begin();
        while (iter2 != fChannelThrIntegralSum.end()) {
            double avechannel = (double)iter2->second / fChannelCounts[iter2->first];
            fChannelAverage[iter2->first] = avechannel;
            sum += avechannel;
            n++;
            iter2++;
        }

        double aveall = sum / n;
        auto iter = fChannelAverage.begin();
        while (iter != fChannelAverage.end()) {
            fChannelGain[iter->first] = aveall / iter->second;
            iter++;
        }

        fCalib = new TRestCalibration();
        fCalib->fChannelGain = fChannelGain;
        fCalib->SetName("ChannelCalibration");
        fCalib->Write();

        TRestRun* r = new TRestRun();
        r->AddMetadata(fCalib);
       
        r->FormOutputFile();
    }
}

//______________________________________________________________________________
// setting amplification:
// <parameter name="modulesAmp" value = "2-1:5-1.2:6-0.8:8-0.9" />
// setting readout modules to draw:
// <parameter name="modulesHist" value="2:5:6:8"/>
void TRestSingleChannelAnalysisProcess::InitFromConfigFile() {
    string mode = GetParameter("mode", "apply");
    if (mode == "create") {
        fCreateGainMap = true;
        fApplyGainCorrection = false;
        fSingleThreadOnly = true;
    } else if (mode == "apply") {
        fCreateGainMap = false;
        fApplyGainCorrection = true;
        fSingleThreadOnly = false;
    } else {
        ferr << "illegal mode definition! supported: create, apply" << endl;
        abort();
    }

    fThrIntegralCutRange = StringTo2DVector(GetParameter("thrEnergyRange", "(0,1e9)"));
    fAmpIntegralCutRange = StringTo2DVector(GetParameter("ampEnergyRange", "(0,0)"));
}
