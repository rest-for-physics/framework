///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGenericEventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestSignalEvent
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestGenericEventViewer.h"

ClassImp(TRestGenericEventViewer)
//______________________________________________________________________________
TRestGenericEventViewer::TRestGenericEventViewer()
{
  Initialize();
}


//______________________________________________________________________________
TRestGenericEventViewer::~TRestGenericEventViewer()
{
   
}

//______________________________________________________________________________
void TRestGenericEventViewer::Initialize()
{
       
fPad = NULL;

fCanvas = new TCanvas("Event Viewer","Event Viewer");

//fEvent = new TRestSignalEvent();

}

void TRestGenericEventViewer::AddEvent( TRestEvent *ev ){


//fEvent=(TRestSignalEvent *)ev;

fPad = ev->DrawEvent();

if(fPad==NULL)return;

fCanvas->cd();
fPad->Draw();
fPad->Update();
fCanvas->Update();





}





