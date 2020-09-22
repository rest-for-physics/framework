///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPositionMappingProcess.cxx
///
///
///             First implementation of raw signal analysis process into REST_v2
///             Created from TRestSignalAnalysisProcess
///             Date : Jun/2018
///             Author : Ni Kaixiang
///
///_______________________________________________________________________________

#include "TRestPositionMappingProcess.h"

#include <TLegend.h>
#include <TPaveText.h>
#include <TRandom.h>
using namespace std;

ClassImp(TRestPositionMappingProcess)
    //______________________________________________________________________________
    TRestPositionMappingProcess::TRestPositionMappingProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestPositionMappingProcess::~TRestPositionMappingProcess() {}

//______________________________________________________________________________
void TRestPositionMappingProcess::Initialize() {
    SetSectionName(this->ClassName());

    fHitsEvent = NULL;

    fReadout = NULL;
}

//______________________________________________________________________________
void TRestPositionMappingProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();
    fCalib = GetMetadata<TRestGainMap>();
    fGas = GetMetadata<TRestGas>();
    if (fReadout == NULL) {
        if (fCreateGainMap) {
            ferr << "You must set a TRestDetectorReadout metadata object to create gain map!" << endl;
            abort();
        }
    } else {
        double xmin, xmax, ymin, ymax;
        fReadout->GetReadoutPlane(0)->GetBoundaries(xmin, xmax, ymin, ymax);
        fAreaGainMap = new TH2F("areaGainMap", "areaGainMap", fNBinsX, xmin, xmax, fNBinsY, ymin, ymax);
        fAreaCounts = new TH2D("areaCounts", "areaCounts", fNBinsX, xmin, xmax, fNBinsY, ymin, ymax);
        fAreaThrIntegralSum =
            new TH2D("areaThrIntegralSum", "areaThrIntegralSum", fNBinsX, xmin, xmax, fNBinsY, ymin, ymax);
    }

    if (fApplyGainCorrection) {
        if (fCalib == NULL || fCalib->f2DGainMapping == NULL) {
            ferr << "You must set a TRestGainMap metadata object to apply gain correction!" << endl;
            abort();
        }
    }
}

//______________________________________________________________________________
TRestEvent* TRestPositionMappingProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsEvent = (TRestHitsEvent*)evInput;

    double nan = numeric_limits<double>::quiet_NaN();

    Double_t newEnergy = 0;

    SetObservableValue("AreaCorrEnergy", newEnergy);

    double x = fHitsEvent->GetMeanPositionX();
    double y = fHitsEvent->GetMeanPositionY();
    double z = fHitsEvent->GetMeanPositionZ();

    double e = fHitsEvent->GetEnergy();
    double n = fHitsEvent->GetNumberOfHits();

    // cout << x << " " << y << " " << e << " " << n << endl;

    if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
        return fHitsEvent;
    }

    if (fCreateGainMap) {
        if (e > fEnergyCutRange.X() && e < fEnergyCutRange.Y() && n > fNHitsCutRange.X() &&
            n < fNHitsCutRange.Y()) {
            // Calculate mean energy for different small areas as the area gain
            int bin = fAreaThrIntegralSum->FindBin(x, y);
            fAreaThrIntegralSum->AddBinContent(bin, e);
            fAreaCounts->AddBinContent(bin, 1);
        }
    }

    else if (fApplyGainCorrection) {
        double correction = GetCorrection2(x, y);
        correction *= GetCorrection3(x, y, z);

        newEnergy = e * correction;

        for (int i = 0; i < fHitsEvent->GetNumberOfHits(); i++) {
            // fHitsEvent->SetEnergy();
        }
    }

    SetObservableValue("AreaCorrEnergy", newEnergy);

    return fHitsEvent;
}

double TRestPositionMappingProcess::GetCorrection2(double x, double y) {
    int bin = fCalib->f2DGainMapping->FindBin(x, y);
    return fCalib->f2DGainMapping->GetBinContent(bin);
}

double TRestPositionMappingProcess::GetCorrection3(double x, double y, double z) {
    double result = 1;
    if (fCalib->f3DGainMapping != NULL && fCalib->f3DGainMapping->GetEntries() > 0) {
        int bin = fCalib->f3DGainMapping->FindBin(x, y, z);
        result *= fCalib->f2DGainMapping->GetBinContent(bin);
    }
    if (fGas != NULL && fGas->GetElectronLifeTime() != 0) {
        double dt = z / fGas->GetDriftVelocity();
        result *= exp(dt / fGas->GetElectronLifeTime());
    }
    return result;
}

//______________________________________________________________________________
void TRestPositionMappingProcess::EndProcess() {
    if (fCreateGainMap) {
        // Calculate the mean of each bin's spectrum
        double sum = 0;
        double n = 0;
        for (int i = 1; i <= fAreaGainMap->GetNbinsX(); i++) {
            for (int j = 1; j <= fAreaGainMap->GetNbinsY(); j++) {
                if (fAreaCounts->GetBinContent(i, j) > 100) {
                    double meanthrintegral =
                        fAreaThrIntegralSum->GetBinContent(i, j) / fAreaCounts->GetBinContent(i, j);
                    fAreaGainMap->SetBinContent(i, j, meanthrintegral);
                    sum += meanthrintegral;
                    n++;
                }
            }
        }

        // the mean value of all the valued bins
        double meanmean = sum / n;

        // normalize and fill the result
        for (int i = 1; i <= fAreaGainMap->GetNbinsX(); i++) {
            for (int j = 1; j <= fAreaGainMap->GetNbinsY(); j++) {
                if (fAreaGainMap->GetBinContent(i, j) == 0) {
                    fAreaGainMap->SetBinContent(i, j, 1);
                } else {
                    fAreaGainMap->SetBinContent(i, j, meanmean / fAreaGainMap->GetBinContent(i, j));
                }
            }
        }

        if (fCalib == NULL) {
            fCalib = new TRestGainMap();
        }
        fCalib->f2DGainMapping = fAreaGainMap;
        fCalib->SetName("PositionCalibration");

        TRestRun* r = new TRestRun();
        r->SetOutputFileName(fMappingSave);
        r->AddMetadata(fCalib);
        r->AddMetadata(fReadout);
        r->FormOutputFile();
        if (fAreaGainMap != NULL) fAreaGainMap->Write();
    }
}

//______________________________________________________________________________
// setting amplification:
// <parameter name="modulesAmp" value = "2-1:5-1.2:6-0.8:8-0.9" />
// setting readout modules to draw:
// <parameter name="modulesHist" value="2:5:6:8"/>
void TRestPositionMappingProcess::InitFromConfigFile() {
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

    fEnergyCutRange = StringTo2DVector(GetParameter("energyRange", "(0,1e9)"));
    fNHitsCutRange = StringTo2DVector(GetParameter("nHitsRange", "(4,14)"));
    fMappingSave = GetParameter("save", "calib.root");

    fNBinsX = StringToDouble(GetParameter("nBinsX", "100"));
    fNBinsY = StringToDouble(GetParameter("nBinsY", "100"));
    fNBinsZ = StringToDouble(GetParameter("nBinsZ", "100"));
}
