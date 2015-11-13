///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventViewer.cxx
///
///             Base geometry class used to visualize events
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestEventViewer.h"


ClassImp(TRestEventViewer)
//______________________________________________________________________________
    TRestEventViewer::TRestEventViewer()
{
   // TRestEventViewer default constructor
   Initialize();
}

//______________________________________________________________________________
TRestEventViewer::~TRestEventViewer()
{
    // TRestEventViewer destructor
    //DeleteCurrentEvent(  );
    
}

void TRestEventViewer::Initialize()
{
    fGeometry=NULL;
    fEvent=NULL;
    
    DeleteCurrentEvent(  );
    
       
}

void TRestEventViewer::DeleteCurrentEvent(  ){

delete fEvent;

}

