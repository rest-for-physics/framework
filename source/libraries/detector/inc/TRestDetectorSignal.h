///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignal.h
///
///             Event class to store signals fromm simulation and acquisition
///             events
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t> to
///< Float_t> 	            JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorSignal
#define RestCore_TRestDetectorSignal

#include <iostream>

#include <TGraph.h>
#include <TObject.h>
#include <TString.h>
#include <TVector2.h>

class TRestDetectorSignal : public TObject {
   private:
    Int_t GetMinIndex();
    Int_t GetTimeIndex(Double_t t);

   protected:
    Int_t fSignalID;

    std::vector<Float_t> fSignalTime;    // Vector with the time of the signal
    std::vector<Float_t> fSignalCharge;  // Vector with the charge of the signal

    void AddPoint(TVector2 p);
    void SetPoint(TVector2 p);

   public:
#ifndef __CINT__

    TGraph* fGraph;  //!

    std::vector<Int_t> fPointsOverThreshold;  //!
#endif

    void Initialize() {
        fSignalCharge.clear();
        fSignalTime.clear();
        fSignalID = -1;
    }

    // TODO other objects should probably skip using GetMaxIndex direclty
    Int_t GetMaxIndex(Int_t from = 0, Int_t to = 0);

    // Getters
    TVector2 GetPoint(Int_t n) {
        TVector2 vector2(GetTime(n), GetData(n));

        return vector2;
    }

    Int_t GetSignalID() { return fSignalID; }
    Int_t GetID() { return fSignalID; }

    void IncreaseTimeBinBy(Int_t bin, Double_t data) {
        if (bin >= GetNumberOfPoints()) {
            std::cout << "Increase time bin: outside limits" << std::endl;
            return;
        }

        fSignalCharge[bin] += data;
    }

    Int_t GetNumberOfPoints() {
        if (fSignalTime.size() != fSignalCharge.size()) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "WARNING, the two vector sizes did not match" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }
        return fSignalTime.size();
    }

    Double_t GetIntegralWithTime(Double_t startTime, Double_t endTime);
    Double_t GetIntegral(Int_t startBin = 0, Int_t endBin = 0);
    /*
Double_t GetIntegralWithThreshold(Int_t from, Int_t to, Int_t startBaseline, Int_t endBaseline,
                                  Double_t threshold = 2, Int_t nPointsOverThreshold = 5,
                                  Double_t nMinSigmas = 5);
Double_t GetIntegralWithThreshold(Int_t from, Int_t to, Double_t baseline, Double_t pointThreshold,
                                  Int_t nPointsOverThreshold, Double_t signalThreshold);
                                                                      */

    void Normalize(Double_t scale = 1.);

    std::vector<Int_t> GetPointsOverThreshold() { return fPointsOverThreshold; }

    Double_t GetAverage(Int_t start = 0, Int_t end = 0);
    Int_t GetMaxPeakWidth();
    Double_t GetMaxPeakWithTime(Double_t startTime, Double_t endTime);

    Double_t GetMaxPeakValue();
    Double_t GetMinPeakValue();

    Double_t GetMaxPeakTime(Int_t from = 0, Int_t to = 0);

    Double_t GetMaxValue() { return GetMaxPeakValue(); }
    Double_t GetMinValue() { return GetMinPeakValue(); }

    Double_t GetMinTime();
    Double_t GetMaxTime();

    Double_t GetData(Int_t index) { return (double)fSignalCharge[index]; }
    Double_t GetTime(Int_t index) { return (double)fSignalTime[index]; }

    // Setters
    void SetSignalID(Int_t sID) { fSignalID = sID; }
    void SetID(Int_t sID) { fSignalID = sID; }

    void NewPoint(Float_t time, Float_t data);
    void AddPoint(Double_t t, Double_t d);
    void AddCharge(Double_t t, Double_t d);
    void AddDeposit(Double_t t, Double_t d);

    void SetPoint(Double_t t, Double_t d);
    void SetPoint(Int_t index, Double_t t, Double_t d);

    Double_t GetStandardDeviation(Int_t startBin, Int_t endBin);
    Double_t GetBaseLine(Int_t startBin, Int_t endBin);
    Double_t GetBaseLineSigma(Int_t startBin, Int_t endBin, Double_t baseline = 0);

    Double_t SubstractBaseline(Int_t startBin, Int_t endBin);
    void AddOffset(Double_t offset);

    void MultiplySignalBy(Double_t factor);
    void ExponentialConvolution(Double_t fromTime, Double_t decayTime, Double_t offset = 0);
    void SignalAddition(TRestDetectorSignal* inSgnl);

    Bool_t isSorted();
    void Sort();

    void GetDifferentialSignal(TRestDetectorSignal* diffSgnl, Int_t smearPoints = 5);
    void GetSignalDelayed(TRestDetectorSignal* delayedSignal, Int_t delay);
    void GetSignalSmoothed(TRestDetectorSignal* smthSignal, Int_t averagingPoints = 3);

    void GetWhiteNoiseSignal(TRestDetectorSignal* noiseSgnl, Double_t noiseLevel = 1.);
    void GetSignalGaussianConvolution(TRestDetectorSignal* convSgnl, Double_t sigma = 100., Int_t nSigmas = 5);

    void AddGaussianSignal(Double_t amp, Double_t sigma, Double_t time, Int_t N, Double_t fromTime,
                           Double_t toTime);

    void Reset() {
        fSignalTime.clear();
        fSignalCharge.clear();
    }

    void WriteSignalToTextFile(TString filename);
    void Print();

    TGraph* GetGraph(Int_t color = 1);

    // Construtor
    TRestDetectorSignal();
    // Destructor
    ~TRestDetectorSignal();

    ClassDef(TRestDetectorSignal, 1);
};
#endif
