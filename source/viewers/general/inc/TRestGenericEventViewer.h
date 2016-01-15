///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGenericEventViewer.h inherited from TRestEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestSignalEvent
///                 JuanAn Garcia
///_______________________________________________________________________________



#ifndef RestCore_TRestGenericEventViewer
#define RestCore_TRestGenericEventViewer

#include <TPad.h>
#include <TCanvas.h>

#include "TRestEventViewer.h"

class TRestGenericEventViewer:public TRestEventViewer {

 protected:
   
    TPad *fPad;
    TCanvas *fCanvas;
    
            
 public:
 
   virtual void Initialize();
         
   virtual void AddEvent( TRestEvent *ev );
         
   //Constructor
   TRestGenericEventViewer();
   //Destructor
   ~TRestGenericEventViewer();

   ClassDef(TRestGenericEventViewer, 1);      //class inherited from TRestEventViewer
};
#endif
