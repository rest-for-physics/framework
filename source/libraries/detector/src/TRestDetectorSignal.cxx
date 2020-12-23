///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignal.cxx
///
///             Event class to store signals from simulation and acquisition
///             events
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t>
/// to
///< Float_t> 	            JuanAn Garcia
//      dec 2015:
//
//              Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorSignal.h"
using namespace std;

#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>

ClassImp(TRestDetectorSignal)
    //______________________________________________________________________________
    TRestDetectorSignal::TRestDetectorSignal() {
    // TRestDetectorSignal default constructor
    fGraph = NULL;
    fSignalID = -1;
    fSignalTime.clear();
    fSignalCharge.clear();

    fPointsOverThreshold.clear();
}

//______________________________________________________________________________
TRestDetectorSignal::~TRestDetectorSignal() {
    // TRestDetectorSignal destructor
}

void TRestDetectorSignal::NewPoint(Float_t time, Float_t data) {
    fSignalTime.push_back(time);
    fSignalCharge.push_back(data);
}

void TRestDetectorSignal::AddPoint(TVector2 p) {
    Int_t index = GetTimeIndex(p.X());
    Float_t x = p.X();
    Float_t y = p.Y();

    if (index >= 0) {
        fSignalTime[index] = x;
        fSignalCharge[index] += y;
    } else {
        fSignalTime.push_back(x);
        fSignalCharge.push_back(y);
    }
}
void TRestDetectorSignal::SetPoint(TVector2 p) {
    Int_t index = GetTimeIndex(p.X());
    Float_t x = p.X();
    Float_t y = p.Y();

    if (index >= 0) {
        fSignalTime[index] = x;
        fSignalCharge[index] = y;
    } else {
        fSignalTime.push_back(x);
        fSignalCharge.push_back(y);
    }
}

void TRestDetectorSignal::AddPoint(Double_t t, Double_t d) {
    TVector2 p(t, d);
    AddPoint(p);
}
void TRestDetectorSignal::AddCharge(Double_t t, Double_t d) {
    TVector2 p(t, d);
    AddPoint(p);
}
void TRestDetectorSignal::AddDeposit(Double_t t, Double_t d) {
    TVector2 p(t, d);
    AddPoint(p);
}

void TRestDetectorSignal::SetPoint(Double_t t, Double_t d) {
    TVector2 p(t, d);
    SetPoint(p);
}
void TRestDetectorSignal::SetPoint(Int_t index, Double_t t, Double_t d) {
    fSignalTime[index] = t;
    fSignalCharge[index] = d;
}

Double_t TRestDetectorSignal::GetIntegral(Int_t startBin, Int_t endBin) {
    if (startBin < 0) startBin = 0;
    if (endBin <= 0 || endBin > GetNumberOfPoints()) endBin = GetNumberOfPoints();

    Double_t sum = 0;
    for (int i = startBin; i < endBin; i++) sum += GetData(i);

    return sum;
}

void TRestDetectorSignal::Normalize(Double_t scale) {
    Double_t sum = GetIntegral();

    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = scale * GetData(i) / sum;
}

Double_t TRestDetectorSignal::GetIntegralWithTime(Double_t startTime, Double_t endTime) {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfPoints(); i++)
        if (GetTime(i) >= startTime && GetTime(i) < endTime) sum += GetData(i);

    return sum;
}

Double_t TRestDetectorSignal::GetMaxPeakWithTime(Double_t startTime, Double_t endTime) {
    Double_t max = -1E10;

    for (int i = 0; i < GetNumberOfPoints(); i++)
        if (GetTime(i) >= startTime && GetTime(i) < endTime) {
            if (this->GetData(i) > max) max = GetData(i);
        }

    return max;
}

/* {{{
Double_t TRestDetectorSignal::GetIntegralWithThreshold(Int_t from, Int_t to, Int_t startBaseline, Int_t endBaseline,
                                               Double_t nSigmas, Int_t nPointsOverThreshold,
                                               Double_t nMinSigmas) {
    if (startBaseline < 0) startBaseline = 0;
    if (endBaseline <= 0 || endBaseline > GetNumberOfPoints()) endBaseline = GetNumberOfPoints();

    Double_t baseLine = GetBaseLine(startBaseline, endBaseline);

    Double_t pointThreshold = nSigmas * GetBaseLineSigma(startBaseline, endBaseline);
    Double_t signalThreshold = nMinSigmas * GetBaseLineSigma(startBaseline, endBaseline);

    return GetIntegralWithThreshold(from, to, baseLine, pointThreshold, nPointsOverThreshold,
                                    signalThreshold);
}

Double_t TRestDetectorSignal::GetIntegralWithThreshold(Int_t from, Int_t to, Double_t baseline,
                                               Double_t pointThreshold, Int_t nPointsOverThreshold,
                                               Double_t signalThreshold) {
    Double_t sum = 0;
    Int_t nPoints = 0;
    fPointsOverThreshold.clear();

    if (to > GetNumberOfPoints()) to = GetNumberOfPoints();

    Float_t maxValue = 0;
    for (int i = from; i < to; i++) {
        if (GetData(i) > baseline + pointThreshold) {
            if (GetData(i) > maxValue) maxValue = GetData(i);
            nPoints++;
        } else {
            if (nPoints >= nPointsOverThreshold) {
                Double_t sig = GetStandardDeviation(i - nPoints, i);
                if (sig > signalThreshold) {
                    for (int j = i - nPoints; j < i; j++) {
                        sum += this->GetData(j);
                        fPointsOverThreshold.push_back(j);
                    }
                }
            }
            nPoints = 0;
            maxValue = 0;
        }
    }

    if (nPoints >= nPointsOverThreshold) {
        Double_t sig = GetStandardDeviation(to - nPoints, to);
        if (sig > signalThreshold) {
            for (int j = to - nPoints; j < to; j++) {
                sum += this->GetData(j);
                fPointsOverThreshold.push_back(j);
            }
        }
    }

    return sum;
}
}}} */

Double_t TRestDetectorSignal::GetAverage(Int_t start, Int_t end) {
    this->Sort();

    if (end == 0) end = this->GetNumberOfPoints();

    Double_t sum = 0;
    for (int i = start; i <= end; i++) {
        sum += this->GetData(i);
    }
    return sum / (end - start + 1);
}

Int_t TRestDetectorSignal::GetMaxPeakWidth() {
    this->Sort();

    Int_t mIndex = this->GetMaxIndex();
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

Double_t TRestDetectorSignal::GetMaxPeakValue() { return GetData(GetMaxIndex()); }

Int_t TRestDetectorSignal::GetMaxIndex(Int_t from, Int_t to) {
    Double_t max = -1E10;
    Int_t index = 0;

    if (from < 0) from = 0;
    if (to > GetNumberOfPoints()) to = GetNumberOfPoints();

    if (to == 0) to = GetNumberOfPoints();

    for (int i = from; i < to; i++) {
        if (this->GetData(i) > max) {
            max = GetData(i);
            index = i;
        }
    }

    return index;
}

Double_t TRestDetectorSignal::GetMaxPeakTime(Int_t from, Int_t to) { return GetTime(GetMaxIndex(from, to)); }

Double_t TRestDetectorSignal::GetMinPeakValue() { return GetData(GetMinIndex()); }

Int_t TRestDetectorSignal::GetMinIndex() {
    Double_t min = 1E10;
    Int_t index = 0;

    for (int i = 0; i < GetNumberOfPoints(); i++) {
        if (this->GetData(i) < min) {
            min = GetData(i);
            index = i;
        }
    }

    return index;
}

Double_t TRestDetectorSignal::GetMinTime() {
    Double_t minTime = 1E10;
    for (int n = 0; n < GetNumberOfPoints(); n++)
        if (minTime > fSignalTime[n]) minTime = fSignalTime[n];

    return minTime;
}

Double_t TRestDetectorSignal::GetMaxTime() {
    Double_t maxTime = -1E10;
    for (int n = 0; n < GetNumberOfPoints(); n++)
        if (maxTime < fSignalTime[n]) maxTime = fSignalTime[n];

    return maxTime;
}

Int_t TRestDetectorSignal::GetTimeIndex(Double_t t) {
    Float_t time = t;

    for (int n = 0; n < GetNumberOfPoints(); n++)
        if (time == fSignalTime[n]) return n;
    return -1;
}

Bool_t TRestDetectorSignal::isSorted() {
    for (int i = 0; i < GetNumberOfPoints() - 1; i++) {
        if (GetTime(i + 1) < GetTime(i)) return false;
    }
    return true;
}

void TRestDetectorSignal::Sort() {
    while (!isSorted()) {
        for (int i = 0; i < GetNumberOfPoints(); i++) {
            for (int j = i; j < GetNumberOfPoints(); j++) {
                if (GetTime(i) > GetTime(j)) {
                    iter_swap(fSignalTime.begin() + i, fSignalTime.begin() + j);
                    iter_swap(fSignalCharge.begin() + i, fSignalCharge.begin() + j);
                }
            }
        }
    }
}

void TRestDetectorSignal::GetDifferentialSignal(TRestDetectorSignal* diffSgnl, Int_t smearPoints) {
    this->Sort();

    for (int i = 0; i < smearPoints; i++) diffSgnl->AddPoint(GetTime(i), 0);

    for (int i = smearPoints; i < this->GetNumberOfPoints() - smearPoints; i++) {
        Double_t value = (this->GetData(i + smearPoints) - GetData(i - smearPoints)) /
                         (GetTime(i + smearPoints) - GetTime(i - smearPoints));
        Double_t time = (GetTime(i + smearPoints) + GetTime(i - smearPoints)) / 2.;

        diffSgnl->AddPoint(time, value);
    }

    for (int i = GetNumberOfPoints() - smearPoints; i < GetNumberOfPoints(); i++)
        diffSgnl->AddPoint(GetTime(i), 0);
}

void TRestDetectorSignal::GetSignalDelayed(TRestDetectorSignal* delayedSignal, Int_t delay) {
    this->Sort();

    for (int i = 0; i < delay; i++) delayedSignal->AddPoint(GetTime(i), GetData(i));

    for (int i = delay; i < GetNumberOfPoints(); i++) delayedSignal->AddPoint(GetTime(i), GetData(i - delay));
}

void TRestDetectorSignal::GetSignalSmoothed(TRestDetectorSignal* smthSignal, Int_t averagingPoints) {
    this->Sort();

    averagingPoints = (averagingPoints / 2) * 2 + 1;  // make it odd >= averagingPoints

    Double_t sum = GetIntegral(0, averagingPoints);
    for (int i = 0; i <= averagingPoints / 2; i++) smthSignal->AddPoint(GetTime(i), sum / averagingPoints);

    for (int i = averagingPoints / 2 + 1; i < GetNumberOfPoints() - averagingPoints / 2; i++) {
        sum -= this->GetData(i - (averagingPoints / 2 + 1));
        sum += this->GetData(i + averagingPoints / 2);
        smthSignal->AddPoint(this->GetTime(i), sum / averagingPoints);
    }

    for (int i = GetNumberOfPoints() - averagingPoints / 2; i < GetNumberOfPoints(); i++)
        smthSignal->AddPoint(GetTime(i), sum / averagingPoints);
}

Double_t TRestDetectorSignal::GetBaseLine(Int_t startBin, Int_t endBin) {
    if (endBin - startBin <= 0) return 0.;

    Double_t baseLine = 0;
    for (int i = startBin; i < endBin; i++) baseLine += fSignalCharge[i];

    return baseLine / (endBin - startBin);
}

Double_t TRestDetectorSignal::GetStandardDeviation(Int_t startBin, Int_t endBin) {
    Double_t bL = GetBaseLine(startBin, endBin);
    return GetBaseLineSigma(startBin, endBin, bL);
}

Double_t TRestDetectorSignal::GetBaseLineSigma(Int_t startBin, Int_t endBin, Double_t baseline) {
    Double_t bL = baseline;
    if (bL == 0) bL = GetBaseLine(startBin, endBin);

    Double_t baseLineSigma = 0;
    for (int i = startBin; i < endBin; i++)
        baseLineSigma += (bL - fSignalCharge[i]) * (bL - fSignalCharge[i]);

    return TMath::Sqrt(baseLineSigma / (endBin - startBin));
}

Double_t TRestDetectorSignal::SubstractBaseline(Int_t startBin, Int_t endBin) {
    Double_t bL = GetBaseLine(startBin, endBin);

    AddOffset(-bL);

    return bL;
}

void TRestDetectorSignal::AddOffset(Double_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = fSignalCharge[i] + offset;
}

void TRestDetectorSignal::MultiplySignalBy(Double_t factor) {
    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] = factor * fSignalCharge[i];
}

void TRestDetectorSignal::ExponentialConvolution(Double_t fromTime, Double_t decayTime, Double_t offset) {
    for (int i = 0; i < GetNumberOfPoints(); i++) {
        if (fSignalTime[i] > fromTime)
            fSignalCharge[i] =
                (fSignalCharge[i] - offset) * exp(-(fSignalTime[i] - fromTime) / decayTime) + offset;
    }
}

void TRestDetectorSignal::SignalAddition(TRestDetectorSignal* inSgnl) {
    if (this->GetNumberOfPoints() != inSgnl->GetNumberOfPoints()) {
        cout << "ERROR : I cannot add two signals with different number of points" << endl;
        return;
    }

    Int_t badSignalTimes = 0;

    for (int i = 0; i < GetNumberOfPoints(); i++)
        if (GetTime(i) != inSgnl->GetTime(i)) {
            cout << "Time : " << GetTime(i) << " != " << inSgnl->GetTime(i) << endl;
            badSignalTimes++;
        }

    if (badSignalTimes) {
        cout << "ERROR : The times of signal addition must be the same" << endl;
        return;
    }

    for (int i = 0; i < GetNumberOfPoints(); i++) fSignalCharge[i] += inSgnl->GetData(i);
}

void TRestDetectorSignal::AddGaussianSignal(Double_t amp, Double_t sigma, Double_t time, Int_t N, Double_t fromTime,
                                    Double_t toTime) {
    for (int i = 0; i < N; i++) {
        Double_t tme = fromTime + (double)i / (N - 1) * (toTime - fromTime);

        Double_t dta = 300 + amp * TMath::Exp(-0.5 * (tme - time) * (tme - time) / sigma / sigma);

        cout << "T : " << tme << " D : " << dta << endl;
        AddPoint(tme, dta);
    }
}

void TRestDetectorSignal::GetWhiteNoiseSignal(TRestDetectorSignal* noiseSgnl, Double_t noiseLevel) {
    this->Sort();

    for (int i = 0; i < GetNumberOfPoints(); i++) {
        TRandom3* fRandom = new TRandom3(0);

        noiseSgnl->AddPoint(GetTime(i), GetData(i) + fRandom->Gaus(0, noiseLevel));

        delete fRandom;
    }
}

void TRestDetectorSignal::GetSignalGaussianConvolution(TRestDetectorSignal* convSgnl, Double_t sigma, Int_t nSigmas) {
    this->Sort();

    Int_t nPoints = GetMaxTime() - GetMinTime();
    TF1* fGaus = new TF1("fGauss", "exp(-0.5*((x-[1])/[2])**2)", -nPoints, nPoints);
    sigma = sigma * 1000.;          // conversion to nanoseconds
    fGaus->SetParameter(2, sigma);  // the width of the gaussian is set

    Double_t totChargeInitial = 0.;
    Double_t totChargeFinal = 0.;

    Double_t sum;

    // We calculate the charge of the event before convolution
    for (int i = 0; i < GetNumberOfPoints(); i++) totChargeInitial += fSignalCharge[i];

    // The gaussian convolution of the initial signal is performed
    for (int i = GetMinTime() - nSigmas * sigma; i < GetMaxTime() + nSigmas * sigma; i++) {
        for (int j = 0; j < GetNumberOfPoints(); j++) {
            if (TMath::Abs(i - GetTime(j)) > nSigmas * sigma) continue;
            if (TMath::Abs(i - GetTime(j)) > nSigmas * sigma && i < GetTime(j)) break;

            fGaus->SetParameter(1, GetTime(j));
            sum = fSignalCharge[j] / TMath::Sqrt(2. * TMath::Pi()) / sigma * fGaus->Integral(i, i + 1);

            convSgnl->AddPoint(i, sum);
            totChargeFinal += sum;
        }
    }

    cout << "Initial charge of the pulse " << totChargeInitial << endl;
    cout << "Final charge of the pulse " << totChargeFinal << endl;
}

void TRestDetectorSignal::WriteSignalToTextFile(TString filename) {
    FILE* fff = fopen(filename.Data(), "w");
    for (int i = 0; i < GetNumberOfPoints(); i++) fprintf(fff, "%e\t%e\n", GetTime(i), GetData(i));
    fclose(fff);
}

void TRestDetectorSignal::Print() {
    for (int i = 0; i < GetNumberOfPoints(); i++)
        cout << "Time : " << GetTime(i) << " Charge : " << GetData(i) << endl;
}

TGraph* TRestDetectorSignal::GetGraph(Int_t color) {
    if (fGraph != NULL) {
        delete fGraph;
        fGraph = NULL;
    }

    fGraph = new TGraph();

    //   cout << "Signal ID " << this->GetSignalID( ) << " points " <<
    //   this->GetNumberOfPoints() << endl;

    fGraph->SetLineWidth(2);
    fGraph->SetLineColor(color);
    fGraph->SetMarkerStyle(7);

    int points = 0;
    for (int n = 0; n < GetNumberOfPoints(); n++) {
        fGraph->SetPoint(points, GetTime(n), GetData(n));
        points++;
    }

    return fGraph;
}
