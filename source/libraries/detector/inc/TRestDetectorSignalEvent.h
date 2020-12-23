///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalEvent.h
///
///             Event class to store DAQ events either from simulation and
///             acquisition
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestDAQ_TRestDetectorSignalEvent
#define RestDAQ_TRestDetectorSignalEvent

#include <iostream>

#include <TArrayD.h>
#include <TAxis.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TPad.h>

#include "TRestEvent.h"
#include "TRestDetectorSignal.h"

class TRestDetectorSignalEvent : public TRestEvent {
   protected:
#ifndef __CINT__
    Double_t fMinTime;   //!
    Double_t fMaxTime;   //!
    Double_t fMinValue;  //!
    Double_t fMaxValue;  //!
#endif

    std::vector<TRestDetectorSignal> fSignal;  // Collection of signals that define the event

   private:
    void SetMaxAndMin();

   public:
    Bool_t signalIDExists(Int_t sID) {
        if (GetSignalIndex(sID) == -1) return false;
        return true;
    }

    void SortSignals() {
        for (int n = 0; n < GetNumberOfSignals(); n++) fSignal[n].Sort();
    }

    // Setters
    void AddSignal(TRestDetectorSignal s);

    void AddChargeToSignal(Int_t sgnlID, Double_t tm, Double_t chrg);

    // Getters
    Int_t GetNumberOfSignals() { return fSignal.size(); }
    TRestDetectorSignal* GetSignal(Int_t n) { return &fSignal[n]; }

    TRestDetectorSignal* GetSignalById(Int_t sid) {
        Int_t index = GetSignalIndex(sid);
        if (index < 0) return NULL;

        return &fSignal[index];
    }

    Int_t GetSignalIndex(Int_t signalID);

    Double_t GetBaseLineAverage(Int_t startBin, Int_t endBin);
    Double_t GetBaseLineSigmaAverage(Int_t startBin, Int_t endBin);
    void SubstractBaselines(Int_t startBin, Int_t endBin);
    Double_t GetIntegral(Int_t startBin = 0, Int_t endBin = 0);
    //   Double_t GetIntegralWithThreshold(Int_t from, Int_t to, Int_t startBaseline, Int_t endBaseline,
    //                                     Double_t nSigmas, Int_t nPointsOverThreshold,
    //                                     Double_t minPeakAmplitude);

    Double_t GetMaxValue();
    Double_t GetMinValue();
    Double_t GetMinTime();
    Double_t GetMaxTime();

    Double_t GetIntegralWithTime(Double_t startTime, Double_t endTime);

    // Default
    void Initialize();
    void PrintEvent();

    TPad* DrawEvent(TString option = "");

    // Construtor
    TRestDetectorSignalEvent();
    // Destructor
    virtual ~TRestDetectorSignalEvent();

    ClassDef(TRestDetectorSignalEvent, 1);  // REST event superclass
};
#endif
