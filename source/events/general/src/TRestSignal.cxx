///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignal.h
///
///             Event class to store signals form simulation and acquisition events 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
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

void TRestSignal::Print( )
{
    for( int i = 0; i < GetNumberOfPoints(); i++ )
        cout << "Time : " << GetTime(i) << " Charge : " << GetData(i) << endl;
}
