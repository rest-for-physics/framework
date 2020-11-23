///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventViewer.cxx
///
///             A geometry class to store detector geometry
///
///             jul 2015:   First concept
///                 J. Galan
///		nov 2015: Generic class for event visualization
///		    JuanAn Garcia
///
///_______________________________________________________________________________

#include "TRestEventViewer.h"
#include "TRestBrowser.h"

ClassImp(TRestEventViewer)
    //______________________________________________________________________________
    TRestEventViewer::TRestEventViewer() {
    // TRestEventViewer default constructor
    //Initialize();
    fPad = NULL;
    fCanvas = NULL;
}

//______________________________________________________________________________
TRestEventViewer::~TRestEventViewer() {
    // TRestEventViewer destructor
    // DeleteCurrentEvent(  );
}

void TRestEventViewer::Initialize() {
    fPad = NULL;
    
    if (fCanvas != NULL) delete fCanvas;
    fCanvas = new TCanvas("Event Viewer", "Event Viewer");

    fCanvas->SetWindowPosition(350, 10);
}

void TRestEventViewer::Embed(TBrowser* b) {
    if (b != NULL) b->StartEmbedding(1, -1);

    Initialize();

    if (b != NULL) b->StopEmbedding();
}

void TRestEventViewer::AddEvent(TRestEvent* ev) { fEvent = ev; }

void TRestEventViewer::Plot(const char* option) {
    if (fPad == NULL) fPad = new TPad();
    if (fEvent != NULL) {
        fPad = fEvent->DrawEvent(option);
        fCanvas->cd();
        fPad->Draw();
        fPad->Update();
        fCanvas->Update();
    } else {
        fCanvas->cd();
        fPad->Clear();
        fPad->Update();
        fCanvas->Update();
    }
}

void TRestEventViewer::DeleteCurrentEvent() { delete fEvent; }
