//______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEvent.h
///
///             Event class to store DAQ events either from simulation and
///             acquisition
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia/Javier Galan
///
///	       feb 2016: Added titles to axis in DrawGraph using TMultiGraph
///		  Javier Gracia
///_______________________________________________________________________________

#include <TMath.h>

#include "TRestSignalEvent.h"
using namespace std;

ClassImp(TRestSignalEvent)
    //______________________________________________________________________________
    TRestSignalEvent::TRestSignalEvent() {
    // TRestSignalEvent default constructor
    Initialize();
}

//______________________________________________________________________________
TRestSignalEvent::~TRestSignalEvent() {
    // TRestSignalEvent destructor
}

void TRestSignalEvent::Initialize() {
    TRestEvent::Initialize();
    fSignal.clear();
    fPad = NULL;
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;
}

void TRestSignalEvent::AddSignal(TRestSignal s) {
    if (signalIDExists(s.GetSignalID())) {
        cout << "Warning. Signal ID : " << s.GetSignalID()
             << " already exists. Signal will not be added to signal event" << endl;
        return;
    }

    fSignal.push_back(s);
}

Int_t TRestSignalEvent::GetSignalIndex(Int_t signalID) {
    for (int i = 0; i < GetNumberOfSignals(); i++)
        if (fSignal[i].GetSignalID() == signalID) return i;
    return -1;
}

Double_t TRestSignalEvent::GetIntegral(Int_t startBin, Int_t endBin) {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++) sum += fSignal[i].GetIntegral(startBin, endBin);

    return sum;
}

Double_t TRestSignalEvent::GetIntegralWithTime(Double_t startTime, Double_t endTime) {
    Double_t sum = 0;
    for (int n = 0; n < GetNumberOfSignals(); n++) sum += fSignal[n].GetIntegralWithTime(startTime, endTime);

    return sum;
}

Double_t TRestSignalEvent::GetIntegralWithThreshold(Int_t from, Int_t to, Int_t startBaseline,
                                                    Int_t endBaseline, Double_t nSigmas,
                                                    Int_t nPointsOverThreshold, Double_t minPeakAmplitude) {
    Double_t sum = 0;

    for (int i = 0; i < GetNumberOfSignals(); i++)
        sum += fSignal[i].GetIntegralWithThreshold(from, to, startBaseline, endBaseline, nSigmas,
                                                   nPointsOverThreshold, minPeakAmplitude);

    return sum;
}

Double_t TRestSignalEvent::GetBaseLineAverage(Int_t startBin, Int_t endBin) {
    Double_t baseLineMean = 0;

    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) {
        Double_t baseline = GetSignal(sgnl)->GetBaseLine(startBin, endBin);
        baseLineMean += baseline;
    }

    return baseLineMean / GetNumberOfSignals();
}

Double_t TRestSignalEvent::GetBaseLineSigmaAverage(Int_t startBin, Int_t endBin) {
    Double_t baseLineSigmaMean = 0;

    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++) {
        Double_t baselineSigma = GetSignal(sgnl)->GetBaseLineSigma(startBin, endBin);
        baseLineSigmaMean += baselineSigma;
    }

    return baseLineSigmaMean / GetNumberOfSignals();
}

void TRestSignalEvent::SubstractBaselines(Int_t startBin, Int_t endBin) {
    for (int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++)
        GetSignal(sgnl)->SubstractBaseline(startBin, endBin);
}

void TRestSignalEvent::AddChargeToSignal(Int_t sgnlID, Double_t tm, Double_t chrg) {
    Int_t sgnlIndex = GetSignalIndex(sgnlID);
    if (sgnlIndex == -1) {
        sgnlIndex = GetNumberOfSignals();

        TRestSignal sgnl;
        sgnl.SetSignalID(sgnlID);
        AddSignal(sgnl);
    }

    fSignal[sgnlIndex].AddDeposit(tm, chrg);
}

void TRestSignalEvent::PrintEvent() {
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
void TRestSignalEvent::SetMaxAndMin() {
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    for (int s = 0; s < GetNumberOfSignals(); s++) {
        if (fMinTime > fSignal[s].GetMinTime()) fMinTime = fSignal[s].GetMinTime();
        if (fMaxTime < fSignal[s].GetMaxTime()) fMaxTime = fSignal[s].GetMaxTime();

        if (fMinValue > fSignal[s].GetMinValue()) fMinValue = fSignal[s].GetMinValue();
        if (fMaxValue < fSignal[s].GetMaxValue()) fMaxValue = fSignal[s].GetMaxValue();
    }
}

Double_t TRestSignalEvent::GetMaxValue() {
    SetMaxAndMin();
    return fMaxValue;
}

Double_t TRestSignalEvent::GetMinValue() {
    SetMaxAndMin();
    return fMinValue;
}

Double_t TRestSignalEvent::GetMinTime() {
    Double_t minTime = 1.e10;
    for (int s = 0; s < GetNumberOfSignals(); s++)
        if (minTime > fSignal[s].GetMinTime()) minTime = fSignal[s].GetMinTime();
    return minTime;
}

Double_t TRestSignalEvent::GetMaxTime() {
    Double_t maxTime = -1.e10;
    for (int s = 0; s < GetNumberOfSignals(); s++)
        if (maxTime < fSignal[s].GetMaxTime()) maxTime = fSignal[s].GetMaxTime();
    return maxTime;
}

// Draw current event in a Tpad
TPad* TRestSignalEvent::DrawEvent(TString option) {
    if (fPad != NULL) {
        delete fPad;
        fPad = NULL;
    }

    int nSignals = this->GetNumberOfSignals();

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
    fPad->DrawFrame(GetMinTime(), 0, GetMaxTime(), GetMaxValue());

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());

    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle(title);
    mg->GetXaxis()->SetTitle("Drift time [us]");
    mg->GetXaxis()->SetTitleOffset(1.1);
    mg->GetYaxis()->SetTitle("Energy [keV]");
    mg->GetYaxis()->SetTitleOffset(0.8);

    mg->GetYaxis()->SetTitleSize(1.4 * mg->GetYaxis()->GetTitleSize());
    mg->GetXaxis()->SetTitleSize(1.4 * mg->GetXaxis()->GetTitleSize());
    mg->GetYaxis()->SetLabelSize(1.25 * mg->GetYaxis()->GetLabelSize());
    mg->GetXaxis()->SetLabelSize(1.25 * mg->GetXaxis()->GetLabelSize());

    for (int n = 0; n < nSignals; n++) {
        TGraph* gr = fSignal[n].GetGraph(n + 1);

        mg->Add(gr);
    }

    fPad->cd();
    mg->Draw("");

    return fPad;
}
