///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFFT.cxx
///
///             Event class to help for using TFFT 
///
///             dec 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestFFT
#define RestCore_TRestFFT

#include <iostream>

#include <TObject.h>
#include <TArrayD.h>

#include <TRestSignal.h>


class TRestFFT: public TObject {

    protected:
        
        Int_t fNfft;

        TArrayD fTimeReal; // [fNfft]
        TArrayD fTimeImg; // [fNfft]
        TArrayD fFrequencyReal; // [fNfft]
        TArrayD fFrequencyImg; // [fNfft]
        
    public:
	
        //Getters
        Double_t GetFrequencyAmplitudeReal( Int_t n ) { return fFrequencyReal.GetArray()[n]; }
        Double_t GetFrequencyAmplitudeImg( Int_t n ) { return fFrequencyImg.GetArray()[n]; }

        Int_t GetNfft( ) { return fNfft; }
        
        void GetSignal( TRestSignal *sgnl );

        //Setters
        void SetNfft( Int_t n );

        void SetNode( Int_t n, Double_t real, Double_t img=0) 
            { fFrequencyReal.GetArray()[n] = real;  fFrequencyImg.GetArray()[n] = img; }

        void SetSecondOrderAnalyticalResponse( Double_t f1, Double_t f2, Double_t to );
	void GaussianSecondOrderResponse( Double_t f1, Double_t f2, Double_t Ao, Double_t sigma );

        // FFT processing
        void ForwardSignalFFT( TRestSignal *sgnl, Int_t fNStart = 0, Int_t fNEnd = 0 );
        void BackwardFFT( );

        void RenormalizeNode( Int_t n, Double_t factor );
        void ApplyLowPassFilter( Int_t cutFrequency);
       // void NoiseReductionFilter( Int_t cutOff );
        void ButterWorthFilter( Int_t cutOff, Int_t order );
        void KillFrequencies( Int_t from, Int_t to );

        void RemoveBaseline( );

	void ProduceDelta( Int_t t_o, Int_t Nfft );

        void DivideBy( TRestFFT *fftInput, Int_t from = 0, Int_t to = 0 );
        void MultiplyBy( TRestFFT *fftInput, Int_t from = 0, Int_t to = 0 );

        void WriteFrequencyToTextFile ( TString filename );
        void WriteTimeSignalToTextFile ( TString filename );
                
        //Construtor
        TRestFFT();
        //Destructor
        ~TRestFFT();


        
        ClassDef(TRestFFT, 1);

};
#endif



