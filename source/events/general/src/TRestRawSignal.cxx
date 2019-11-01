///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignal.cxx
///
///             Event class to store signals from simulation and acquisition
///             events
///
///             feb 2017:   Created from TRestSignal
///              Javier Galan
///_______________________________________________________________________________

#include "TRestRawSignal.h"
#include <numeric>
using namespace std;

#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>

ClassImp(TRestRawSignal);
//______________________________________________________________________________
TRestRawSignal::TRestRawSignal() {
    // TRestRawSignal default constructor
    fGraph = NULL;
    fSignalID = -1;
    fSignalData.clear();

    fPointsOverThreshold.clear();

    fThresholdIntegral = -1;

    fHeadPoints = 0;
    fTailPoints = 0;
}

TRestRawSignal::TRestRawSignal(Int_t nBins) {
    // TRestRawSignal default constructor
    fGraph = NULL;
    fSignalID = -1;
    fSignalData.clear();

    fPointsOverThreshold.clear();

    for (int n = 0; n < nBins; n++) fSignalData.push_back(0);

    fThresholdIntegral = -1;

    fHeadPoints = 0;
    fTailPoints = 0;
}

//______________________________________________________________________________
TRestRawSignal::~TRestRawSignal() {
    // TRestRawSignal destructor
}

void TRestRawSignal::Initialize() {
    fSignalData.clear();
    fPointsOverThreshold.clear();
    fSignalID = -1;

    fThresholdIntegral = -1;

    fHeadPoints = 0;
    fTailPoints = 0;
}

void TRestRawSignal::Reset() {
    Int_t nBins = GetNumberOfPoints();
    fSignalData.clear();
    for (int n = 0; n < nBins; n++) fSignalData.push_back(0);
}

void TRestRawSignal::AddPoint(Short_t d) { fSignalData.push_back(d); }

void TRestRawSignal::AddCharge(Short_t d) { AddPoint(d); }
void TRestRawSignal::AddDeposit(Short_t d) { AddPoint(d); }

Short_t TRestRawSignal::GetSignalData(Int_t n) {
    if (n >= GetNumberOfPoints()) {
        std::cout << "TRestRawSignal::GetSignalData: outside limits" << std::endl;
        return 0xFFFF;
    }

    return fSignalData[n];
}

void TRestRawSignal::IncreaseBinBy(Int_t bin, Double_t data) {
    if (bin >= GetNumberOfPoints()) {
        std::cout << "TRestRawSignal::IncreaseBinBy: outside limits" << std::endl;
        return;
    }

    fSignalData[bin] += data;
}

Double_t TRestRawSignal::GetIntegral(Int_t startBin, Int_t endBin) {
    if (startBin < 0) startBin = 0;
    if (endBin <= 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();

    Double_t sum = 0;
    for (int i = startBin; i < endBin; i++) sum += GetData(i);

    return sum;
}

void TRestRawSignal::InitializePointsOverThreshold(TVector2 blRange, TVector2 sgnlRange, TVector2 thrPar,
                                                   Int_t nPointsOver, Int_t nPointsFlat) {
    fPointsOverThreshold.clear();

    double bl = this->GetBaseLine(blRange.X(), blRange.Y());
    double rms = this->GetBaseLineSigma(blRange.X(), blRange.Y());

    double pointTh = thrPar.X();
    double signalTh = thrPar.Y();

    double threshold = bl + pointTh * rms;

    for (int i = sgnlRange.X(); i < sgnlRange.Y(); i++) {
        // Filling a pulse with consecutive points that are over threshold
        if (this->GetData(i) > threshold) {
            int pos = i;
            std::vector<double> pulse;
            pulse.push_back(this->GetData(i) - bl);
            i++;

            // If the pulse ends in a flat end above the threshold, the parameter
            // nPointsFlat will serve to artificially end the pulse.
            // If nPointsFlat is big enough, this parameter will not affect the
            // decision to cut this anomalous behaviour. And all points over threshold
            // will be added to the pulse vector.
            int flatN = 0;
            while (i < sgnlRange.Y() && this->GetData(i) > threshold) {
                if (TMath::Abs(this->GetData(i) - this->GetData(i - 1)) > pointTh * rms) {
                    flatN = 0;
                } else {
                    flatN++;
                }

                if (flatN < nPointsFlat) {
                    pulse.push_back(this->GetData(i));
                    i++;
                } else {
                    break;
                }
            }

            if (pulse.size() >= nPointsOver) {
                // auto stdev = TMath::StdDev(begin(pulse), end(pulse));
                // calculate stdev
                double mean = std::accumulate(pulse.begin(), pulse.end(), 0.0) / pulse.size();
                double sq_sum = std::inner_product(pulse.begin(), pulse.end(), pulse.begin(), 0.0);
                double stdev = std::sqrt(sq_sum / pulse.size() - mean * mean);

                if (stdev > signalTh * rms)
                    for (int j = pos; j < i; j++) fPointsOverThreshold.push_back(j);
            }
        }
    }

    InitializeThresholdIntegral(sgnlRange.X(), sgnlRange.Y(), bl);
}

void TRestRawSignal::InitializeThresholdIntegral(Int_t startBin, Int_t endBin, Double_t baseLine) {
    if (startBin < 0) startBin = 0;
    if (endBin <= 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();

    fThresholdIntegral = 0;

    for (unsigned int n = 0; n < fPointsOverThreshold.size(); n++) {
        if (fPointsOverThreshold[n] >= startBin && fPointsOverThreshold[n] < endBin) {
            fThresholdIntegral += GetData(fPointsOverThreshold[n]) - baseLine;
        }
    }
}

Double_t TRestRawSignal::GetSlopeIntegral() {
    if (fThresholdIntegral == -1)
        cout << "REST Warning. TRestRawSignal::GetSlopeIntegral. "
                "InitializePointsOverThreshold should be called first."
             << endl;

    Double_t sum = 0;
    for (unsigned int n = 0; n < fPointsOverThreshold.size(); n++) {
        if (n + 1 >= fPointsOverThreshold.size()) return sum;

        sum += GetData(fPointsOverThreshold[n]);

        if (GetData(fPointsOverThreshold[n + 1]) - GetData(fPointsOverThreshold[n]) < 0) break;
    }
    return sum;
}

Double_t TRestRawSignal::GetRiseSlope() {
    if (fThresholdIntegral == -1)
        cout << "REST Warning. TRestRawSignal::GetRiseSlope. "
                "InitializePointsOverThreshold should be called first."
             << endl;

    if (fPointsOverThreshold.size() < 2) {
        cout << "REST Warning. TRestRawSignal::GetRiseSlope. Less than 2 points!." << endl;
        return 0;
    }

    Int_t maxBin = GetMaxPeakBin() - 1;

    Double_t hP = GetData(maxBin);

    Double_t lP = GetData(fPointsOverThreshold[0]);

    return (hP - lP) / (maxBin - fPointsOverThreshold[0]);
}

Int_t TRestRawSignal::GetRiseTime() {
    if (fThresholdIntegral == -1)
        cout << "REST Warning. TRestRawSignal::GetRiseTime. "
                "InitializePointsOverThreshold should be called first."
             << endl;

    if (fPointsOverThreshold.size() < 2) {
        cout << "REST Warning. TRestRawSignal::GetRiseTime. Less than 2 points!." << endl;
        return 0;
    }

    return GetMaxPeakBin() - fPointsOverThreshold[0];
}

Double_t TRestRawSignal::GetTripleMaxIntegral(Int_t startBin, Int_t endBin) {
    if (startBin < 0) startBin = 0;
    if (endBin <= 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();

    if (fThresholdIntegral == -1) {
        cout << "REST Warning. TRestRawSignal::GetTripleMaxIntegral. "
                "InitializePointsOverThreshold should be called first."
             << endl;
        return 0;
    }

    if (fPointsOverThreshold.size() < 2) {
        cout << "REST Warning. TRestRawSignal::GetTripleMaxIntegral. Points over "
                "threshold = "
             << fPointsOverThreshold.size() << endl;
        return 0;
    }

    Int_t cBin = GetMaxPeakBin(startBin, endBin);

    if (cBin + 1 >= GetNumberOfPoints()) return 0;

    Double_t a1 = GetData(cBin);
    Double_t a2 = GetData(cBin - 1);
    Double_t a3 = GetData(cBin + 1);

    return a1 + a2 + a3;
}

Double_t TRestRawSignal::GetAverage(Int_t startBin, Int_t endBin) {
    if (startBin < 0) startBin = 0;
    if (endBin <= 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();

    Double_t sum = 0;
    for (int i = startBin; i <= endBin; i++) sum += this->GetData(i);

    return sum / (endBin - startBin + 1);
}

Int_t TRestRawSignal::GetMaxPeakWidth(Int_t startBin, Int_t endBin) {
    Int_t mIndex = this->GetMaxPeakBin(startBin, endBin);
    Double_t maxValue = this->GetData(mIndex);

    Double_t value = maxValue;
    Int_t rightIndex = mIndex;
    while (value > maxValue / 2) {
        value = this->GetData(rightIndex);
        rightIndex++;
    }
    Int_t leftIndex = mIndex;
    value = maxValue;
    while (value > maxValue / 2) {
        value = this->GetData(leftIndex);
        leftIndex--;
    }

    return rightIndex - leftIndex;
}

Double_t TRestRawSignal::GetMaxPeakValue(Int_t startBin, Int_t endBin) {
    return GetData(GetMaxPeakBin(startBin, endBin));
}

Int_t TRestRawSignal::GetMaxPeakBin(Int_t startBin, Int_t endBin) {
    Double_t max = -1E10;
    Int_t index = 0;

    if (endBin == 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();
    if (startBin < 0) startBin = 0;

    for (int i = startBin; i < endBin; i++) {
        if (this->GetData(i) > max) {
            max = GetData(i);
            index = i;
        }
    }

    return index;
}

Double_t TRestRawSignal::GetMinPeakValue(Int_t startBin, Int_t endBin) {
    return GetData(GetMinPeakBin(startBin, endBin));
}

Int_t TRestRawSignal::GetMinPeakBin(Int_t startBin, Int_t endBin) {
    Double_t min = 1E10;
    Int_t index = 0;

    if (endBin == 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();
    if (startBin < 0) startBin = 0;

    for (int i = startBin; i < endBin; i++) {
        if (this->GetData(i) < min) {
            min = GetData(i);
            index = i;
        }
    }

    return index;
}

void TRestRawSignal::GetDifferentialSignal(TRestRawSignal* diffSgnl, Int_t smearPoints) {
    diffSgnl->Initialize();

    for (int i = 0; i < smearPoints; i++) diffSgnl->AddPoint(0);

    for (int i = smearPoints; i < this->GetNumberOfPoints() - smearPoints; i++) {
        Double_t value = 0.5 * (this->GetData(i + smearPoints) - GetData(i - smearPoints)) / smearPoints;

        diffSgnl->AddPoint((Short_t)value);
    }

    for (int i = GetNumberOfPoints() - smearPoints; i < GetNumberOfPoints(); i++) diffSgnl->AddPoint(0);
}

void TRestRawSignal::GetWhiteNoiseSignal(TRestRawSignal* noiseSgnl, Double_t noiseLevel) {
    unsigned int seed = (uintptr_t)noiseSgnl + this->GetIntegral();
    TRandom3* fRandom = new TRandom3(seed);
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        // cout << "i : " << i << " random : " <<  (Short_t) fRandom->Gaus(0,
        // noiseLevel) << endl;
        noiseSgnl->AddPoint(this->GetData(i) + (Short_t)fRandom->Gaus(0, noiseLevel));
    }
    // getchar();
    delete fRandom;
}

void TRestRawSignal::GetSignalSmoothed(TRestRawSignal* smthSignal, Int_t averagingPoints) {
    smthSignal->Initialize();

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Double_t sumAvg = GetIntegral(0, averagingPoints) / averagingPoints;
    for (int i = 0; i <= averagingPoints / 2; i++) smthSignal->AddPoint((Short_t)sumAvg);

    for (int i = averagingPoints / 2 + 1; i < GetNumberOfPoints() - averagingPoints / 2; i++) {
        sumAvg -= this->GetData(i - (averagingPoints / 2 + 1)) / averagingPoints;
        sumAvg += this->GetData(i + averagingPoints / 2) / averagingPoints;
        smthSignal->AddPoint((Short_t)sumAvg);
    }

    for (int i = GetNumberOfPoints() - averagingPoints / 2; i < GetNumberOfPoints(); i++)
        smthSignal->AddPoint(sumAvg);
}

Double_t TRestRawSignal::GetBaseLine(Int_t startBin, Int_t endBin) {
    if (endBin - startBin <= 0) return 0.;

    Double_t baseLine = 0;
    for (int i = startBin; i < endBin; i++) baseLine += fSignalData[i];

    return baseLine / (endBin - startBin);
}

Double_t TRestRawSignal::GetStandardDeviation(Int_t startBin, Int_t endBin) {
    Double_t bL = GetBaseLine(startBin, endBin);
    return GetBaseLineSigma(startBin, endBin, bL);
}

Double_t TRestRawSignal::GetBaseLineSigma(Int_t startBin, Int_t endBin, Double_t baseline) {
    Double_t bL = baseline;
    if (bL == 0) bL = GetBaseLine(startBin, endBin);

    Double_t baseLineSigma = 0;
    for (int i = startBin; i < endBin; i++) baseLineSigma += (bL - fSignalData[i]) * (bL - fSignalData[i]);

    return TMath::Sqrt(baseLineSigma / (endBin - startBin));
}

Double_t TRestRawSignal::SubstractBaseline(Int_t startBin, Int_t endBin) {
    Double_t bL = GetBaseLine(startBin, endBin);

    AddOffset((Short_t)-bL);

    return bL;
}

void TRestRawSignal::AddOffset(Short_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalData[i] = fSignalData[i] + offset;
}

void TRestRawSignal::Scale(Double_t value) {
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        Double_t scaledValue = value * fSignalData[i];
        fSignalData[i] = (Short_t)scaledValue;
    }
}

void TRestRawSignal::SignalAddition(TRestRawSignal* inSgnl) {
    if (this->GetNumberOfPoints() != inSgnl->GetNumberOfPoints()) {
        cout << "ERROR : TRestRawSignal::SignalAddition." << endl;
        cout << "I cannot add two signals with different number of points" << endl;
        return;
    }

    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalData[i] += inSgnl->GetData(i);
}

void TRestRawSignal::WriteSignalToTextFile(TString filename) {
    FILE* fff = fopen(filename.Data(), "w");
    for (int i = 0; i < GetNumberOfPoints(); i++) fprintf(fff, "%d\t%d\n", i, GetData(i));
    fclose(fff);
}

void TRestRawSignal::Print() {
    cout << "---------------------" << endl;
    cout << "Signal id : " << this->GetSignalID() << endl;
    cout << "+++++++++++++++++++++" << endl;
    for (int i = 0; i < GetNumberOfPoints(); i++) cout << "Bin : " << i << " Charge : " << GetData(i) << endl;
    cout << "---------------------" << endl;
}

TGraph* TRestRawSignal::GetGraph(Int_t color) {
    if (fGraph != NULL) {
        delete fGraph;
        fGraph = NULL;
    }

    fGraph = new TGraph();

    fGraph->SetLineWidth(2);
    fGraph->SetLineColor(color % 8 + 1);
    fGraph->SetMarkerStyle(7);

    int points = 0;
    for (int n = 0; n < GetNumberOfPoints(); n++) {
        fGraph->SetPoint(points, n, GetData(n));
        points++;
    }

    return fGraph;
}
