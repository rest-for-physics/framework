///______________________________________________________________________________
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
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestDAQ_TRestRawSignalEvent
#define RestDAQ_TRestRawSignalEvent

#include <iostream>

#include <TArrayD.h>
#include <TAxis.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TPad.h>
#include <TVector2.h>

#include "TRestEvent.h"
#include "TRestRawSignal.h"

class TRestRawSignalEvent : public TRestEvent {
   protected:
    TMultiGraph* mg;     //!
    TGraph* gr;          //!
    Double_t fMinTime;   //!
    Double_t fMaxTime;   //!
    Double_t fMinValue;  //!
    Double_t fMaxValue;  //!

    TVector2 fBaseLineRange = TVector2(-1, -1);  //!
    TVector2 fRange = TVector2(-1, -1);          //!

    std::vector<TRestRawSignal> fSignal;  // Collection of signals that define the event

   private:
    void SetMaxAndMin();

   public:
    Bool_t signalIDExists(Int_t sID) {
        if (GetSignalIndex(sID) == -1) return false;
        return true;
    }

    // Setters
    void AddSignal(TRestRawSignal s);

    void RemoveSignalWithId(Int_t sId);

    void AddChargeToSignal(Int_t sgnlID, Int_t bin, Short_t value);

    void SetTailPoints(Int_t p) {
        for (int n = 0; n < GetNumberOfSignals(); n++) fSignal[n].SetTailPoints(p);
    }

    void SetBaseLineRange(TVector2 blRange) { SetBaseLineRange(blRange.X(), blRange.Y()); }

    void SetBaseLineRange(Int_t from, Int_t to) {
        fBaseLineRange = TVector2(from, to);
        for (int n = 0; n < GetNumberOfSignals(); n++) fSignal[n].CalculateBaseLine(from, to);
    }

    void SetRange(TVector2 range) { SetRange(range.X(), range.Y()); }

    void SetRange(Int_t from, Int_t to) {
        fRange = TVector2(from, to);
        for (int n = 0; n < GetNumberOfSignals(); n++) fSignal[n].SetRange(fRange);
    }

    // Getters
    Int_t GetNumberOfSignals() { return fSignal.size(); }
    TRestRawSignal* GetSignal(Int_t n) { return &fSignal[n]; }

    void PrintSignalIds() {
        for (int n = 0; n < GetNumberOfSignals(); n++) {
            if (n > 0) cout << " , ";
            cout << GetSignal(n)->GetSignalID();
        }
        cout << endl;
    }

    Bool_t isBaseLineInitialized() {
        // If one signal is initialized we assume initialization happened for any signal
        for (int n = 0; n < GetNumberOfSignals(); n++)
            if (fSignal[n].isBaseLineInitialized()) return true;
        return false;
    }

    TRestRawSignal* GetSignalById(Int_t sid) {
        Int_t index = GetSignalIndex(sid);
        if (index < 0) return NULL;

        return &fSignal[index];
    }

    TRestRawSignal* GetMaxSignal();

    Int_t GetLowestWidth(Double_t minPeakAmplitude = 0);
    Double_t GetLowAverageWidth(Int_t nSignals = 5, Double_t minPeakAmplitude = 0);
    Double_t GetAverageWidth(Double_t minPeakAmplitude = 0);

    Int_t GetSignalIndex(Int_t signalID);

    Double_t GetBaseLineAverage();
    Double_t GetBaseLineSigmaAverage();
    //   void SubstractBaselines();
    Double_t GetIntegral();
    Double_t GetThresholdIntegral();

    Double_t GetSlopeIntegral();
    Double_t GetRiseSlope();
    Double_t GetRiseTime();
    Double_t GetTripleMaxIntegral();

    Double_t GetMaxValue();
    Double_t GetMinValue();
    Double_t GetMinTime();
    Double_t GetMaxTime();

    // Default
    void Initialize();
    void PrintEvent();

    TPad* DrawEvent(TString option = "");

    // Construtor
    TRestRawSignalEvent();
    // Destructor
    virtual ~TRestRawSignalEvent();

    ClassDef(TRestRawSignalEvent, 1);  // REST event superclass
};
#endif
