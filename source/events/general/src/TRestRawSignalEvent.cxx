//______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             feb 2017:   First concept
///                 Created from TRestSignalEvent
///                 Javier Galan
///_______________________________________________________________________________

#include <TMath.h>

#include "TRestRawSignalEvent.h"
using namespace std;

ClassImp(TRestRawSignalEvent)
    //______________________________________________________________________________
TRestRawSignalEvent::TRestRawSignalEvent()
{
    // TRestRawSignalEvent default constructor
    Initialize();
}

//______________________________________________________________________________
TRestRawSignalEvent::~TRestRawSignalEvent()
{
    // TRestRawSignalEvent destructor
}

void TRestRawSignalEvent::Initialize()
{
    TRestEvent::Initialize();
    fSignal.clear();
    fPad = NULL;
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;
}

void TRestRawSignalEvent::AddSignal(TRestRawSignal s) 
{ 
    if( signalIDExists( s.GetSignalID() ) )
    {
        cout << "Warning. Signal ID : " << s.GetSignalID() << " already exists. Signal will not be added to signal event" << endl;
        return;
    }

    fSignal.push_back(s);
}

Int_t TRestRawSignalEvent::GetSignalIndex( Int_t signalID ) 
{ 
    for( int i = 0; i < GetNumberOfSignals(); i++ ) 
        if (fSignal[i].GetSignalID() == signalID ) 
            return i; 
    return -1; 
} 

Double_t TRestRawSignalEvent::GetIntegral( Int_t startBin, Int_t endBin ) 
{
    Double_t sum = 0;

    for( int i = 0; i < GetNumberOfSignals(); i++ )
        sum += fSignal[i].GetIntegral( startBin, endBin );

    return sum;
}

Double_t TRestRawSignalEvent::GetIntegralWithThreshold( Int_t from, Int_t to, 
        Int_t startBaseline, Int_t endBaseline, 
        Double_t nSigmas, Int_t nPointsOverThreshold, Double_t minPeakAmplitude ) 
{
    Double_t sum = 0;

    for( int i = 0; i < GetNumberOfSignals(); i++ )
        sum += fSignal[i].GetIntegralWithThreshold( from, to, startBaseline, endBaseline, nSigmas, nPointsOverThreshold, minPeakAmplitude );

    return sum;

}

TRestRawSignal* TRestRawSignalEvent::GetMaxSignal( Int_t startBin, Int_t endBin )
{
    if( GetNumberOfSignals() <= 0 ) return NULL;

    Double_t max = fSignal[0].GetIntegral( startBin, endBin );

    Int_t sId;
    for( int i = 0; i < GetNumberOfSignals(); i++ )
    {
        Int_t integ = fSignal[i].GetIntegral( startBin, endBin );
        if( max < integ ) 
        { 
            max = integ; 
            sId = i;
        }
    }

    return &fSignal[sId];
}

Double_t TRestRawSignalEvent::GetSlopeIntegral( )
{
    Double_t sum = 0;

    for( int i = 0; i < GetNumberOfSignals(); i++ )
        sum += fSignal[i].GetSlopeIntegral( );

    return sum;
}

Double_t TRestRawSignalEvent::GetRiseSlope( )
{
    Double_t sum = 0;

    Int_t n = 0;
    for( int i = 0; i < GetNumberOfSignals(); i++ )
    {
	if( fSignal[i].GetThresholdIntegralValue() > 0 )
	{
		sum += fSignal[i].GetSlopeIntegral( );
		n++;
	}
    }

    if( n == 0 ) return 0;

    return sum/n;
}

Double_t TRestRawSignalEvent::GetRiseTime( )
{
    Double_t sum = 0;

    Int_t n = 0;
    for( int i = 0; i < GetNumberOfSignals(); i++ )
    {
        if( fSignal[i].GetThresholdIntegralValue() > 0 )
        {
            sum += fSignal[i].GetRiseTime( );
            n++;
        }
    }

    if( n == 0 ) return 0;

    return sum/n;
}

Double_t TRestRawSignalEvent::GetTripleMaxIntegral( Int_t startBin, Int_t endBin )
{
    Double_t sum = 0;

    for( int i = 0; i < GetNumberOfSignals(); i++ )
        if( fSignal[i].GetThresholdIntegralValue() > 0 )
            sum += fSignal[i].GetTripleMaxIntegral( startBin, endBin );

    return sum;
}

Double_t TRestRawSignalEvent::GetBaseLineAverage( Int_t startBin, Int_t endBin )
{
    Double_t baseLineMean = 0;

    for( int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++ )
    {
        Double_t baseline = GetSignal(sgnl)->GetBaseLine( startBin, endBin );
        baseLineMean += baseline;
    }

    return baseLineMean/GetNumberOfSignals();
}

Int_t TRestRawSignalEvent::GetLowestWidth( Int_t startBin, Int_t endBin,  Double_t minPeakAmplitude )
{
    Int_t low = 10000000;

    for( int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++ )
    {
        if( GetSignal( sgnl )->GetMaxPeakValue( startBin, endBin ) > minPeakAmplitude )
        {
            Int_t lW = GetSignal( sgnl )->GetMaxPeakWidth( startBin, endBin );
            if( low > lW )
                low = lW;
        }
    }

    return low;
}

Double_t TRestRawSignalEvent::GetAverageWidth( Int_t startBin, Int_t endBin,  Double_t minPeakAmplitude )
{
    Double_t avg = 0;
    Int_t n = 0;
    for( int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++ )
    {
        if( GetSignal( sgnl )->GetMaxPeakValue( startBin, endBin ) > minPeakAmplitude )
        {
            avg += GetSignal( sgnl )->GetMaxPeakWidth( startBin, endBin );
            n++;
        }
    }

    if( n == 0 )
        return 0;
    else
        return avg/n;
}

Double_t TRestRawSignalEvent::GetLowAverageWidth( Int_t nSignals, Int_t startBin, Int_t endBin,  Double_t minPeakAmplitude )
{
    std::vector <Double_t> widths;

    for( int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++ )
    {
        if( GetSignal( sgnl )->GetMaxPeakValue( startBin, endBin ) > minPeakAmplitude )
            widths.push_back( GetSignal( sgnl )->GetMaxPeakWidth( startBin, endBin ) );
    }

    if( widths.size() == 0 )
        return 0;

    std::sort( widths.begin(), widths.end() );

    Int_t nMax = nSignals;
    if ( widths.size() < (unsigned int) nSignals )
        nMax = widths.size();

    Double_t avg = 0;
    for( int n = 0; n < nMax; n++ )
        avg += widths[n];

    return avg/nSignals;
}

Double_t TRestRawSignalEvent::GetBaseLineSigmaAverage( Int_t startBin, Int_t endBin )
{
    Double_t baseLineSigmaMean = 0;

    for( int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++ )
    {
        Double_t baselineSigma = GetSignal(sgnl)->GetBaseLineSigma( startBin, endBin );
        baseLineSigmaMean += baselineSigma;
    }   

    return baseLineSigmaMean / GetNumberOfSignals();
}

void TRestRawSignalEvent::SubstractBaselines( Int_t startBin, Int_t endBin )
{
    for( int sgnl = 0; sgnl < GetNumberOfSignals(); sgnl++ )
        GetSignal( sgnl )->SubstractBaseline( startBin, endBin );
}

void TRestRawSignalEvent::AddChargeToSignal( Int_t sgnlID, Int_t bin, Short_t value )
{
    Int_t sgnlIndex = GetSignalIndex( sgnlID );
    if( sgnlIndex == -1 ) 
    { 
        sgnlIndex = GetNumberOfSignals();

        TRestRawSignal sgnl( 512 ); // For the moment we use the default nBins=512
        sgnl.SetSignalID( sgnlID );
        AddSignal( sgnl );
    }

    fSignal[sgnlIndex].IncreaseBinBy( bin, value );
}

void TRestRawSignalEvent::PrintEvent()
{
    TRestEvent::PrintEvent();

    for( int i = 0; i < GetNumberOfSignals(); i++ )
    {
        cout << "================================================" << endl;
        cout << "Signal ID : " << fSignal[i].GetSignalID() << endl;
        cout << "Integral : " << fSignal[i].GetIntegral() << endl;
        cout << "------------------------------------------------" << endl;
        fSignal[i].Print();
        cout << "================================================" << endl;
    }
}

// TODO: GetMaxTimeFast, GetMinTimeFast, GetMaxValueFast that return the value of fMinTime, fMaxTime, etc
void TRestRawSignalEvent::SetMaxAndMin( Int_t startBin, Int_t endBin )
{
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 0;
    fMaxTime = -1E10;

    for( int s = 0; s < GetNumberOfSignals(); s++ )
    {

        if( fMinValue > fSignal[s].GetMinValue( startBin, endBin ) ) fMinValue = fSignal[s].GetMinValue( startBin, endBin );
        if( fMaxValue < fSignal[s].GetMaxValue( startBin, endBin ) ) fMaxValue = fSignal[s].GetMaxValue( startBin, endBin );
    }

    if( GetNumberOfSignals() > 0 )
        fMaxTime = fSignal[0].GetNumberOfPoints();
}

Double_t TRestRawSignalEvent::GetMaxValue( Int_t startBin, Int_t endBin )
{
    SetMaxAndMin( startBin, endBin );
    return fMaxValue;
}

Double_t TRestRawSignalEvent::GetMinValue( Int_t startBin, Int_t endBin )
{
    SetMaxAndMin( startBin, endBin );
    return fMinValue;
}

Double_t TRestRawSignalEvent::GetMinTime( )
{
    return 0;
}

Double_t TRestRawSignalEvent::GetMaxTime( )
{
    Double_t maxTime = 512;

    if( GetNumberOfSignals() > 0 )
        maxTime = fSignal[0].GetNumberOfPoints();

    return maxTime;
}

//Draw current event in a Tpad
TPad *TRestRawSignalEvent::DrawEvent( TString option )
{
    if(fPad != NULL) { delete fPad; fPad=NULL; }

    int nSignals = this->GetNumberOfSignals();

    if( nSignals == 0 )
    {
        cout << "Empty event " << endl;
        return NULL;
    }


    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    fPad = new TPad( this->GetName(), " ", 0, 0, 1, 1 );
    fPad->Draw();
    fPad->cd();
    fPad->DrawFrame( 0, GetMinValue()-1 , GetMaxTime()+1, GetMaxValue()+1);

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());


    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle(title);
    mg->GetXaxis()->SetTitle("time bins");
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->SetTitle("Energy");


    for( int n = 0; n < nSignals; n++ )
    {
        TGraph *gr = fSignal[n].GetGraph( n + 1 );

        mg->Add(gr);
    }

     fPad->cd();
     mg->Draw("");

    return fPad;
}


