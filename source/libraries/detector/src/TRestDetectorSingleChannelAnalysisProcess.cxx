///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSingleChannelAnalysisProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestDetectorSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include "TRestDetectorSingleChannelAnalysisProcess.h"

#include <TFitResult.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TRandom.h>
#include <TSpectrum.h>
using namespace std;

ClassImp(TRestDetectorSingleChannelAnalysisProcess)
    //______________________________________________________________________________
    TRestDetectorSingleChannelAnalysisProcess::TRestDetectorSingleChannelAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorSingleChannelAnalysisProcess::~TRestDetectorSingleChannelAnalysisProcess() {}

//______________________________________________________________________________
void TRestDetectorSingleChannelAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = NULL;

    fReadout = NULL;
}

//______________________________________________________________________________
void TRestDetectorSingleChannelAnalysisProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();
    fCalib = GetMetadata<TRestDetectorGainMap>();
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
            ferr << "You must set a TRestDetectorGainMap metadata object to apply gain correction!" << endl;
            abort();
        }
    }

    if (GetFriend("TRestRawSignalAnalysisProcess") == NULL) {
        ferr << "please add friend process TRestRawSignalAnalysisProcess and "
                "TRestRawReadoutAnalysisProcess "
                "and turn on all their observables!"
             << endl;
        abort();
    }
}

//______________________________________________________________________________
TRestEvent* TRestDetectorSingleChannelAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestDetectorSignalEvent*)evInput;

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
        // calculate updated ThresholdIntegral and PeakAmplitudeIntegral applying correction map
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

        // update charge value in output event
        for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
            TRestDetectorSignal* sgn = fSignalEvent->GetSignal(i);
            if (fCalib->fChannelGain.count(sgn->GetID()) == 0 || fCalib->fChannelGain[sgn->GetID()] == 0) {
                cout << "warning! unrecorded gain for channel: " << sgn->GetID() << endl;
                continue;
            }
            double gain = fCalib->fChannelGain[sgn->GetID()];
            for (int j = 0; j < sgn->GetNumberOfPoints(); j++) {
                auto timecharge = sgn->GetPoint(j);
                sgn->SetPoint(j, timecharge.X(), timecharge.Y() * gain);
            }
        }
    }

    SetObservableValue("ChnCorr_AmpInt", new_PeakAmplitudeIntegral);
    SetObservableValue("ChnCorr_ThrInt", new_ThresholdIntegral);

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestDetectorSingleChannelAnalysisProcess::EndProcess() {
    if (fCreateGainMap) {
        FitChannelGain();
        SaveGainMetadata(fCalibSave);
    }
}

void TRestDetectorSingleChannelAnalysisProcess::FitChannelGain() {
    cout << "TRestDetectorSingleChannelAnalysisProcess: fitting channel gain..." << endl;

    double channelFitMeanSum = 0;

    for (auto iter = fChannelThrIntegral.begin(); iter != fChannelThrIntegral.end(); iter++) {
        TH1D* h = iter->second;
        if (h->GetEntries() > 100) {
            // direct fit
            double middle = (fSpecFitRange.X() + fSpecFitRange.Y()) / 2;
            double range = (fSpecFitRange.Y() - fSpecFitRange.X()) / 2;
            TFitResultPtr r = h->Fit("gaus", "QS", "", fSpecFitRange.X(), fSpecFitRange.Y());
            if (r != -1) {
                const double* results = r->GetParams();
                double constant = results[0];
                double mean = results[1];
                double sigma = results[2];

                if (mean > middle - range / 1.2 && mean < middle + range / 1.2 && sigma / mean < 0.5) {
                    fChannelFitMean[iter->first] = mean;
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
                double dist = abs(peaks[i] - middle);
                if (dist < min) {
                    min = dist;
                    minpos = i;
                }
            }
            if (min < range * 2) {
                fChannelFitMean[iter->first] = peaks[minpos];
                channelFitMeanSum += peaks[minpos];
                fChannelGainError[iter->first] = 1;
                cout << iter->first << ", peak position: " << peaks[minpos] << ", total peaks: " << n << endl;
                continue;
            }

            // it is very bad channel, we prompt a warning
            cout << iter->first << ", too bad to fit" << endl;
        }
    }

    double meanmean = channelFitMeanSum / fChannelFitMean.size();

    cout << meanmean << endl;

    // normalize and fill the result
    for (auto iter = fChannelGain.begin(); iter != fChannelGain.end(); iter++) {
        if (fChannelFitMean.count(iter->first) == 0) {
            iter->second = 1;
        } else {
            iter->second = meanmean / fChannelFitMean[iter->first];
        }
    }
}

void TRestDetectorSingleChannelAnalysisProcess::SaveGainMetadata(string filename) {
    cout << "TRestDetectorSingleChannelAnalysisProcess: saving result..." << endl;

    // for (auto iter = fChannelGain.begin(); iter != fChannelGain.end(); iter++) {
    //    cout << iter->first << " " << iter->second << endl;
    //}

    fCalib = new TRestDetectorGainMap();
    fCalib->fChannelGain = fChannelGain;
    fCalib->SetName("ChannelCalibration");

    TRestRun* r = new TRestRun();
    r->SetOutputFileName(filename);
    r->AddMetadata(fCalib);
    r->AddMetadata(fReadout);
    r->AddMetadata(this);
    r->FormOutputFile();

    PrintChannelSpectrums(filename);
}

TH1D* TRestDetectorSingleChannelAnalysisProcess::GetChannelSpectrum(int id) {
    if (fChannelThrIntegral.count(id) != 0) return fChannelThrIntegral[id];
    return NULL;
}

void TRestDetectorSingleChannelAnalysisProcess::PrintChannelSpectrums(string filename) {
    TCanvas* c = new TCanvas();
    TLatex pText;
    pText.SetTextColor(kBlack);
    pText.SetTextFont(132);
    pText.SetTextSize(0.05);
    TLine pLine;
    pLine.SetLineColor(1);
    pLine.SetLineWidth(1);
    pLine.SetLineColor(1);

    c->Print((filename + ".pdf[").c_str());
    for (auto iter = fChannelThrIntegral.begin(); iter != fChannelThrIntegral.end(); iter++) {
        if (iter->second != NULL && iter->second->GetEntries() > 0) {
            cout << "Drawing: " << iter->first << endl;
            c->Clear();
            iter->second->Draw();
            pText.DrawLatex(fChannelFitMean[iter->first], iter->second->GetMaximum(),
                            Form("Relative gain: %.3f", fChannelGain[iter->first]));
            pText.DrawLatex(fChannelFitMean[iter->first], iter->second->GetMaximum() * 0.9,
                            Form("Error: %.2f", fChannelGainError[iter->first]));
            pLine.DrawLine(fChannelFitMean[iter->first], 0, fChannelFitMean[iter->first],
                           iter->second->GetMaximum() * 0.9);
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
void TRestDetectorSingleChannelAnalysisProcess::InitFromConfigFile() {
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
