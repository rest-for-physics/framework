///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignal.h
///
///             Event class to store signals fromm simulation and acquisition events 
///
///             feb 2017:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestRawSignal
#define RestCore_TRestRawSignal

#include <iostream>

#include <TObject.h>
#include <TString.h>
#include <TGraph.h>

class TRestRawSignal: public TObject {

    private:

    protected:
        
        Int_t fSignalID;
        
        std::vector <Short_t> fSignalData;   //Vector with the data of the signal
        
    public:

#ifndef __CINT__

        TGraph *fGraph;

        std::vector <Int_t> fPointsOverThreshold;

	Double_t fThresholdIntegral;

	Int_t fTailPoints;
#endif

        //Getters
        Short_t GetData( Int_t n ) { return GetSignalData( n ); }
        Int_t GetSignalID( ) { return fSignalID; }
        Int_t GetID( ) { return fSignalID; }

        Int_t GetNumberOfPoints() { return fSignalData.size(); }

        std::vector <Int_t> GetPointsOverThreshold( ) { return fPointsOverThreshold; }

        Double_t GetMaxValue() { return GetMaxPeakValue(); }
        Double_t GetMinValue() { return GetMinPeakValue(); }

        //Setters
        void SetSignalID( Int_t sID ) { fSignalID = sID; }
        void SetID( Int_t sID ) { fSignalID = sID; }

        void Reset(); 

        void Initialize();

        void AddPoint( Short_t d );
        void AddCharge( Short_t d );
        void AddDeposit( Short_t d );

        Short_t GetSignalData( Int_t n );

        void IncreaseBinBy( Int_t bin, Double_t data );

        Double_t GetIntegral( Int_t startBin = 0, Int_t endBin = 0 );

	void SetTailPoints( Int_t p ) { fTailPoints = p; }

        Double_t GetIntegralWithThreshold( Int_t from, Int_t to, 
                Int_t startBaseline, Int_t endBaseline, 
                Double_t nSigmas, Int_t nPointsOverThreshold, Double_t nMinSigmas );

        Double_t GetIntegralWithThreshold( Int_t from, Int_t to, 
                Double_t baseline, Double_t pointThreshold,
                Int_t nPointsOverThreshold, Double_t signalThreshold );

	Double_t GetThresholdIntegralValue() { return fThresholdIntegral; }

	Double_t GetSlopeIntegral( );
	Double_t GetRiseSlope( );
	Int_t GetRiseTime( );
	Double_t GetTripleMaxIntegral( );

        Double_t GetAverage( Int_t startBin, Int_t endBin );

        Int_t GetMaxPeakWidth();
        Double_t GetMaxPeakValue();
        Int_t GetMaxPeakBin( );

        Double_t GetMinPeakValue();
        Int_t GetMinPeakBin( );

        void GetDifferentialSignal( TRestRawSignal *diffSgnl, Int_t smearPoints );
        void GetSignalSmoothed( TRestRawSignal *smthSignal, Int_t averagingPoints );

        Double_t GetBaseLine( Int_t startBin, Int_t endBin );
        Double_t GetStandardDeviation( Int_t startBin, Int_t endBin );
        Double_t GetBaseLineSigma( Int_t startBin, Int_t endBin, Double_t baseline = 0 );
        Double_t SubstractBaseline( Int_t startBin, Int_t endBin );
        void AddOffset( Short_t offset );
        void SignalAddition( TRestRawSignal *inSgnl );

	void Scale( Double_t value );

        void WriteSignalToTextFile ( TString filename );
        void Print( );

        TGraph *GetGraph( Int_t color = 1 );
                
        //Construtor
        TRestRawSignal();
        TRestRawSignal( Int_t nBins );
        //Destructor
        ~TRestRawSignal();
        
        ClassDef(TRestRawSignal, 1);



};
#endif



