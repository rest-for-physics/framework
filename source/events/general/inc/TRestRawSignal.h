///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignal.h
///
///             Event class to store signals fromm simulation and acquisition
///             events
///
///             feb 2017:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestRawSignal
#define RestCore_TRestRawSignal

#include <iostream>

#include <TGraph.h>
#include <TObject.h>
#include <TString.h>
#include <TVector2.h>

class TRestRawSignal : public TObject {
   private:
    /// This method will be called each time InitializePointsOverThreshold is called to re-define the value of
    /// fThresholdIntegral. Thats why this method is private.
    void InitializeThresholdIntegral(Int_t startBin, Int_t endBin, Double_t baseLine);

   protected:
    Int_t fSignalID;

    std::vector<Short_t> fSignalData;  // Vector with the data of the signal

   public:
    TGraph* fGraph;  //!

    std::vector<Int_t> fPointsOverThreshold;  //!

    Double_t fThresholdIntegral = -1;  //!

    Int_t fHeadPoints;  //!
    Int_t fTailPoints;  //!

    // Getters
    Short_t GetData(Int_t n) { return GetSignalData(n); }
    Int_t GetSignalID() { return fSignalID; }
    Int_t GetID() { return fSignalID; }

    Int_t GetNumberOfPoints() { return fSignalData.size(); }

    std::vector<Int_t> GetPointsOverThreshold() { return fPointsOverThreshold; }

    void InitializePointsOverThreshold(TVector2 blRange, TVector2 sgnlRange, TVector2 thrPar,
                                       Int_t nPointsOver, Int_t nPointsFlat = 512);

    Double_t GetMaxValue(Int_t startBin = 0, Int_t endBin = 0) { return GetMaxPeakValue(startBin, endBin); }
    Double_t GetMinValue(Int_t startBin = 0, Int_t endBin = 0) { return GetMinPeakValue(startBin, endBin); }

    // Setters
    void SetSignalID(Int_t sID) { fSignalID = sID; }
    void SetID(Int_t sID) { fSignalID = sID; }

    void Reset();

    void Initialize();

    void AddPoint(Short_t d);
    void AddCharge(Short_t d);
    void AddDeposit(Short_t d);

    Short_t GetSignalData(Int_t n);

    void IncreaseBinBy(Int_t bin, Double_t data);

    Double_t GetIntegral(Int_t startBin = 0, Int_t endBin = 0);

    void SetHeadPoints(Int_t p) { fHeadPoints = p; }
    void SetTailPoints(Int_t p) { fTailPoints = p; }

    Double_t GetIntegralWithThreshold() { return GetThresholdIntegral(); }
    Double_t GetThresholdIntegral() {
        if (fThresholdIntegral == -1)
            std::cout << "TRestRawSignal::GetThresholdIntegral. InitializePointsOverThreshold should be "
                         "called first!"
                      << std::endl;
        return fThresholdIntegral;
    }

    Double_t GetSlopeIntegral();
    Double_t GetRiseSlope();
    Int_t GetRiseTime();
    Double_t GetTripleMaxIntegral(Int_t startBin = 0, Int_t endBin = 0);

    Double_t GetAverage(Int_t startBin = 0, Int_t endBin = 0);

    Int_t GetMaxPeakWidth(Int_t startBin = 0, Int_t endBin = 0);
    Double_t GetMaxPeakValue(Int_t startBin = 0, Int_t endBin = 0);
    Int_t GetMaxPeakBin(Int_t startBin = 0, Int_t endBin = 0);

    Double_t GetMinPeakValue(Int_t startBin = 0, Int_t endBin = 0);
    Int_t GetMinPeakBin(Int_t startBin = 0, Int_t endBin = 0);

    void GetDifferentialSignal(TRestRawSignal* diffSgnl, Int_t smearPoints);
    void GetSignalSmoothed(TRestRawSignal* smthSignal, Int_t averagingPoints);
    void GetWhiteNoiseSignal(TRestRawSignal* noiseSgnl, Double_t noiseLevel = 1.);

    Double_t GetBaseLine(Int_t startBin, Int_t endBin);
    Double_t GetStandardDeviation(Int_t startBin, Int_t endBin);
    Double_t GetBaseLineSigma(Int_t startBin, Int_t endBin, Double_t baseline = 0);
    Double_t SubstractBaseline(Int_t startBin, Int_t endBin);
    void AddOffset(Short_t offset);
    void SignalAddition(TRestRawSignal* inSgnl);

    void Scale(Double_t value);

    void WriteSignalToTextFile(TString filename);
    void Print();

    TGraph* GetGraph(Int_t color = 1);

    // Construtor
    TRestRawSignal();
    TRestRawSignal(Int_t nBins);
    // Destructor
    ~TRestRawSignal();

    ClassDef(TRestRawSignal, 1);
};
#endif
