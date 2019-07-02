///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsEventViewer.h inherited from TRestEveEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestHitEvent
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestHitsEventViewer
#define RestCore_TRestHitsEventViewer

#include "TRestEveEventViewer.h"

#include "TRestHitsEvent.h"

class TRestHitsEventViewer : public TRestEveEventViewer {
   private:
    TRestHitsEvent* fHitsEvent;

   public:
    void Initialize();
    void DeleteCurrentEvent();
    void AddEvent(TRestEvent* ev);

    // Constructor
    TRestHitsEventViewer();
    // Destructor
    ~TRestHitsEventViewer();

    ClassDef(TRestHitsEventViewer, 1);  // class inherited from TRestEventViewer
};
#endif
