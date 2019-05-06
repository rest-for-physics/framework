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
  Initialize();
}

//______________________________________________________________________________
TRestEventViewer::~TRestEventViewer() {
  // TRestEventViewer destructor
  // DeleteCurrentEvent(  );
}

void TRestEventViewer::Initialize() {
  fGeometry = NULL;
  fEvent = NULL;

  DeleteCurrentEvent();
}

void TRestEventViewer::DeleteCurrentEvent() { delete fEvent; }
