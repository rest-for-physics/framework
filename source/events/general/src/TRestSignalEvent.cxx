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
///
///	       feb 2016: Added titles to axis in DrawGraph using TMultiGraph
///		  Javier Gracia
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
    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    for( int s = 0; s < GetNumberOfSignals(); s++ )
    {
        if( fMinTime > fSignal[s].GetMinTime() ) fMinTime = fSignal[s].GetMinTime();
        if( fMaxTime < fSignal[s].GetMaxTime() ) fMaxTime = fSignal[s].GetMaxTime();

        if( fMinValue > fSignal[s].GetMinValue() ) fMinValue = fSignal[s].GetMinValue();
        if( fMaxValue < fSignal[s].GetMaxValue() ) fMaxValue = fSignal[s].GetMaxValue();
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


    fMinValue = 1E10;
    fMaxValue = -1E10;
    fMinTime = 1E10;
    fMaxTime = -1E10;

    fPad = new TPad( this->GetName(), " ", 0, 0, 1, 1 );
    fPad->Draw();
    fPad->cd();
    cout << "GetMinTime : " << GetMinTime() << endl;
    cout << "GetMaxTime : " << GetMaxTime() << endl;
    cout << "GetMinValue : " << GetMinValue() << endl;
    cout << "GetMaxValue : " << GetMaxValue() << endl;
    fPad->DrawFrame( GetMinTime()-1, GetMinValue()-1 , GetMaxTime()+1, GetMaxValue()+1);

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());


    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle(title);
    mg->GetXaxis()->SetTitle("time bins");
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->SetTitle("charge (electrons)");


    for( int n = 0; n < nSignals; n++ )
    {
        TGraph *gr = fSignal[n].GetGraph( n + 1 );

        mg->Add(gr);
    }

     fPad->cd();
     mg->Draw("");

    return fPad;
}


