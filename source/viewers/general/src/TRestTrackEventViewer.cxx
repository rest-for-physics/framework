///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestTrackEvent
///                 Javier Gracia
///_______________________________________________________________________________



#include "TRestTrackEventViewer.h"

ClassImp(TRestTrackEventViewer)
//______________________________________________________________________________
TRestTrackEventViewer::TRestTrackEventViewer()
{
  Initialize();
}


//______________________________________________________________________________
TRestTrackEventViewer::~TRestTrackEventViewer()
{
   // TRestTrackEventViewer destructor
}

//______________________________________________________________________________
void TRestTrackEventViewer::Initialize()
{

   fPad = NULL;
   fCanvas = new TCanvas("view_track","view_track");

   fTrackEvent = new TRestTrackEvent( );
   fEvent = fTrackEvent;     
}



void TRestTrackEventViewer::AddEvent( TRestEvent *ev )
{

fTrackEvent=(TRestTrackEvent *)ev;

fPad = fTrackEvent->DrawEvent();

if(fPad==NULL)return;

fCanvas->cd();
fPad->Draw();
fPad->Update();
fCanvas->Update();


}

