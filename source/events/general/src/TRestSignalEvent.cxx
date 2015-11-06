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
    fSignal.clear();
    fGr=NULL;
    fPad=NULL;
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

//Draw current event in a Tpad
TPad *TRestSignalEvent::DrawEvent(){

if(fGr!=NULL)delete[] fGr;
if(fPad!=NULL)delete fPad;

int nSignals = this->GetNumberOfSignals();

fGr = new TGraph[nSignals];

int c;

double maxX=-1E10,minX=1E10,maxY=-1E10,minY=1E10;
double x,y;

cout<<"N Signals "<<nSignals<<endl;

if(nSignals==0)return NULL;

	for(int i=0;i<nSignals;i++){
	c=0;
	cout<<"Signal "<<i<<" points "<<fSignal[i].GetNumberOfPoints()<<endl;
		for(int j=0;j<fSignal[i].GetNumberOfPoints();j++){
		x=fSignal[i].GetTime(j);
		y=fSignal[i].GetData(j);
		fGr[i].SetPoint(c,x,y);
		if(x>maxX)maxX=x;
		if(x<minX)minX=x;
		if(y>maxY)maxY=y;
		if(y<minY)minY=y;
		
		//cout<<v->X()<<"  "<<v->Y()<<endl;
		c++;
		
		}
	
	}

fPad = new TPad(this->GetClassName().Data()," ",0,0,1,1);
fPad->Draw();
fPad->cd();
fPad->DrawFrame(minX,minY,maxX,maxY);
for(int i=0;i<nSignals;i++){
fPad->cd();
fGr[i].Draw("LP");
}

return fPad;

}



