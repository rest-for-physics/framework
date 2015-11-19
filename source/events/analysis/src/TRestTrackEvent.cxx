///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#include "TRestTrackEvent.h"

ClassImp(TRestTrackEvent)
//______________________________________________________________________________
    TRestTrackEvent::TRestTrackEvent()
{
   // TRestTrackEvent default constructor
   Initialize();
}

//______________________________________________________________________________
TRestTrackEvent::~TRestTrackEvent()
{
   // TRestTrackEvent destructor
}

void TRestTrackEvent::Initialize()
{
    nTracks = 0;
    fTrack.clear();
    TRestEvent::Initialize();
    fEventClassName = "TRestTrackEvent";

}

void TRestTrackEvent::PrintEvent()
{
    TRestEvent::PrintEvent();


}


//Draw current event in a Tpad
TPad *TRestTrackEvent::DrawEvent(){


if(fGr!=NULL)delete[] fGr;
if(fPad!=NULL)delete fPad;

int nTracks = this->GetNumberOfTracks();

TRestTrack *tck = new TRestTrack();
TRestVolumeHits hits;

if(nTracks==0)return NULL;

fGr = new TGraph[nTracks];

int c;

double maxX=-1E10,minX=1E10,maxY=-1E10,minY=1E10;
double x,y;

	for(int i=0;i<nTracks;i++){

	     tck = this->GetTrack( i );
	     hits = tck->GetVolumeHits();
	     cout<<"Number of hits "<< hits.GetNumberOfHits( ) <<endl;

	     c=0;
	
   	     for(int nhit=0; nhit <  hits.GetNumberOfHits( ); nhit++ )
	     {
	

	   	  Double_t x= hits.GetX( nhit );
		  Double_t y= hits.GetY( nhit );
		  Double_t z= hits.GetZ( nhit );

		  fGr[i].SetPoint(c,x,y);

		  if(x>maxX)maxX=x;
		  if(x<minX)minX=x;
		  if(y>maxY)maxY=y;
		  if(y<minY)minY=y;
		

		  c++;
		
 	    }
	
	}


fPad = new TPad(this->GetClassName().Data()," ",0,0,1,1);
fPad->Draw();
fPad->cd();
fPad->DrawFrame(minX-10,minY+10,maxX+10,maxY+10);

for(int ntr=0;ntr<nTracks;ntr++)
	{
	    fGr[ntr].SetMarkerColor(1+ntr);
	    fGr[ntr].SetMarkerSize(1.);
	    fGr[ntr].SetMarkerStyle(21);

	    fPad->cd();
	    if(ntr==0)   
	        fGr[ntr].Draw("P");
             else if (ntr>0)
	        fGr[ntr].Draw("PSAME");
	}

return fPad;


}
