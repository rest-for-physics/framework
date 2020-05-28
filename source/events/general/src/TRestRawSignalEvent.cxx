//______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalEvent.h
///
///             Event class to store DAQ events either from simulation and
///             acquisition
///
///             feb 2017:   First concept
///                 Created from TRestSignalEvent
///                 Javier Galan
///_______________________________________________________________________________

#include <TMath.h>

#include "TRestRawSignalEvent.h"
#include "TRestStringHelper.h"
using namespace std;

ClassImp(TRestRawSignalEvent);
//______________________________________________________________________________
TRestRawSignalEvent::TRestRawSignalEvent() {
    // TRestRawSignalEvent default constructor
    Initialize();
}

//______________________________________________________________________________
TRestRawSignalEvent::~TRestRawSignalEvent() {
    // TRestRawSignalEvent destructor
}

void TRestRawSignalEvent::Initialize() {
    TRestEvent::Initialize();
    fSignal.clear();
    fPad = NULL;
    mg = NULL;
    gr = NULL;
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;
}

void TRestRawSignalEvent::AddSignal(TRestRawSignal s) {
    if (signalIDExists(s.GetSignalID())) {
        cout << "Warning. Signal ID : " << s.GetSignalID()
             << " already exists. Signal will not be added to signal event" << endl;
        return;
    }

    s.CalculateBaseLine(fBaseLineRange.X(), fBaseLineRange.Y());
    s.SetRange(fRange);

    fSignal.push_back(s);
}

void TRestRawSignalEvent::RemoveSignalWithId(Int_t sId) {
    Int_t index = GetSignalIndex(sId);

    if (index == -1) {
        cout << "Warning. Signal ID : " << sId
             << " does not exist. Signal will not be removed from signal event" << endl;
        return;
    }

    fSignal.erase(fSignal.begin() + index);
}

Int_t TRestRawSignalEvent::GetSignalIndex(Int_t signalID) {
    for (int i = 0; i < GetNumberOfSignals(); i++)
        if (fSignal[i].GetSignalID() == signalID) return i;
    return -1;
}

Double_t TRestRawSignalEvent::GetIntegral() {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++) sum += fSignal[i].GetIntegral();

    return sum;
}

/// The result if this method depends on InitializePointsOverThreshold. Arguments are given there.
Double_t TRestRawSignalEvent::GetThresholdIntegral() {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfSignals(); i++) sum += fSignal[i].GetThresholdIntegral();
    return sum;
}

TRestRawSignal* TRestRawSignalEvent::GetMaxSignal() {
    if (GetNumberOfSignals() <= 0) return NULL;

    Double_t max = fSignal[0].GetIntegral();

    Int_t sId;
    for (int i = 0; i < GetNumberOfSignals(); i++) {
        Int_t integ = fSignal[i].GetIntegral();
        if (max < integ) {
            max = integ;
            sId = i;
        }
    }

    return &fSignal[sId];
}

Double_t TRestRawSignalEvent::GetSlopeIntegral() {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++) sum += fSignal[i].GetSlopeIntegral();

    return sum;
}

Double_t TRestRawSignalEvent::GetRiseSlope() {
    Double_t sum = 0;

    Int_t n = 0;
    for (int i = 0; i < GetNumberOfSignals(); i++) {
        if (fSignal[i].GetThresholdIntegral() > 0) {
            sum += fSignal[i].GetSlopeIntegral();
            n++;
        }
    }

    if (n == 0) return 0;

    return sum / n;
}

Double_t TRestRawSignalEvent::GetRiseTime() {
    Double_t sum = 0;

    Int_t n = 0;
    for (int i = 0; i < GetNumberOfSignals(); i++) {
        if (fSignal[i].GetThresholdIntegral() > 0) {
            sum += fSignal[i].GetRiseTime();
            n++;
        }
    }

    if (n == 0) return 0;

    return sum / n;
}

Double_t TRestRawSignalEvent::GetTripleMaxIntegral() {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++)
        if (fSignal[i].GetThresholdIntegral() > 0) sum += fSignal[i].GetTripleMaxIntegral();

    return sum;
}

Double_t TRestRawSignalEvent::GetBaseLineAverage() {
    Double_t baseLineMean = 0;

    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) {
        Double_t baseline = GetSignal(sgnl)->GetBaseLine();
        baseLineMean += baseline;
    }

    return baseLineMean / GetNumberOfSignals();
}

Int_t TRestRawSignalEvent::GetLowestWidth(Double_t minPeakAmplitude) {
    Int_t low = 10000000;

    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) {
        if (GetSignal(sgnl)->GetMaxPeakValue() > minPeakAmplitude) {
            Int_t lW = GetSignal(sgnl)->GetMaxPeakWidth();
            if (low > lW) low = lW;
        }
    }

    return low;
}

Double_t TRestRawSignalEvent::GetAverageWidth(Double_t minPeakAmplitude) {
    Double_t avg = 0;
    Int_t n = 0;
    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) {
        if (GetSignal(sgnl)->GetMaxPeakValue() > minPeakAmplitude) {
            avg += GetSignal(sgnl)->GetMaxPeakWidth();
            n++;
        }
    }

    if (n == 0)
        return 0;
    else
        return avg / n;
}

Double_t TRestRawSignalEvent::GetLowAverageWidth(Int_t nSignals, Double_t minPeakAmplitude) {
    std::vector<Double_t> widths;

    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++)
        if (GetSignal(sgnl)->GetMaxPeakValue() > minPeakAmplitude)
            widths.push_back(GetSignal(sgnl)->GetMaxPeakWidth());

    if (widths.size() == 0) return 0;

    std::sort(widths.begin(), widths.end());

    Int_t nMax = nSignals;
    if (widths.size() < (unsigned int)nSignals) nMax = widths.size();

    Double_t avg = 0;
    for (int n = 0; n < nMax; n++) avg += widths[n];

    return avg / nSignals;
}

Double_t TRestRawSignalEvent::GetBaseLineSigmaAverage() {
    Double_t baseLineSigmaMean = 0;

    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) {
        Double_t baselineSigma = GetSignal(sgnl)->GetBaseLineSigma();
        baseLineSigmaMean += baselineSigma;
    }

    return baseLineSigmaMean / GetNumberOfSignals();
}

/// Perhaps we should not substract baselines on a TRestRawSignal. Just consider it in the observables
/// calculation if a baseline range is provided in the argument (as it is done in
/// InitializeThresholdIntegral). This method should be probably removed.
// void TRestRawSignalEvent::SubstractBaselines() {
//    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) GetSignal(sgnl)->SubstractBaseline();
//}

void TRestRawSignalEvent::AddChargeToSignal(Int_t sgnlID, Int_t bin, Short_t value) {
    Int_t sgnlIndex = GetSignalIndex(sgnlID);
    if (sgnlIndex == -1) {
        sgnlIndex = GetNumberOfSignals();

        TRestRawSignal sgnl(512);  // For the moment we use the default nBins=512
        sgnl.SetSignalID(sgnlID);
        AddSignal(sgnl);
    }

    fSignal[sgnlIndex].IncreaseBinBy(bin, value);
}

void TRestRawSignalEvent::PrintEvent() {
    TRestEvent::PrintEvent();

    for (int i = 0; i < GetNumberOfSignals(); i++) {
        cout << "================================================" << endl;
        cout << "Signal ID : " << fSignal[i].GetSignalID() << endl;
        cout << "Integral : " << fSignal[i].GetIntegral() << endl;
        cout << "------------------------------------------------" << endl;
        fSignal[i].Print();
        cout << "================================================" << endl;
    }
}

// TODO: GetMaxTimeFast, GetMinTimeFast, GetMaxValueFast that return the value
// of fMinTime, fMaxTime, etc
void TRestRawSignalEvent::SetMaxAndMin() {
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 0;
    fMaxTime = -1E10;

    for (int s = 0; s < GetNumberOfSignals(); s++) {
        if (fMinValue > fSignal[s].GetMinValue()) fMinValue = fSignal[s].GetMinValue();
        if (fMaxValue < fSignal[s].GetMaxValue()) fMaxValue = fSignal[s].GetMaxValue();
    }

    if (GetNumberOfSignals() > 0) fMaxTime = fSignal[0].GetNumberOfPoints();
}

Double_t TRestRawSignalEvent::GetMaxValue() {
    SetMaxAndMin();
    return fMaxValue;
}

Double_t TRestRawSignalEvent::GetMinValue() {
    SetMaxAndMin();
    return fMinValue;
}

Double_t TRestRawSignalEvent::GetMinTime() { return 0; }

Double_t TRestRawSignalEvent::GetMaxTime() {
    Double_t maxTime = 512;

    if (GetNumberOfSignals() > 0) maxTime = fSignal[0].GetNumberOfPoints();

    return maxTime;
}

// Draw current event in a Tpad
TPad* TRestRawSignalEvent::DrawEvent(TString option) {
    int nSignals = this->GetNumberOfSignals();

    if (fPad != NULL) {
        for (int n = 0; n < nSignals; n++) {
            delete fSignal[n].fGraph;
            fSignal[n].fGraph = NULL;
        }
        delete fPad;
        fPad = NULL;
    }

    if (nSignals == 0) {
        cout << "Empty event " << endl;
        return NULL;
    }

    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    fPad->Draw();
    fPad->cd();
    fPad->DrawFrame(0, GetMinValue() - 1, GetMaxTime() + 1, GetMaxValue() + 1);

    char title[256];

    if (option == "") {
        sprintf(title, "Event ID %d", this->GetID());

        if (mg != NULL) delete mg;
        mg = new TMultiGraph();
        mg->SetTitle(title);
        mg->GetXaxis()->SetTitle("time bins");
        mg->GetYaxis()->SetTitleOffset(1.4);
        mg->GetYaxis()->SetTitle("Amplitude [ADC units]");

        for (int n = 0; n < nSignals; n++) {
            TGraph* gr = fSignal[n].GetGraph(n + 1);

            mg->Add(gr);
        }

        fPad->cd();
        mg->Draw("");
    } else if (isANumber((string)option)) {
        int signalid = StringToInteger((string)option);
        if (signalid >= fSignal.size()) {
            fPad->SetTitle("No Such Signal");
            return fPad;
        }
        TRestRawSignal& sgn = fSignal[signalid];

        sprintf(title, "Event ID %d, Signal ID. %d", this->GetID(), sgn.GetID());

        gr = sgn.GetGraph(1);
        gr->SetTitle(title);
        gr->GetXaxis()->SetTitle("time bins");
        gr->GetYaxis()->SetTitleOffset(1.4);
        gr->GetYaxis()->SetTitle("Voltage");

        fPad->cd();
        gr->Draw("ALP");
    }

    return fPad;
}
