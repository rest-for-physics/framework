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

   fThresholdIntegral = -1;

   fTailPoints = 0;
}

TRestRawSignal::TRestRawSignal( Int_t nBins )
{
   // TRestRawSignal default constructor
   fGraph = NULL;
   fSignalID = -1;
   fSignalData.clear();

   fPointsOverThreshold.clear();

   for( int n = 0; n < nBins; n++ )
       fSignalData.push_back( 0 );

   fThresholdIntegral = -1;

   fTailPoints = 0;
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

    fThresholdIntegral = -1;

    fTailPoints = 0;
}

void TRestRawSignal::Reset()
{
    Int_t nBins = GetNumberOfPoints();
    fSignalData.clear();
    for( int n = 0; n < nBins; n++ )
        fSignalData.push_back( 0 );
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

	//int debug = 0;
	//if ( GetMaxPeakValue( 150, 250 ) > 20 ) debug = 1;

	Float_t maxValue = 0;
	for( int i = from; i < to; i++ )
	{
		//if( debug )
		//	cout << "point : " << i << " data : " << GetData(i) << " baseline : " << baseline << " threshold : " << pointThreshold << endl;
		if( GetData(i) > baseline + pointThreshold )
		{
		//	if( debug )
		//		cout << "Point over threshold" << endl;
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
		//		if( debug )
		//			cout << "Signal : " << sig << " signal Threshold : " << signalThreshold << endl;
				if( sig > signalThreshold )
				{
					for( int j = i - nPoints - fTailPoints; j < i + fTailPoints && i + j < GetNumberOfPoints(); j++ )
					{
						if( j < 0 ) j = 0;
		//				if( debug )
		//					cout << "Adding point : " <<  j << " data : " << GetData( j ) << endl;
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

	fThresholdIntegral = sum;
	return sum;
}

Double_t TRestRawSignal::GetSlopeIntegral( )
{
	//cout << __PRETTY_FUNCTION__ << endl;
	if( fThresholdIntegral == -1 )
		cout << "REST Warning. TRestRawSignal::GetSlopeIntegral. GetIntegralWithThreshold should be called first." << endl;

	Double_t sum = 0;
	for( unsigned int n = 0; n < fPointsOverThreshold.size(); n++ )
	{
		if( n + 1 >= fPointsOverThreshold.size() ) return sum;

		sum += GetData( fPointsOverThreshold[n] );

		if( GetData( fPointsOverThreshold[n+1] ) - GetData( fPointsOverThreshold[n] ) < 0 )
			break;
			
	}
	return sum;
}

Double_t TRestRawSignal::GetRiseSlope( )
{
	//cout << __PRETTY_FUNCTION__ << endl;
	if( fThresholdIntegral == -1 )
		cout << "REST Warning. TRestRawSignal::GetRiseSlope. GetIntegralWithThreshold should be called first." << endl;

	if( fPointsOverThreshold.size() < 2 )
	{
		cout << "REST Warning. TRestRawSignal::GetRiseSlope. Less than 2 points!." << endl;
		return 0;
	}

	Int_t maxBin = GetMaxPeakBin()-1;

	Double_t hP = GetData( maxBin );

	Double_t lP = GetData( fPointsOverThreshold[0] );

	return (hP-lP)/(maxBin-fPointsOverThreshold[0]);
}

Int_t TRestRawSignal::GetRiseTime( )
{
	//cout << __PRETTY_FUNCTION__ << endl;
	if( fThresholdIntegral == -1 )
		cout << "REST Warning. TRestRawSignal::GetRiseTime. GetIntegralWithThreshold should be called first." << endl;

	if( fPointsOverThreshold.size() < 2 )
	{
		cout << "REST Warning. TRestRawSignal::GetRiseTime. Less than 2 points!." << endl;
		return 0;
	}

	return GetMaxPeakBin() - fPointsOverThreshold[0];
}

Double_t TRestRawSignal::GetTripleMaxIntegral( )
{
	//cout << __PRETTY_FUNCTION__ << endl;
	if( fThresholdIntegral == -1 )
		cout << "REST Warning. TRestRawSignal::GetRiseTime. GetIntegralWithThreshold should be called first." << endl;

	Int_t cBin = GetMaxPeakBin();

	if( cBin+1 >= GetNumberOfPoints() ) return 0;

	Double_t a1 = GetData( cBin );
	Double_t a2 = GetData( cBin-1 );
	Double_t a3 = GetData( cBin+1 );

	return a1 + a2 + a3;
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

Double_t TRestRawSignal::GetMaxPeakValue( Int_t start, Int_t end) 
{
    return GetData( GetMaxPeakBin( start, end ) ); 
}

Int_t TRestRawSignal::GetMaxPeakBin( Int_t start, Int_t end )
{
    Double_t max = -1E10;
    Int_t index = 0;

    if( end == 0 || end > GetNumberOfPoints() ) end = GetNumberOfPoints();
    if( start < 0 ) start = 0;

    for( int i = start; i < end; i++ )
    {
        if( this->GetData(i) > max) 
        {
            max = GetData(i);
            index = i;
        }
    }

    return index;
}

Double_t TRestRawSignal::GetMinPeakValue() 
{
    return GetData( GetMinPeakBin() ); 
}

Int_t TRestRawSignal::GetMinPeakBin( )
{
    Double_t min = 1E10;
    Int_t index = 0;

    for( int i = 0; i < GetNumberOfPoints(); i++ )
    {
        if( this->GetData(i) < min) 
        {
            min = GetData(i);
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

void TRestRawSignal::Scale( Double_t value )
{
	for( int i = 0; i < GetNumberOfPoints(); i++ )
	{
		Double_t scaledValue = value * fSignalData[i];
		fSignalData[i] = (Short_t) scaledValue;
	}
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
    fGraph->SetLineColor( color%8 + 1 );
    fGraph->SetMarkerStyle( 7 );

    int points = 0;
    for( int n = 0; n < GetNumberOfPoints(); n++ )
    {
        fGraph->SetPoint( points, n, GetData(n) );
        points++;
    }

    return fGraph;
}
