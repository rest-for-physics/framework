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
#include <TRandom.h>
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
    fCalib = GetMetadata<TRestGainMap>();
    if (fReadout == NULL) {
        if (fCreateGainMap && fMethod == "area") {
            ferr << "You must set a TRestReadout metadata object to create gain map!" << endl;
            abort();
        }
    } else {
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

        double xmin, xmax, ymin, ymax;
        fReadout->GetReadoutPlane(0)->GetBoundaries(xmin, xmax, ymin, ymax);
        fAreaGainMap = new TH2D("areaGainMap", "areaGainMap", 100, xmin, xmax, 100, ymin, ymax);
        fAreaCounts = new TH2D("areaCounts", "areaCounts", 100, xmin, xmax, 100, ymin, ymax);
        fAreaThrIntegralSum =
            new TH2D("areaThrIntegralSum", "areaThrIntegralSum", 100, xmin, xmax, 100, ymin, ymax);
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
            ferr << "You must set a TRestGainMap metadata object to apply gain correction!" << endl;
            abort();
        }
    }

    if (!HasFriend("TRestRawSignalAnalysisProcess") || !HasFriend("TRestReadoutAnalysisProcess")) {
        ferr << "please add friend process TRestRawSignalAnalysisProcess and TRestReadoutAnalysisProcess "
                "and turn on all their observables!"
             << endl;
        abort();
    }
}

//______________________________________________________________________________
TRestEvent* TRestSingleChannelAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestSignalEvent*)evInput;
    fOutputEvent = fSignalEvent;

    double nan = numeric_limits<double>::quiet_NaN();

    Double_t new_PeakAmplitudeIntegral = 0;
    Double_t new_ThresholdIntegral = 0;

    map<int, Double_t> sAna_max_amplitude_map =
        fAnalysisTree->GetObservableValue<map<int, Double_t>>("sAna_max_amplitude_map");
    map<int, Double_t> sAna_thr_integral_map =
        fAnalysisTree->GetObservableValue<map<int, Double_t>>("sAna_thr_integral_map");
    Double_t sAna_PeakAmplitudeIntegral =
        fAnalysisTree->GetObservableValue<Double_t>("sAna_PeakAmplitudeIntegral");
    Double_t sAna_ThresholdIntegral = fAnalysisTree->GetObservableValue<Double_t>("sAna_ThresholdIntegral");
    Double_t sAna_NumberOfGoodSignals =
        fAnalysisTree->GetObservableValue<int>("sAna_NumberOfGoodSignals");
    Double_t rA_MeanX = fAnalysisTree->GetObservableValue<Double_t>("rA_MeanX");
    Double_t rA_MeanY = fAnalysisTree->GetObservableValue<Double_t>("rA_MeanY");

    if (fCreateGainMap) {
        if ((sAna_ThresholdIntegral > fThrIntegralCutRange.X() &&
             sAna_ThresholdIntegral < fThrIntegralCutRange.Y()) &&
            (sAna_NumberOfGoodSignals > fNGoodSignalsCutRange.X() &&
             sAna_NumberOfGoodSignals < fNGoodSignalsCutRange.Y())) {
            // if within energy cut range

            if (fMethod == "singleInt") {
                // use the sum of the channel's waveform to draw single channel spectrum
                int n = 0;
                auto iter = sAna_thr_integral_map.begin();
                while (iter != sAna_thr_integral_map.end()) {
                    fChannelThrIntegralSum[iter->first] += iter->second;
                    fChannelCounts[iter->first]++;
                    n++;
                    iter++;
                }

            } else if (fMethod == "invInt") {
                // use sAna_ThresholdIntegral to draw single channel spectrum
                int n = 0;
                auto iter = sAna_thr_integral_map.begin();
                while (iter != sAna_thr_integral_map.end()) {
                    fChannelThrIntegralSum[iter->first] += sAna_ThresholdIntegral;
                    fChannelCounts[iter->first]++;
                    n++;
                    iter++;
                }
            } else if (fMethod == "area") {
                // Draw spectrum for different small areas. decode the area gain map in the end of the process
                int bin = fAreaThrIntegralSum->FindBin(rA_MeanX, rA_MeanY);
                fAreaThrIntegralSum->SetBinContent(
                    bin, fAreaThrIntegralSum->GetBinContent(bin) + sAna_ThresholdIntegral);
                fAreaCounts->SetBinContent(bin, fAreaCounts->GetBinContent(bin) + 1);
            } else {
                ferr << "unknown method!" << endl;
                abort();
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
        if (fMethod == "area") {
            // Calculate the mean of each bin's spectrum
            double sum = 0;
            double n = 0;
            map<int, double> fChannelGainSum;
            for (int i = 1; i <= fAreaGainMap->GetNbinsX(); i++) {
                for (int j = 1; j <= fAreaGainMap->GetNbinsY(); j++) {
                    if (fAreaCounts->GetBinContent(i, j) > 20) {
                        double avebin =
                            fAreaThrIntegralSum->GetBinContent(i, j) / fAreaCounts->GetBinContent(i, j);
                        fAreaGainMap->SetBinContent(i, j, avebin);
                        sum += avebin;
                        n++;
                    }
                }
            }

            // normalize the bins to around 1, fill empty bins to 1
            fAreaGainMap->Scale(n / sum);
            for (int i = 1; i <= fAreaGainMap->GetNbinsX(); i++) {
                for (int j = 1; j <= fAreaGainMap->GetNbinsY(); j++) {
                    if (fAreaGainMap->GetBinContent(i, j) == 0) {
                        fAreaGainMap->SetBinContent(i, j, 1);
                    }
                }
            }

            // throw points randomly to both histogram and readout
            for (int i = 0; i < 100000; i++) {
                double xmin, xmax, ymin, ymax;
                fReadout->GetReadoutPlane(0)->GetBoundaries(xmin, xmax, ymin, ymax);
                double xx = gRandom->Rndm() * (xmax - xmin) + xmin;
                double yy = gRandom->Rndm() * (ymax - ymin) + ymin;
                int p, m, c;
                int channel = fReadout->GetHitsDaqChannel(TVector3(xx, yy, 0.1), p, m, c);
                if (channel != -1) {
                    fChannelGainSum[channel] += fAreaGainMap->GetBinContent(fAreaGainMap->FindBin(xx, yy));
                    fChannelCounts[channel]++;
                }
            }

            // now each channel gain is the sum of several area gains, we take average
            auto iter = fChannelGainSum.begin();
            while (iter != fChannelGainSum.end()) {
                fChannelGain[iter->first] = fChannelCounts[iter->first] / iter->second;
                if (fVerboseLevel >= REST_Info) {
                    cout << "channel: " << iter->first << ", gain(relative): " << fChannelGain[iter->first]
                         << endl;
                }
                iter++;
            }

        } else {
            double sum = 0;
            double n = 0;
            map<int, double> fChannelThrIntegralAverage;
            auto iter2 = fChannelThrIntegralSum.begin();
            while (iter2 != fChannelThrIntegralSum.end()) {
                if (fChannelCounts[iter2->first] > GetFullAnalysisTree()->GetEntries() / 2000) {
                    // otherwise it may be a dead channel
                    double avechannel = (double)iter2->second / fChannelCounts[iter2->first];
                    fChannelThrIntegralAverage[iter2->first] = avechannel;
                    // cout << iter2->first << " " << avechannel << ", sum and counts: " <<iter2->second << "
                    // " << fChannelCounts[iter2->first] << endl;
                    sum += avechannel;
                    n++;
                }
                iter2++;
            }

            double aveall = sum / n;
            auto iter = fChannelThrIntegralAverage.begin();
            while (iter != fChannelThrIntegralAverage.end()) {
                fChannelGain[iter->first] = aveall / iter->second;
                if (fVerboseLevel >= REST_Info) {
                    cout << "channel: " << iter->first << ", gain(relative): " << fChannelGain[iter->first]
                         << endl;
                }
                iter++;
            }
        }

        fCalib = new TRestGainMap();
        fCalib->fChannelGain = fChannelGain;
        fCalib->SetName("ChannelCalibration");

        TRestRun* r = new TRestRun();
        r->SetOutputFileName(fOutputCalibrationFileName);
        r->AddMetadata(fCalib);
        r->AddMetadata(fReadout);
        r->FormOutputFile();
        if (fAreaGainMap != NULL) fAreaGainMap->Write();

        delete fCalib;
        fCalib = NULL;
        delete r;
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

    fMethod = GetParameter("method", "singleInt");
    fThrIntegralCutRange = StringTo2DVector(GetParameter("thrEnergyRange", "(0,1e9)"));
    fNGoodSignalsCutRange = StringTo2DVector(GetParameter("nGoodSignalsRange", "(4,14)"));
    fOutputCalibrationFileName = GetParameter("calibOutput", "calib.root");
}
