///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEventViewer.h inherited from TRestEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestSignalEvent
///                 JuanAn Garcia
///_______________________________________________________________________________



#ifndef RestCore_TRestSignalEventViewer
#define RestCore_TRestSignalEventViewer

#include <TPad.h>
#include <TCanvas.h>
#include <TRestSignalEvent.h>

#include "TRestEventViewer.h"

class TRestSignalEventViewer:public TRestEventViewer {

 protected:
   
    TPad *fPad;
    TCanvas *fCanvas;
    
    TRestSignalEvent *fSignalEvent;
    
 public:
 
   void Initialize();
         
   void AddEvent( TRestEvent *ev );
   
   //Constructor
   TRestSignalEventViewer();
   //Destructor
   ~TRestSignalEventViewer();

   ClassDef(TRestSignalEventViewer, 1);      //class inherited from TRestEventViewer
};
#endif
