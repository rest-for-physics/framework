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

#include "TRestReadoutAnalysisProcess.h"

#include <TLegend.h>
#include <TPaveText.h>
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

    fSignalEvent = NULL;

    fReadout = NULL;
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::InitProcess() {
    fReadout = GetMetadata<TRestReadout>();
    if (fReadout != NULL) {
        auto iter = fModuleHitMaps.begin();
        while (iter != fModuleHitMaps.end()) {
            TRestReadoutModule* mod = fReadout->GetReadoutModuleWithID(iter->first);
            if (mod == NULL) {
                warning << "REST Warning(TRestReadoutAnalysisProcess): readout "
                           "module with id "
                        << iter->first << " not found!" << endl;
            } else {
                fModuleHitMaps[iter->first] =
                    new TH2D((TString) "Hitmap_M" + ToString(iter->first),
                             (TString) "FirstX/Y Hitmap of Module " + ToString(iter->first),
                             mod->GetNumberOfChannels() / 2, 0, mod->GetNumberOfChannels() / 2,
                             mod->GetNumberOfChannels() / 2, mod->GetNumberOfChannels() / 2,
                             mod->GetNumberOfChannels());

                fModuleActivityX[iter->first] =
                    new TH1D((TString) "ActivityX_M" + ToString(iter->first),
                             (TString) "X Channel Activity of Module " + ToString(iter->first),
                             mod->GetNumberOfChannels() / 2, 0, mod->GetNumberOfChannels() / 2);

                fModuleActivityY[iter->first] =
                    new TH1D((TString) "ActivityY_M" + ToString(iter->first),
                             (TString) "Y Channel Activity of Module " + ToString(iter->first),
                             mod->GetNumberOfChannels() / 2, mod->GetNumberOfChannels() / 2,
                             mod->GetNumberOfChannels());

                fModuleBSLSigmaX[iter->first] =
                    new TH2D((TString) "BSLSX_M" + ToString(iter->first),
                             (TString) "X Channel Baseline Sigma of Module " + ToString(iter->first),
                             mod->GetNumberOfChannels() / 2, 0, mod->GetNumberOfChannels() / 2, 100, 0, 100);

                fModuleBSLSigmaY[iter->first] =
                    new TH2D((TString) "BSLSY_M" + ToString(iter->first),
                             (TString) "Y Channel Baseline Sigma of Module " + ToString(iter->first), 100, 0,
                             100, mod->GetNumberOfChannels() / 2, mod->GetNumberOfChannels() / 2,
                             mod->GetNumberOfChannels());
            }
            iter++;
        }
    }
}

//______________________________________________________________________________
TRestEvent* TRestReadoutAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestRawSignalEvent*)evInput;
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

        // calculate firstx, firsty in position coordinate
        for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
            TRestRawSignal* sgnl = fSignalEvent->GetSignal(i);

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

            if (sgnl->GetMaxPeakBin() < firstX_t) {
                if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
                    firstX_id = sgnl->GetID();
                    firstX_t = sgnl->GetMaxPeakBin();
                }
            }
            if (sgnl->GetMaxPeakBin() < firstY_t) {
                if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
                    firstY_id = sgnl->GetID();
                    firstY_t = sgnl->GetMaxPeakBin();
                }
            }

            if (sgnl->GetMaxPeakBin() > lastX_t) {
                if (!TMath::IsNaN(fReadout->GetX(sgnl->GetID()))) {
                    lastX_id = sgnl->GetID();
                    lastX_t = sgnl->GetMaxPeakBin();
                }
            }
            if (sgnl->GetMaxPeakBin() > lastY_t) {
                if (!TMath::IsNaN(fReadout->GetY(sgnl->GetID()))) {
                    lastY_id = sgnl->GetID();
                    lastY_t = sgnl->GetMaxPeakBin();
                }
            }
        }

        this->SetObservableValue("MeanX", XEnergySum == 0 ? nan : XEnergyPosSum / XEnergySum);
        this->SetObservableValue("MeanY", YEnergySum == 0 ? nan : YEnergyPosSum / YEnergySum);
        this->SetObservableValue("NmodulesTriggered", (int)TriggeredModuleId.size());
        this->SetObservableValue("TriggeredModuleId", TriggeredModuleId);

        // fill firstx/y hitmap in channel coordinate
        map<int, int> modulefirstxchannel;  // moduleid, firstx channelid
        map<int, int> modulefirstychannel;  // moduleid, firsty channelid
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
                // consider the rotation of readout module, firstX may be from the Y channel!
                int x = -1, y = -1;
                int n = fReadout->GetReadoutModuleWithID(mod1)->GetNumberOfChannels() / 2;
                if (channel1 >= n && channel2 < n) {
                    x = channel2;
                    y = channel1;
                } else if (channel2 >= n && channel1 < n) {
                    x = channel1;
                    y = channel2;
                }
                modulefirstxchannel[mod1] = x;
                modulefirstychannel[mod1] = y;
                if (fModuleHitMaps.count(mod1) > 0) {
                    if (fModuleHitMaps[mod1] != NULL) fModuleHitMaps[mod1]->Fill(x, y);
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
        this->SetObservableValue("ModuleFirstX", modulefirstxchannel);
        this->SetObservableValue("ModuleFirstY", modulefirstychannel);


        // for each channel
        vector<int> moduleid;
        vector<int> channelid;
        vector<double> baselinesigma;
        vector<double> baseline;
        vector<double> thresholdint;
        //map<int, map<int, double>> modulebaselinesigma;  // moduleid, channelid, baselinesigma
        //map<int, map<int, double>> modulebaseline;       // moduleid, channelid, baseline
        //map<int, map<int, double>> modulethresholdint;   // moduleid, channelid, thresholdintergal

        for (int i = 0; i < fSignalEvent->GetNumberOfSignals(); i++) {
            TRestRawSignal* sgn = fSignalEvent->GetSignal(i);

            // channel histo
            int plane = -1, mod = -1, channel = -1;
            fReadout->GetPlaneModuleChannel(sgn->GetID(), plane, mod, channel);
            if (mod != -1 && channel!=-1) {
                moduleid.push_back(mod);
                channelid.push_back(channel);
                baselinesigma.push_back(sgn->GetBaseLineSigma());
                baseline.push_back(sgn->GetBaseLine());
                thresholdint.push_back(sgn->GetThresholdIntegral());


                if (fModuleHitMaps.count(mod) > 0) {
                    fModuleActivityX[mod]->Fill(channel);
                    fModuleActivityY[mod]->Fill(channel);
                    fModuleBSLSigmaX[mod]->Fill(channel, sgn->GetBaseLineSigma());
                    fModuleBSLSigmaY[mod]->Fill(sgn->GetBaseLineSigma(), channel);
                }
            }
        }
        this->SetObservableValue("Module", moduleid);
        this->SetObservableValue("Channel", channelid);
        this->SetObservableValue("BaselineSigma", baselinesigma);
        this->SetObservableValue("Baseline", baseline);
        this->SetObservableValue("ThresholdIntegral", thresholdint);

    }
    return fSignalEvent;
}

//______________________________________________________________________________
void TRestReadoutAnalysisProcess::EndProcess() {
    if (fReadout != NULL) {
        {
            auto iter = fModuleHitMaps.begin();
            while (iter != fModuleHitMaps.end()) {
                if (fModuleHitMaps[iter->first] != NULL) {
                    TH2D* histo = fModuleHitMaps[iter->first];
                    histo->GetXaxis()->SetTitle("X Channel");
                    histo->GetYaxis()->SetTitle("Y Channel");
                    histo->Write();
                }

                if (fModuleCanvasSave != "none") {
                    if (fModuleHitMaps[iter->first] != NULL) {
                        TH2D* histo = fModuleHitMaps[iter->first];
                        TCanvas* c1 =
                            new TCanvas((TString) "Can_ModuleHitMap" + ToString(iter->first),
                                        (TString) "Hit Map of Module " + ToString(iter->first), 800, 600);
                        histo->Draw("colz");
                        c1->Write();
                        c1->Print((TString)fModuleCanvasSave + "/M" + ToString(iter->first) + "_HitMap.png");
                        delete c1;
                    }

                    auto h0 = fModuleHitMaps[iter->first];
                    h0->SetStats(false);
                    h0->Reset();

                    if (fModuleActivityX[iter->first] != NULL && fModuleActivityY[iter->first] != NULL) {
                        TH1D* h1 = fModuleActivityX[iter->first];
                        TH1D* h2 = fModuleActivityY[iter->first];

                        TCanvas* c1 = new TCanvas(
                            (TString) "Can_ModuleActivity" + ToString(iter->first),
                            (TString) "Channel Activity of Module " + ToString(iter->first), 800, 600);
                        c1->Divide(2, 2, 0, 0);
                        c1->cd(1);
                        h1->SetFillColor(kBlue);
                        h1->SetStats(false);
                        h1->GetYaxis()->SetTitle("Counts");
                        h1->Draw("bar");

                        c1->cd(4);
                        h2->SetFillColor(kBlue);
                        h2->SetStats(false);
                        h2->GetXaxis()->SetTitle("Counts");
                        h2->Draw("hbar");

                        c1->cd(3);
                        h0->Draw("colz");
                        c1->Write();
                        c1->Print((TString)fModuleCanvasSave + "/M" + ToString(iter->first) +
                                  "_Activity.png");
                        delete c1;
                    }

                    if (fModuleBSLSigmaX[iter->first] != NULL && fModuleBSLSigmaY[iter->first] != NULL) {
                        TH2D* h1 = fModuleBSLSigmaX[iter->first];
                        TH2D* h2 = fModuleBSLSigmaY[iter->first];

                        TCanvas* c1 = new TCanvas(
                            (TString) "Can_ModuleBSLS" + ToString(iter->first),
                            (TString) "Channel Baseline Sigma of Module " + ToString(iter->first), 800, 600);
                        c1->Divide(2, 2, 0, 0);
                        c1->cd(1);
                        h1->SetFillColor(kBlue);
                        h1->SetStats(false);
                        h1->GetYaxis()->SetTitle("Baseline Sigma");
                        h1->Draw("colz");

                        c1->cd(4);
                        h2->SetFillColor(kBlue);
                        h2->SetStats(false);
                        h2->GetXaxis()->SetTitle("Baseline Sigma");
                        h2->Draw("colz");

                        c1->cd(3);
                        h0->Draw("colz");
                        c1->Write();
                        c1->Print((TString)fModuleCanvasSave + "/M" + ToString(iter->first) + "_BSLS.png");
                        delete c1;
                    }
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
    fModuleCanvasSave = GetParameter("outputPlotPath", "none");
    if (fModuleCanvasSave != "none") {
        fSingleThreadOnly = true;
        TRestTools::Execute("mkdir -p " + fModuleCanvasSave);
    }

    string moduleHist = GetParameter("modulesHist", "");
    auto histdef = Split(moduleHist, ":");
    for (int i = 0; i < histdef.size(); i++) {
        fModuleHitMaps[StringToInteger(histdef[i])] = NULL;
        fModuleActivityX[StringToInteger(histdef[i])] = NULL;
        fModuleActivityY[StringToInteger(histdef[i])] = NULL;
        fModuleBSLSigmaX[StringToInteger(histdef[i])] = NULL;
        fModuleBSLSigmaY[StringToInteger(histdef[i])] = NULL;
    }
}
