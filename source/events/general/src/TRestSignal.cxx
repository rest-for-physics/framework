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
    if( index >= 0 )
        fSignalData[index].Set( p.X(), fSignalData[index].Y() + p.Y() );
    else
        fSignalData.push_back(p);
}

void TRestSignal::AddPoint( Double_t t, Double_t d ) { TVector2 p( t,d); AddPoint( p ); }
void TRestSignal::AddCharge( Double_t t, Double_t d ) { TVector2 p( t,d); AddPoint( p ); }
void TRestSignal::AddDeposit( Double_t t, Double_t d ) { TVector2 p( t,d); AddPoint( p ); }

Double_t TRestSignal::GetIntegral( ) 
{
    Double_t sum = 0;
    for( int i = 0; i < GetNumberOfPoints(); i++ )
    {
        sum += fSignalData[i].Y();
    }

    return sum;
}

Int_t TRestSignal::GetTimeIndex( Double_t t )
{
    for( int n = 0; n < GetNumberOfPoints(); n++ )
        if( t == fSignalData[n].X() ) return n;
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
                if( GetTime( i ) > GetTime( j ) )
                    iter_swap(fSignalData.begin() + i, fSignalData.begin() + j);
            }
        }
    }

}

void TRestSignal::Print( )
{
    for( int i = 0; i < GetNumberOfPoints(); i++ )
        cout << "Time : " << fSignalData[i].X() << " Charge : " << fSignalData[i].Y() << endl;
}
