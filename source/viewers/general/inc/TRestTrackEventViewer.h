///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEventViewer.h inherited from TRestEveEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestTrackEvent
///                 Javier Gracia
///_______________________________________________________________________________



#ifndef RestCore_TRestTrackEventViewer
#define RestCore_TRestTrackEventViewer

#include <TPad.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include "TRestTrackEvent.h"
#include "TRestEventViewer.h"

class TRestTrackEventViewer:public TRestEventViewer {

 protected:

    TPad *fPad;
    TCanvas *fCanvas;

    TRestTrackEvent *fTrackEvent;
               
 public:
 
   virtual void Initialize();

   virtual void AddEvent( TRestEvent *ev );
     
   //Constructor
   TRestTrackEventViewer();
   //Destructor
   ~TRestTrackEventViewer();

   ClassDef(TRestTrackEventViewer, 1);      //class inherited from TRestEventViewer
};
#endif
