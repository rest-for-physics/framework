///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawFFT.cxx
///
///             Event class to help for using TFFT
///
///             dec 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestRawFFT
#define RestCore_TRestRawFFT

#include <iostream>

#include <TArrayD.h>
#include <TObject.h>

#include <TRestRawSignal.h>

class TRestRawFFT : public TObject {
   protected:
    Int_t fNfft;

    TArrayD fTimeReal;       // [fNfft]
    TArrayD fTimeImg;        // [fNfft]
    TArrayD fFrequencyReal;  // [fNfft]
    TArrayD fFrequencyImg;   // [fNfft]

   public:
    // Getters
    Double_t GetFrequencyAmplitudeReal(Int_t n) { return fFrequencyReal.GetArray()[n]; }
    Double_t GetFrequencyAmplitudeImg(Int_t n) { return fFrequencyImg.GetArray()[n]; }

    Double_t GetFrequencyNorm2(Int_t n);

    Int_t GetNfft() { return fNfft; }

    void GetSignal(TRestRawSignal* sgnl);

    // Setters
    void SetNfft(Int_t n);

    void SetNode(Int_t n, Double_t real, Double_t img = 0) {
        fFrequencyReal.GetArray()[n] = real;
        fFrequencyImg.GetArray()[n] = img;
    }

    void SetSecondOrderAnalyticalResponse(Double_t f1, Double_t f2, Double_t to);
    void GaussianSecondOrderResponse(Double_t f1, Double_t f2, Double_t Ao, Double_t sigma);

    // FFT processing
    void ForwardSignalFFT(TRestRawSignal* sgnl, Int_t fNStart = 0, Int_t fNEnd = 0);
    void BackwardFFT();

    void RenormalizeNode(Int_t n, Double_t factor);
    void ApplyLowPassFilter(Int_t cutFrequency);
    // void NoiseReductionFilter( Int_t cutOff );
    void ButterWorthFilter(Int_t cutOff, Int_t order);
    void KillFrequencies(Int_t cutOff);

    void RemoveBaseline();

    void ProduceDelta(Int_t t_o, Int_t Nfft);

    void DivideBy(TRestRawFFT* fftInput, Int_t from = 0, Int_t to = 0);
    void MultiplyBy(TRestRawFFT* fftInput, Int_t from = 0, Int_t to = 0);

    void ApplyResponse(TRestRawFFT* fftInput, Int_t cutOff);

    void WriteFrequencyToTextFile(TString filename);
    void WriteTimeSignalToTextFile(TString filename);

    // Construtor
    TRestRawFFT();
    // Destructor
    ~TRestRawFFT();

    ClassDef(TRestRawFFT, 1);
};
#endif
