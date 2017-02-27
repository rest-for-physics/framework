///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignal.cxx
///
///             Event class to store signals from simulation and acquisition events 
///
///             feb 2017:   Created from TRestSignal
///              Javier Galan
///_______________________________________________________________________________



#include "TRestRawSignal.h"
using namespace std;

#include <TMath.h>
#include <TF1.h>
#include <TRandom3.h>

ClassImp(TRestRawSignal)
//______________________________________________________________________________
    TRestRawSignal::TRestRawSignal()
{
   // TRestRawSignal default constructor
   fGraph = NULL;
   fSignalID = -1;
   fSignalData.clear();

   fPointsOverThreshold.clear();
}

//______________________________________________________________________________
TRestRawSignal::~TRestRawSignal()
{
   // TRestRawSignal destructor
}

void TRestRawSignal::Initialize()
{
    fSignalData.clear();
    fPointsOverThreshold.clear();
    fSignalID = -1;
}

void TRestRawSignal::AddPoint( Short_t d )
{
    fSignalData.push_back(d);
}

void TRestRawSignal::AddCharge( Short_t d ) { AddPoint( d ); }
void TRestRawSignal::AddDeposit( Short_t d ) { AddPoint( d ); }

Short_t TRestRawSignal::GetSignalData( Int_t n )
{
    if( n >= GetNumberOfPoints() ) 
    {
        std::cout << "TRestRawSignal::GetSignalData: outside limits" << std::endl; 
        return 0xFFFF; 
    }

    return fSignalData[n];
}

void TRestRawSignal::IncreaseBinBy( Int_t bin, Double_t data )
{
    if( bin >= GetNumberOfPoints() ) 
    { 
        std::cout << "TRestRawSignal::IncreaseBinBy: outside limits" << std::endl; 
        return; 
    }

    fSignalData[bin] += data;
}

Double_t TRestRawSignal::GetIntegral( Int_t startBin, Int_t endBin ) 
{
    if( startBin < 0 ) startBin = 0;
    if( endBin <= 0 || endBin > GetNumberOfPoints() ) endBin = GetNumberOfPoints();

    Double_t sum = 0;
    for( int i = startBin; i < endBin; i++ )
        sum += GetData(i);

    return sum;
}


Double_t TRestRawSignal::GetIntegralWithThreshold( Int_t from, Int_t to, 
        Int_t startBaseline, Int_t endBaseline, 
        Double_t nSigmas, Int_t nPointsOverThreshold, Double_t nMinSigmas ) 
{
    if( startBaseline < 0 ) startBaseline = 0;
    if( endBaseline <= 0 || endBaseline > GetNumberOfPoints()) endBaseline = GetNumberOfPoints();

    Double_t baseLine = GetBaseLine( startBaseline, endBaseline );

    Double_t pointThreshold = nSigmas * GetBaseLineSigma( startBaseline, endBaseline );
    Double_t signalThreshold = nMinSigmas * GetBaseLineSigma( startBaseline, endBaseline );

    return GetIntegralWithThreshold( from, to, baseLine, pointThreshold, nPointsOverThreshold, signalThreshold);
}

Double_t TRestRawSignal::GetIntegralWithThreshold( Int_t from, Int_t to, 
        Double_t baseline, Double_t pointThreshold,
        Int_t nPointsOverThreshold, Double_t signalThreshold ) 
{
    Double_t sum = 0;
    Int_t nPoints = 0;
    fPointsOverThreshold.clear();

    if( to > GetNumberOfPoints() ) to = GetNumberOfPoints();

    Float_t maxValue = 0;
    for( int i = from; i < to; i++ )
    {
        if( GetData(i) > baseline + pointThreshold )
        {
            if( GetData( i ) > maxValue ) maxValue = GetData( i );
            nPoints++;
        }
        else
        {
            if( nPoints >= nPointsOverThreshold )
            {
                Double_t sig = GetStandardDeviation( i - nPoints, i );

                // Only if the sigma of points found over threshold 
                // are found above the signal threshold defined 
                // we will add them to the integral
                if( sig > signalThreshold )
                {
                    for( int j = i - nPoints; j < i; j++ )
                    {
                        sum += this->GetData( j );
                        fPointsOverThreshold.push_back( j );
                    }
                }
            }
            nPoints = 0;
            maxValue = 0;
        }
    }

    if( nPoints >= nPointsOverThreshold )
    {
        Double_t sig = GetStandardDeviation( to - nPoints, to );
        if( sig > signalThreshold )
        {
            for( int j = to - nPoints; j < to; j++ )
            {
                sum += this->GetData( j );
                fPointsOverThreshold.push_back( j );
            }
        }
    }

    return sum;
}

Double_t TRestRawSignal::GetAverage( Int_t startBin, Int_t endBin )
{
    if( startBin < 0 ) startBin = 0;
    if( endBin <= 0 || endBin > GetNumberOfPoints() ) endBin = GetNumberOfPoints();

    Double_t sum = 0;
    for( int i = startBin; i <= endBin; i++ )
        sum += this->GetData(i);

    return sum/(endBin-startBin+1);
}

Int_t TRestRawSignal::GetMaxPeakWidth()
{
    Int_t mIndex = this->GetMaxPeakBin();
    Double_t maxValue = this->GetData(mIndex);

    Double_t value = maxValue;
    Int_t rightIndex = mIndex;
    while( value > maxValue/2 )
    {
        value = this->GetData(rightIndex);
        rightIndex++;
    }
    Int_t leftIndex = mIndex;
    value = maxValue;
    while( value > maxValue/2 )
    {
        value = this->GetData(leftIndex);
        leftIndex--;
    }

    return rightIndex-leftIndex;
}

Double_t TRestRawSignal::GetMaxPeakValue() 
{
    return GetData( GetMaxPeakBin() ); 
}

Int_t TRestRawSignal::GetMaxPeakBin( )
{
    Double_t max = -1E10;
    Int_t index = 0;

    for( int i = 0; i < GetNumberOfPoints(); i++ )
    {
        if( this->GetData(i) > max) 
        {
            max = GetData(i);
            index = i;
        }
    }

    return index;
}

void TRestRawSignal::GetDifferentialSignal( TRestRawSignal *diffSgnl, Int_t smearPoints )
{
    diffSgnl->Initialize();

    for( int i = 0; i < smearPoints; i++ )
         diffSgnl->AddPoint( 0 );

    for ( int i = smearPoints; i < this->GetNumberOfPoints() - smearPoints; i++ )
    {
         Double_t value = 0.5 * (this->GetData(i+smearPoints) - GetData( i-smearPoints)) / smearPoints;

         diffSgnl->AddPoint( (Short_t) value );
    }

    for( int i = GetNumberOfPoints()-smearPoints; i < GetNumberOfPoints(); i++ )
         diffSgnl->AddPoint( 0 );
}

void TRestRawSignal::GetSignalSmoothed( TRestRawSignal *smthSignal, Int_t averagingPoints )
{
    smthSignal->Initialize();

    averagingPoints = (averagingPoints / 2) * 2 + 1; // make it odd >= averagingPoints

    Double_t sumAvg = GetIntegral( 0, averagingPoints )/ averagingPoints;
    for( int i = 0; i <= averagingPoints/2; i++ )
        smthSignal->AddPoint( (Short_t) sumAvg );

    for ( int i = averagingPoints/2+1; i < GetNumberOfPoints()-averagingPoints/2; i++ )
    {
        sumAvg -= this->GetData( i-(averagingPoints/2+1) )/ averagingPoints;
        sumAvg += this->GetData( i+averagingPoints/2 ) / averagingPoints;
        smthSignal->AddPoint( (Short_t) sumAvg );
    }

    for ( int i = GetNumberOfPoints()-averagingPoints/2; i < GetNumberOfPoints(); i++ )
         smthSignal->AddPoint( sumAvg );
}

Double_t TRestRawSignal::GetBaseLine( Int_t startBin, Int_t endBin )
{
    if( endBin-startBin <= 0 ) return 0.;

    Double_t baseLine = 0;
    for( int i = startBin; i < endBin; i++ )
        baseLine += fSignalData[i];

    return baseLine/(endBin-startBin);
}

Double_t TRestRawSignal::GetStandardDeviation( Int_t startBin, Int_t endBin )
{
    Double_t bL = GetBaseLine( startBin, endBin );
    return GetBaseLineSigma( startBin, endBin, bL );
}

Double_t TRestRawSignal::GetBaseLineSigma( Int_t startBin, Int_t endBin, Double_t baseline )
{
    Double_t bL = baseline;
    if( bL == 0 ) bL = GetBaseLine( startBin, endBin );

    Double_t baseLineSigma = 0;
    for( int i = startBin; i < endBin; i++ )
        baseLineSigma += (bL-fSignalData[i]) * (bL-fSignalData[i]);

    return TMath::Sqrt(baseLineSigma/(endBin-startBin));
}

Double_t TRestRawSignal::SubstractBaseline( Int_t startBin, Int_t endBin )
{
    Double_t bL = GetBaseLine( startBin, endBin );

    AddOffset( (Short_t) -bL );

    return bL;
}

void TRestRawSignal::AddOffset( Short_t offset )
{
    for( int i = 0; i < GetNumberOfPoints(); i++ )
        fSignalData[i] = fSignalData[i] + offset;
}

void TRestRawSignal::SignalAddition( TRestRawSignal *inSgnl )
{
    if( this->GetNumberOfPoints() != inSgnl->GetNumberOfPoints() )
    {
        cout << "ERROR : TRestRawSignal::SignalAddition." <<endl;
        cout << "I cannot add two signals with different number of points" << endl;
        return;
    }

    for( int i = 0; i < GetNumberOfPoints(); i++ )
        fSignalData[i] += inSgnl->GetData(i);

}


void TRestRawSignal::WriteSignalToTextFile ( TString filename )
{

    FILE *fff = fopen( filename.Data(), "w" );
    for ( int i = 0; i < GetNumberOfPoints(); i++ )
        fprintf( fff, "%d\t%d\n", i, GetData(i) );
    fclose(fff);
}

void TRestRawSignal::Print( )
{
    for( int i = 0; i < GetNumberOfPoints(); i++ )
        cout << "Bin : " << i << " Charge : " << GetData(i) << endl;
}

TGraph *TRestRawSignal::GetGraph( Int_t color )
{
    if( fGraph != NULL ) { delete fGraph; fGraph = NULL; }

    fGraph = new TGraph();

    fGraph->SetLineWidth( 2 );
    fGraph->SetLineColor( color );
    fGraph->SetMarkerStyle( 7 );

    int points = 0;
    for( int n = 0; n < GetNumberOfPoints(); n++ )
    {
        fGraph->SetPoint( points, n, GetData(n) );
        points++;
    }

    return fGraph;
}
