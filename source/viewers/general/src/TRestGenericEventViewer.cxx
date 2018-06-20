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
using namespace std;

ClassImp(TRestGenericEventViewer)
//______________________________________________________________________________
TRestGenericEventViewer::TRestGenericEventViewer( Int_t fW, Int_t fH )
{
  Initialize( fW, fH );

  fOption = "";
}


//______________________________________________________________________________
TRestGenericEventViewer::~TRestGenericEventViewer()
{
   
}

//______________________________________________________________________________
void TRestGenericEventViewer::Initialize( Int_t fW, Int_t fH )
{

    fPad = NULL;

    fCanvas = new TCanvas("Event Viewer","Event Viewer", fW, fH );

}

void TRestGenericEventViewer::AddEvent( TRestEvent *ev ){


fPad = ev->DrawEvent( fOption );

if(fPad==NULL)return;

fCanvas->cd();
fPad->Draw();
fPad->Update();
fCanvas->Update();





}





