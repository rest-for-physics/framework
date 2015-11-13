///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEventViewer.h
///
///             nov 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
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
