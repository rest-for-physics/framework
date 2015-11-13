///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsEventViewer.h
///
///             nov 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
///_______________________________________________________________________________


#ifndef RestCore_TRestHitsEventViewer
#define RestCore_TRestHitsEventViewer

#include "TRestSimEventViewer.h"

#include "TRestHitsEvent.h"

class TRestHitsEventViewer:public TRestSimEventViewer {

 private:
        
    TRestHitsEvent *fHitsEvent;
               
 public:
 
   void Initialize();
   void DeleteCurrentEvent(  );
   void AddEvent( TRestEvent *ev );
     
   //Constructor
   TRestHitsEventViewer();
   //Destructor
   ~TRestHitsEventViewer();

   ClassDef(TRestHitsEventViewer, 1);      //class inherited from TRestEventViewer
};
#endif
