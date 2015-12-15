///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignal.cxx
///
///             Event class to store signals from simulation and acquisition events 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia/Javier Galan
///		nov 2015:
///		    Changed vectors fSignalTime and fSignalCharge from <Int_t> to <Float_t>
///	            JuanAn Garcia
///_______________________________________________________________________________



#include "TRestSignal.h"

ClassImp(TRestSignal)
//______________________________________________________________________________
    TRestSignal::TRestSignal()
{
   // TRestSignal default constructor
}

//______________________________________________________________________________
TRestSignal::~TRestSignal()
{
   // TRestSignal destructor
}

void TRestSignal::AddPoint(TVector2 p)
{
    Int_t index =  GetTimeIndex( p.X() );
    Float_t x = p.X();
    Float_t y = p.Y();
    
    if( index >= 0 ){
        fSignalTime[index] =x;
        fSignalCharge[index] +=y;
        }
    else{
        fSignalTime.push_back(x);
        fSignalCharge.push_back(y);
        }
}

void TRestSignal::AddPoint( Double_t t, Double_t d ) { TVector2 p( t,d); AddPoint( p ); }
void TRestSignal::AddCharge( Double_t t, Double_t d ) { TVector2 p( t,d); AddPoint( p ); }
void TRestSignal::AddDeposit( Double_t t, Double_t d ) { TVector2 p( t,d); AddPoint( p ); }

Double_t TRestSignal::GetIntegral( ) 
{
    Double_t sum = 0;
    for( int i = 0; i < GetNumberOfPoints(); i++ )
    {
        sum += GetData(i);
    }

    return sum;
}

Double_t TRestSignal::GetIntegral( Int_t ni, Int_t nf ) 
{
    if( nf > GetNumberOfPoints()) nf = GetNumberOfPoints();
    Double_t sum = 0;
    for( int i = ni; i < nf; i++ )
    {
        sum += GetData(i);
    }

    return sum;
}

Int_t TRestSignal::GetMaxIndex(){

double max=1E-9;
int index;

for( int i = 0; i < GetNumberOfPoints(); i++ )
    {
    
        if(GetData(i)>max){
        max=GetData(i);
        index=i;
        }
    
    }

return index;

}

Int_t TRestSignal::GetTimeIndex( Double_t t )
{
    Float_t time = t;
    
    for( int n = 0; n < GetNumberOfPoints(); n++ )
        if( time == fSignalTime[n] ) return n;
    return -1;
}

Bool_t TRestSignal::isSorted( )
{
    for( int i = 0; i < GetNumberOfPoints()-1; i++ )
    {
        if( GetTime( i+1 ) < GetTime( i ) ) return false;
    }
    return true;
}

void TRestSignal::Sort()
{
    while( !isSorted() )
    {
        for( int i = 0; i < GetNumberOfPoints(); i++ )
        {
            for( int j = i; j < GetNumberOfPoints(); j++ )
            {
                if( GetTime( i ) > GetTime( j ) ){
                    iter_swap(fSignalTime.begin() + i, fSignalTime.begin() + j);
                    iter_swap(fSignalCharge.begin() + i, fSignalCharge.begin() + j);
                }
            }
        }
    }

}

void TRestSignal::GetDifferentialSignal( TRestSignal *diffSgnl, Int_t smearPoints )
{

    this->Sort();

    for( int i = 0; i < smearPoints; i++ )
         diffSgnl->AddPoint( GetTime(i), 0);

    for ( int i = smearPoints; i < this->GetNumberOfPoints() - smearPoints; i++ )
    {
         Double_t value = (this->GetData(i+smearPoints) - GetData( i-smearPoints)) / (GetTime(i+smearPoints) - GetTime(i-smearPoints));
         Double_t time = (GetTime(i+smearPoints) + GetTime(i-smearPoints))/2.;

         diffSgnl->AddPoint( time, value );
    }

    for( int i = GetNumberOfPoints()-smearPoints; i < GetNumberOfPoints(); i++ )
         diffSgnl->AddPoint( GetTime(i), 0);
}

void TRestSignal::GetSignalSmoothed( TRestSignal *smthSignal, Int_t averagingPoints )
{

    this->Sort();

    averagingPoints = (averagingPoints / 2) * 2 + 1; // make it odd >= averagingPoints

    Double_t sum = GetIntegral( 0, averagingPoints );
    for( int i = 0; i <= averagingPoints/2; i++ )
        smthSignal->AddPoint( GetTime(i), sum/averagingPoints);

    for ( int i = averagingPoints/2+1; i < GetNumberOfPoints()-averagingPoints/2; i++ )
    {
        sum -= this->GetData( i-(averagingPoints/2+1) );
        sum += this->GetData( i+averagingPoints/2 );
        smthSignal->AddPoint( this->GetTime(i), sum/averagingPoints );
    }

    for ( int i = GetNumberOfPoints()-averagingPoints/2; i < GetNumberOfPoints(); i++ )
         smthSignal->AddPoint( GetTime(i), sum/averagingPoints);

}

void TRestSignal::MultiplySignalBy( Double_t factor )
{
    for( int i = 0; i < GetNumberOfPoints(); i++ )
        fSignalCharge[i] = factor * fSignalCharge[i];
}

void TRestSignal::SignalAddition( TRestSignal *inSgnl )
{
    if( this->GetNumberOfPoints() != inSgnl->GetNumberOfPoints() )
    {
        cout << "ERROR : I cannot add two signals with different number of points" << endl;
        return;
    }

    Int_t badSignalTimes = 0;

    for( int i = 0; i < GetNumberOfPoints(); i++ )
        if( GetTime(i) != inSgnl->GetTime(i) ) { cout << "Time : " << GetTime(i) << " != " << inSgnl->GetTime(i) << endl; badSignalTimes++; }

    if( badSignalTimes )
    {
        cout << "ERROR : The times of signal addition must be the same" << endl;
        return;
    }

    for( int i = 0; i < GetNumberOfPoints(); i++ )
        fSignalCharge[i] += inSgnl->GetData(i);

}

void TRestSignal::Print( )
{
    for( int i = 0; i < GetNumberOfPoints(); i++ )
        cout << "Time : " << GetTime(i) << " Charge : " << GetData(i) << endl;
}
