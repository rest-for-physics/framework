//______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia/Javier Galan
///_______________________________________________________________________________


#include "TRestSignalEvent.h"
using namespace std;

ClassImp(TRestSignalEvent)
    //______________________________________________________________________________
TRestSignalEvent::TRestSignalEvent()
{
    // TRestSignalEvent default constructor
    Initialize();
}

//______________________________________________________________________________
TRestSignalEvent::~TRestSignalEvent()
{
    // TRestSignalEvent destructor
}

void TRestSignalEvent::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestSignalEvent";
    fSignal.clear();
    fPad = NULL;
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;
}

void TRestSignalEvent::AddSignal(TRestSignal s) 
{ 
    if( signalIDExists( s.GetSignalID() ) )
    {
        cout << "Warning. Signal ID : " << s.GetSignalID() << " already exists. Signal will not be added to signal event" << endl;
        return;
    }

    fSignal.push_back(s);
}

Int_t TRestSignalEvent::GetSignalIndex( Int_t signalID ) 
{ 
    for( int i = 0; i < GetNumberOfSignals(); i++ ) 
        if (fSignal[i].GetSignalID() == signalID ) 
            return i; 
    return -1; 
} 

Double_t TRestSignalEvent::GetIntegral( ) 
{
    Double_t sum = 0;
    for( int i = 0; i < GetNumberOfSignals(); i++ )
    {
        sum += fSignal[i].GetIntegral();
    }
    return sum;
}

void TRestSignalEvent::AddChargeToSignal( Int_t sgnlID, Double_t tm, Double_t chrg )
{
    Int_t sgnlIndex = GetSignalIndex( sgnlID );
    if( sgnlIndex == -1 ) 
    { 
        sgnlIndex = GetNumberOfSignals();

        TRestSignal sgnl;
        sgnl.SetSignalID( sgnlID );
        AddSignal( sgnl );
    }

    fSignal[sgnlIndex].AddDeposit( tm, chrg );
}

void TRestSignalEvent::PrintEvent()
{
    //TRestEvent::PrintEvent();

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

void TRestSignalEvent::SetMaxAndMin()
{
    for( int s = 0; s < GetNumberOfSignals(); s++ )
    {
        if( fMinTime > fSignal[s].GetMinTime() - 1 ) fMinTime = fSignal[s].GetMinTime() - 1;
        if( fMaxTime < fSignal[s].GetMaxTime() - 1 ) fMaxTime = fSignal[s].GetMaxTime() - 1;

        if( fMinValue > fSignal[s].GetMinValue() - 1 ) fMinValue = fSignal[s].GetMinValue() - 1;
        if( fMaxValue < fSignal[s].GetMaxValue() - 1 ) fMaxValue = fSignal[s].GetMaxValue() - 1;
    }
}

Double_t TRestSignalEvent::GetMaxValue( )
{
    SetMaxAndMin();
    return fMaxValue;
}

Double_t TRestSignalEvent::GetMinValue( )
{
    SetMaxAndMin();
    return fMinValue;
}

Double_t TRestSignalEvent::GetMinTime( )
{
    SetMaxAndMin();
    return fMinTime;
}

Double_t TRestSignalEvent::GetMaxTime( )
{
    SetMaxAndMin();
    return fMaxTime;
}

//Draw current event in a Tpad
TPad *TRestSignalEvent::DrawEvent()
{

    if(fPad != NULL) { delete fPad; fPad=NULL; }

    int nSignals = this->GetNumberOfSignals();

    if( nSignals == 0 )
    {
        cout << "Empty event " << endl;
        return NULL;
    }

    fPad = new TPad( this->GetClassName().Data(), " ", 0, 0, 1, 1 );
    fPad->Draw();
    fPad->cd();
    fPad->DrawFrame( GetMinTime() , GetMinValue() , GetMaxTime(), GetMaxValue() );

    for( int n = 0; n < nSignals; n++ )
    {
        TGraph *gr = fSignal[n].GetGraph( n + 1 );
        fPad->cd();
        gr->Draw( "LP" );
    }

    return fPad;
}



