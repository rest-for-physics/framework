///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestSignalEvent
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestSignalEventViewer.h"

ClassImp(TRestSignalEventViewer)
//______________________________________________________________________________
TRestSignalEventViewer::TRestSignalEventViewer()
{
  Initialize();
}


//______________________________________________________________________________
TRestSignalEventViewer::~TRestSignalEventViewer()
{
   
}

//______________________________________________________________________________
void TRestSignalEventViewer::Initialize()
{
       
fPad = NULL;

fCanvas = new TCanvas("Signal","Signal");

fSignalEvent = new TRestSignalEvent( );
fEvent = fSignalEvent;

}

void TRestSignalEventViewer::AddEvent( TRestEvent *ev ){


fSignalEvent=(TRestSignalEvent *)ev;

fPad = fSignalEvent->DrawEvent();

if(fPad==NULL)return;

fCanvas->cd();
fPad->Draw();
fPad->Update();
fCanvas->Update();





}





