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

#include <TFitResult.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TRandom.h>
#include <TSpectrum.h>
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
    } else {
        auto readout = *fReadout;
        for (int i = 0; i < readout.GetNumberOfReadoutPlanes(); i++) {
            auto plane = readout[i];
            for (int j = 0; j < plane.GetNumberOfModules(); j++) {
                auto mod = plane[j];
                for (int k = 0; k < mod.GetNumberOfChannels(); k++) {
                    auto channel = mod[k];
                    fChannelGain[channel.GetDaqID()] = 1;       // default correction factor is 1
                    fChannelGainError[channel.GetDaqID()] = 1;  // relative error
                    fChannelThrIntegral[channel.GetDaqID()] =
                        new TH1D(Form("h%i", channel.GetDaqID()), Form("h%i", channel.GetDaqID()), 100, 0,
                                 fSpecFitRange.Y() * 1.5);
                }
            }
        }
    }

    if (fApplyGainCorrection) {
        if (fCalib != NULL) {
            for (auto iter = fChannelGain.begin(); iter != fChannelGain.end(); iter++) {
                if (fCalib->fChannelGain.count(iter->first) == 0) {
                    ferr << "in consistent gain mapping and readout definition!" << endl;
                    ferr << "channel: " << iter->first << " not fount in mapping file!" << endl;
                    abort();
                }
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
    Double_t sAna_NumberOfGoodSignals = fAnalysisTree->GetObservableValue<int>("sAna_NumberOfGoodSignals");
    Double_t rA_MeanX = fAnalysisTree->GetObservableValue<Double_t>("rA_MeanX");
    Double_t rA_MeanY = fAnalysisTree->GetObservableValue<Double_t>("rA_MeanY");

    if (fCreateGainMap) {
        if ((sAna_ThresholdIntegral > fThrIntegralCutRange.X() &&
             sAna_ThresholdIntegral < fThrIntegralCutRange.Y()) &&
            (sAna_NumberOfGoodSignals > fNGoodSignalsCutRange.X() &&
             sAna_NumberOfGoodSignals < fNGoodSignalsCutRange.Y())) {
            // if within energy cut range

            for (auto iter = sAna_thr_integral_map.begin(); iter != sAna_thr_integral_map.end(); iter++) {
                if (fChannelThrIntegral[iter->first] == NULL) {
                    fChannelThrIntegral[iter->first] = new TH1D(
                        Form("h%i", iter->first), Form("h%i", iter->first), 100, 0, fSpecFitRange.Y() * 1.5);
                }

                fChannelThrIntegral[iter->first]->Fill(iter->second);
            }
        }
    }

    else if (fApplyGainCorrection) {
        for (auto iter = sAna_max_amplitude_map.begin(); iter != sAna_max_amplitude_map.end(); iter++) {
            if (fCalib->fChannelGain.count(iter->first)) {
                iter->second *= fCalib->fChannelGain[iter->first];
            }
            new_PeakAmplitudeIntegral += iter->second;
        }

        for (auto iter = sAna_thr_integral_map.begin(); iter != sAna_thr_integral_map.end(); iter++) {
            if (fCalib->fChannelGain.count(iter->first)) {
                iter->second *= fCalib->fChannelGain[iter->first];
            }
            new_ThresholdIntegral += iter->second;
        }
    }

    SetObservableValue("ChnCorr_AmpInt", new_PeakAmplitudeIntegral);
    SetObservableValue("ChnCorr_ThrInt", new_ThresholdIntegral);

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestSingleChannelAnalysisProcess::EndProcess() {
    if (fCreateGainMap) {
        FitChannelGain();
        SaveGainMetadata(fCalibSave);
    }
}

void TRestSingleChannelAnalysisProcess::FitChannelGain() {
    cout << "TRestSingleChannelAnalysisProcess: fitting channel gain..." << endl;

    map<int, double> channelFitMean;
    double channelFitMeanSum = 0;

    for (auto iter = fChannelThrIntegral.begin(); iter != fChannelThrIntegral.end(); iter++) {
        TH1D* h = iter->second;
        if (h->GetEntries() > GetFullAnalysisTree()->GetEntries() / 2000) {
            TFitResultPtr r = h->Fit("gaus", "QS", "", fSpecFitRange.X(), fSpecFitRange.Y());
            if (r != -1) {
                const double* results = r->GetParams();
                double constant = results[0];
                double mean = results[1];
                double sigma = results[2];

                if (mean > fSpecFitRange.X() && mean < fSpecFitRange.Y() && sigma / mean < 0.5) {
                    channelFitMean[iter->first] = mean;
                    channelFitMeanSum += mean;
                    fChannelGainError[iter->first] = sigma / mean;
                    cout << iter->first << ", mean: " << mean << ", error: " << sigma / mean << endl;
                    continue;
                }
            }

            // if fit with gaus failed, we use TSpectrum to find the peak
            TSpectrum spc;
            int n = spc.Search(h);
            double* peaks = spc.GetPositionX();
            double min = 1e9;
            int minpos = 0;
            for (int i = 0; i < n; i++) {
                double dist = abs(peaks[i] - (fSpecFitRange.X() + fSpecFitRange.Y()) / 2);
                if ( dist < min) {
                    min = dist;
                    minpos = i;
                }
            }
            channelFitMean[iter->first] = peaks[minpos];
            channelFitMeanSum += peaks[minpos];
            fChannelGainError[iter->first] = 1;
            cout << iter->first << ", peak position: " << peaks[minpos] << ", total peaks: "<< n << endl;

        }
    }

    double meanmean = channelFitMeanSum / channelFitMean.size();

    cout << meanmean << endl;

    for (auto iter = fChannelGain.begin(); iter != fChannelGain.end(); iter++) {
        if (channelFitMean.count(iter->first) == 0) {
            iter->second = 1;
        } else {
            iter->second = meanmean / channelFitMean[iter->first];
        }
    }
}

void TRestSingleChannelAnalysisProcess::SaveGainMetadata(string filename) {
    cout << "TRestSingleChannelAnalysisProcess: saving result..." << endl;

    // for (auto iter = fChannelGain.begin(); iter != fChannelGain.end(); iter++) {
    //    cout << iter->first << " " << iter->second << endl;
    //}

    fCalib = new TRestGainMap();
    fCalib->fChannelGain = fChannelGain;
    fCalib->SetName("ChannelCalibration");

    TRestRun* r = new TRestRun();
    r->SetOutputFileName(filename);
    r->AddMetadata(fCalib);
    r->AddMetadata(fReadout);
    r->AddMetadata(this);
    r->FormOutputFile();
}

TH1D* TRestSingleChannelAnalysisProcess::GetChannelSpectrum(int id) {
    if (fChannelThrIntegral.count(id) != 0) return fChannelThrIntegral[id];
    return NULL;
}

void TRestSingleChannelAnalysisProcess::PrintChannelSpectrums(string filename) {
    TCanvas* c = new TCanvas();
    
    c->Print((filename + ".pdf[").c_str());
    for (auto iter = fChannelThrIntegral.begin(); iter != fChannelThrIntegral.end(); iter++) {
        if (iter->second != NULL && iter->second->GetEntries() > 0) {
            cout << "Drawing: " << iter->first << endl;
            iter->second->Draw();
            c->Print((filename + ".pdf").c_str());
        }
    }
    c->Clear();
    c->Print((filename + ".pdf]").c_str());
    delete c;
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
    fNGoodSignalsCutRange = StringTo2DVector(GetParameter("nGoodSignalsRange", "(4,14)"));
    fSpecFitRange = StringTo2DVector(GetParameter("specFitRange", "(1e4,2e4)"));
    fCalibSave = GetParameter("save", "calib.root");
}
