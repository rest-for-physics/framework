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
    void CalculateThresholdIntegral();

    /// This method is called each time we call CalculateBaseLine
    void CalculateBaseLineSigma(Int_t startBin, Int_t endBin);

   protected:
    Int_t fSignalID;

    std::vector<Short_t> fSignalData;  // Vector with the data of the signal

   public:
    TGraph* fGraph;  //!

    std::vector<Int_t> fPointsOverThreshold;  //!

    Double_t fThresholdIntegral = -1;  //!

    Int_t fHeadPoints;  //!
    Int_t fTailPoints;  //!

    /// This baseline value will be substracted from GetData for any raw signal observable calculation.
    Double_t fBaseLine = 0;       //!
    Double_t fBaseLineSigma = 0;  //!

    /// Any signal calculation will be restricted to the following range definition.
    TVector2 fRange = TVector2(0, 0);  //!

    // Getters
    Double_t GetData(Int_t n) { return (Double_t)GetSignalData(n) - fBaseLine; }
    Short_t GetSignalData(Int_t n);

    Int_t GetSignalID() { return fSignalID; }
    Int_t GetID() { return fSignalID; }

    Int_t GetNumberOfPoints() { return fSignalData.size(); }

    std::vector<Int_t> GetPointsOverThreshold() { return fPointsOverThreshold; }

    void InitializePointsOverThreshold(TVector2 thrPar, Int_t nPointsOver, Int_t nPointsFlat = 512);

    Double_t GetMaxValue() { return GetMaxPeakValue(); }
    Double_t GetMinValue() { return GetMinPeakValue(); }

    // Setters
    void SetSignalID(Int_t sID) { fSignalID = sID; }
    void SetID(Int_t sID) { fSignalID = sID; }

    void Reset();

    void Initialize();

    void AddPoint(Short_t d);
    void AddCharge(Short_t d);
    void AddDeposit(Short_t d);

    void IncreaseBinBy(Int_t bin, Double_t data);

    void SetHeadPoints(Int_t p) { fHeadPoints = p; }
    void SetTailPoints(Int_t p) { fTailPoints = p; }

    void SetRange(TVector2 rng) { fRange = rng; }
    void SetRange(Int_t from, Int_t to) { fRange = TVector2(from, to); }

    Int_t GetHeadPoints() { return fHeadPoints; }
    Int_t GetTailPoints() { return fTailPoints; }

    Double_t GetBaseLine() { return fBaseLine; }
    Double_t GetBaseLineSigma() { return fBaseLineSigma; }
    TVector2 GetRange() { return fRange; }

    Double_t GetIntegral();

    Double_t GetIntegralInRange(Int_t startBin, Int_t endBin);

    Double_t GetIntegralWithThreshold() { return GetThresholdIntegral(); }

    Double_t GetThresholdIntegral() {
        if (fThresholdIntegral == -1)
            std::cout << "TRestRawSignal::GetThresholdIntegral. InitializePointsOverThreshold should be "
                         "called first!"
                      << std::endl;
        return fThresholdIntegral;
    }

    Double_t GetTripleMaxIntegral();

    Double_t GetSlopeIntegral();

    Double_t GetRiseSlope();

    Int_t GetRiseTime();

    Double_t GetAverageInRange(Int_t startBin, Int_t endBin);

    Int_t GetMaxPeakWidth();
    Double_t GetMaxPeakValue();
    Int_t GetMaxPeakBin();

    Double_t GetMinPeakValue();
    Int_t GetMinPeakBin();

    void GetDifferentialSignal(TRestRawSignal* diffSgnl, Int_t smearPoints);
    void GetSignalSmoothed(TRestRawSignal* smthSignal, Int_t averagingPoints);
    void GetWhiteNoiseSignal(TRestRawSignal* noiseSgnl, Double_t noiseLevel = 1.);

    void CalculateBaseLine(Int_t startBin, Int_t endBin);
    Double_t GetStandardDeviation(Int_t startBin, Int_t endBin);
    void SubstractBaseline();
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
