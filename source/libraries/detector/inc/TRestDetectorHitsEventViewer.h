///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsEventViewer.h inherited from TRestEveEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestHitEvent
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsEventViewer
#define RestCore_TRestDetectorHitsEventViewer

#include "TRestEveEventViewer.h"

#include "TRestDetectorHitsEvent.h"

class TRestDetectorHitsEventViewer : public TRestEveEventViewer {
   private:
    TRestDetectorHitsEvent* fHitsEvent;

   public:
    void Initialize();
    void DeleteCurrentEvent();
    void AddEvent(TRestEvent* ev);

    // Constructor
    TRestDetectorHitsEventViewer();
    // Destructor
    ~TRestDetectorHitsEventViewer();

    ClassDef(TRestDetectorHitsEventViewer, 1);  // class inherited from TRestEventViewer
};
#endif
