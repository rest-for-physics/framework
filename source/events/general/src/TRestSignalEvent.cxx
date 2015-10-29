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
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestSignalEvent.h"
#include "TGraph.h"
#include "TH1.h"

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
}

void TRestSignalEvent::PrintEvent()
{
    //TRestEvent::PrintEvent();


}

//Draw current event in a Tpad
TPad *TRestSignalEvent::DrawEvent(){

int nSignals = this->GetNumberOfSignals();

TGraph *gr = new TGraph[nSignals];

TVector2 *v;

int c;

double maxX=-1E10,minX=1E10,maxY=-1E10,minY=1E10;

//cout<<"N Signals "<<nSignals<<endl;

	for(int i=0;i<nSignals;i++){
	c=0;
	//cout<<"Signal "<<i<<" points"<<fSignal[i].GetNumberOfPoints()<<endl;
		for(int j=0;j<fSignal[i].GetNumberOfPoints();j++){
		v=fSignal[i].GetPoint(j);
		gr[i].SetPoint(c,v->X(),v->Y());
		if(v->X()>maxX)maxX=v->X();
		if(v->X()<minX)minX=v->X();
		if(v->Y()>maxY)maxY=v->Y();
		if(v->Y()<minY)minY=v->Y();
		
		//cout<<v->X()<<"  "<<v->Y()<<endl;
		c++;
		
		}
	
	}

TPad *pad = new TPad(this->GetClassName().Data()," ",0,0,1,1);
pad->Draw();
pad->cd();
pad->DrawFrame(minX,minY,maxX,maxY);
for(int i=0;i<nSignals;i++){
pad->cd();
gr[i].Draw("LP");
}

return pad;

}



