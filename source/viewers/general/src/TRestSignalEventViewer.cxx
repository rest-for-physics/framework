///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEventViewer.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestSignalEventViewer
///             How to use: replace TRestSignalEventViewer by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
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





