///______________________________________________________________________________
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
    if( sgnlIndex == -1 ) { cout << "Warning. Signal ID does not exist. I cannot add charge to signal" << endl; return; }

    fSignal[sgnlIndex].AddDeposit( tm, chrg );
}

void TRestSignalEvent::PrintEvent()
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
