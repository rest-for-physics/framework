///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutAnalysisProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include <TLegend.h>
#include <TPaveText.h>

#include "TRestReadoutAnalysisProcess.h"
using namespace std;

ClassImp(TRestReadoutAnalysisProcess)
    //______________________________________________________________________________
    TRestReadoutAnalysisProcess::TRestReadoutAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestReadoutAnalysisProcess::~TRestReadoutAnalysisProcess() {}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fSignalEvent = new TRestSignalEvent();

    fOutputEvent = fSignalEvent;
    fInputEvent = fSignalEvent;

    fReadout = NULL;
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::InitProcess() {
    fReadout = GetMetadata<TRestReadout>();
    if (fReadout != NULL) {
        {
            auto iter = fChannelsHistos.begin();
            while (iter != fChannelsHistos.end()) {
                TRestReadoutModule* mod = fReadout->GetReadoutModuleWithID(iter->first);
                if (mod == NULL) {
                    warning << "REST Warning(TRestReadoutAnalysisProcess): readout "
                               "module with id "
                            << iter->first << " not found!" << endl;
                } else {
                    iter->second =
                        new TH1D((TString) "ChannelActivity_M" + ToString(iter->first),
                                 (TString) "Readout Channel Activity of Module " + ToString(iter->first),
                                 mod->GetNumberOfChannels(), 0, mod->GetNumberOfChannels());
                }
                iter++;
            }
        }

        {
            auto iter = fChannelsHitMaps.begin();
            while (iter != fChannelsHitMaps.end()) {
                TRestReadoutModule* mod = fReadout->GetReadoutModuleWithID(iter->first);
                if (mod == NULL) {
                    warning << "REST Warning(TRestReadoutAnalysisProcess): readout "
                               "module with id "
                            << iter->first << " not found!" << endl;
                } else {
                    iter->second = new TH2D((TString) "Hitmap_M" + ToString(iter->first),
                                            (TString) "FirstX/Y Hitmap of Module " + ToString(iter->first),
                                            mod->GetNumberOfChannels() / 2, 0, mod->GetNumberOfChannels() / 2,
                                            mod->GetNumberOfChannels() / 2, mod->GetNumberOfChannels() / 2,
                                            mod->GetNumberOfChannels());
                }
                iter++;
            }
        }
    }
}

//______________________________________________________________________________
TRestEvent* TRestReadoutAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestSignalEvent*)evInput;
    fOutputEvent = fSignalEvent;
    if (fReadout != NULL) {
        Double_t firstX_id = -1.;
        Double_t firstY_id = -1.;
        Double_t firstX_t = 512.0;
        Double_t firstY_t = 512.0;

        Double_t lastX_id = -1.;
        Double_t lastY_id = -1.;
        Double_t lastX_t = 0.0;
        Double_t lastY_t = 0.0;

        double nan = numeric_limits<double>::quiet_NaN();
        this->SetObservableValue("FirstX", nan);
        this->SetObservableValue("FirstY", nan);
        this->SetObservableValue("LastX", nan);
        this->SetObservableValue("LastY", nan);

        set<int> TriggeredModuleId;

        double XEnergySum = 0;
        double XEnergyPosSum = 0;
        double YEnergySum = 0;
        double YEnergyPosSum = 0;

        for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
            TRestSignal* sgnl = fSignalEvent->GetSignal(i);

            int p, m, c;
            fReadout->GetPlaneModuleChannel(sgnl->GetID(), p, m, c);
            TriggeredModuleId.insert(m);

            if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
                XEnergySum += sgnl->GetIntegral();
                XEnergyPosSum += fReadout->GetX(sgnl->GetID()) * sgnl->GetIntegral();
            }
            if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
                YEnergySum += sgnl->GetIntegral();
                YEnergyPosSum += fReadout->GetY(sgnl->GetID()) * sgnl->GetIntegral();
            }

            if (sgnl->GetMaxPeakTime() < firstX_t) {
                if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
                    firstX_id = sgnl->GetID();
                    firstX_t = sgnl->GetMaxPeakTime();
                }
            }
            if (sgnl->GetMaxPeakTime() < firstY_t) {
                if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
                    firstY_id = sgnl->GetID();
                    firstY_t = sgnl->GetMaxPeakTime();
                }
            }

            if (sgnl->GetMaxPeakTime() > lastX_t) {
                if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
                    lastX_id = sgnl->GetID();
                    lastX_t = sgnl->GetMaxPeakTime();
                }
            }
            if (sgnl->GetMaxPeakTime() > lastY_t) {
                if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
                    lastY_id = sgnl->GetID();
                    lastY_t = sgnl->GetMaxPeakTime();
                }
            }
        }

        this->SetObservableValue("MeanX", XEnergySum == 0 ? nan : XEnergyPosSum / XEnergySum);
        this->SetObservableValue("MeanY", YEnergySum == 0 ? nan : YEnergyPosSum / YEnergySum);
        this->SetObservableValue("NmodulesTriggered", (int)TriggeredModuleId.size());
        this->SetObservableValue("TriggeredModuleId", TriggeredModuleId);

        if (firstX_id > -1 && firstY_id > -1) {
            double firstx = fReadout->GetX(firstX_id);
            double firsty = fReadout->GetY(firstY_id);
            double lastx = fReadout->GetX(lastX_id);
            double lasty = fReadout->GetY(lastY_id);
            this->SetObservableValue("FirstX", firstx);
            this->SetObservableValue("FirstY", firsty);
            this->SetObservableValue("LastX", lastx);
            this->SetObservableValue("LastY", lasty);

            int mod1 = -1, mod2 = -1;
            int channel1 = -1, channel2 = -1;
            int plane = -1;
            fReadout->GetPlaneModuleChannel(firstX_id, plane, mod1, channel1);
            fReadout->GetPlaneModuleChannel(firstY_id, plane, mod2, channel2);
            if (mod1 == mod2 && mod1 > -1) {
                int x = -1, y = -1;
                int n = fReadout->GetReadoutModuleWithID(mod1)->GetNumberOfChannels() / 2;
                if (channel1 >= n && channel2 < n) {
                    x = channel2;
                    y = channel1;
                } else if (channel2 >= n && channel1 < n) {
                    x = channel1;
                    y = channel2;
                }
                if (fChannelsHitMaps.count(mod1) > 0) {
                    if (fChannelsHitMaps[mod1] != NULL) fChannelsHitMaps[mod1]->Fill(x, y);
                }
                // cout << n<<" "<<channel1 <<" "<< channel2 << endl;
                // cout << x << " " << y << endl;
                // cout << fReadout->GetX(firstX_id) << " " << fReadout->GetY(firstY_id)
                // << endl; cout << endl;

                debug << "TRestReadoutAnalysisProcess. Adding point to hitmap of "
                         "module : "
                      << mod1 << endl;
                debug << "Position on module(X, Y) : (" << x << ", " << y << ")" << endl;
                debug << "Absolute position:(X, Y) : (" << firstx << ", " << firsty << ")" << endl;
            }
        }
        double integral = 0;
        for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
            TRestSignal* sgn = fSignalEvent->GetSignal(i);

            // channel histo
            int plane = -1, mod = -1, channel = -1;
            fReadout->GetPlaneModuleChannel(sgn->GetID(), plane, mod, channel);
            if (mod != -1 && fChannelsHistos.count(mod) > 0) {
                if (fChannelsHistos[mod] != NULL) fChannelsHistos[mod]->Fill(channel);

                // amplification, integral
                if (fModuldeAmplification[mod] == 0) fModuldeAmplification[mod] = 1;
                for (int j = 0; j < sgn->GetNumberOfPoints(); j++) {
                    auto timecharge = sgn->GetPoint(j);
                    sgn->SetPoint(j, timecharge.X(), timecharge.Y() / fModuldeAmplification[mod]);
                }
                integral += sgn->GetIntegral();
            }
        }

        this->SetObservableValue("CalibratedIntegral", integral);

        debug << "TRestReadoutAnalysisProcess. Calibrated event energy : " << integral << endl;
    }
    return fSignalEvent;
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::EndProcess() {
    if (fReadout != NULL) {
        {
            auto iter = fChannelsHistos.begin();
            while (iter != fChannelsHistos.end()) {
                if (iter->second != NULL) {
                    TH1D* histo = iter->second;
                    histo->GetXaxis()->SetTitle("Channel ID");
                    histo->GetYaxis()->SetTitle("Count");
                    histo->Write();
                }
                iter++;
            }
        }
        {
            auto iter = fChannelsHitMaps.begin();
            while (iter != fChannelsHitMaps.end()) {
                if (iter->second != NULL) {
                    TH2D* histo = iter->second;
                    histo->GetXaxis()->SetTitle("X Channel");
                    histo->GetYaxis()->SetTitle("Y Channel");
                    histo->Write();
                }
                iter++;
            }
        }
    }
}

//______________________________________________________________________________
// setting amplification:
// <parameter name="modulesAmp" value = "2-1:5-1.2:6-0.8:8-0.9" />
// setting readout modules to draw:
// <parameter name="modulesHist" value="2:5:6:8"/>
void TRestReadoutAnalysisProcess::InitFromConfigFile() {
    string moduleAmp = GetParameter("modulesAmp", "");
    auto ampdef = Split(moduleAmp, ":");
    for (int i = 0; i < ampdef.size(); i++) {
        auto amppair = Split(ampdef[i], "-");
        if (amppair.size() == 2) {
            fModuldeAmplification[StringToInteger(amppair[0])] = StringToDouble(amppair[1]);
        }
    }

    string moduleHist = GetParameter("modulesHist", "");
    auto histdef = Split(moduleHist, ":");
    for (int i = 0; i < histdef.size(); i++) {
        fChannelsHistos[StringToInteger(histdef[i])] = NULL;
        fChannelsHitMaps[StringToInteger(histdef[i])] = NULL;
    }
}
