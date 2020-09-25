///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4EventViewer.h inherited from TRestEveEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestGeant4Event
///                 Javier Galan/JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestGeant4EventViewer
#define RestCore_TRestGeant4EventViewer

#include "TRestEveEventViewer.h"

#include "TRestGeant4Event.h"

class TRestGeant4EventViewer : public TRestEveEventViewer {
   private:
    std::vector<TEveLine*> fHitConnectors;

    TRestGeant4Event* fG4Event;

   public:
    void Initialize();
    void DeleteCurrentEvent();
    void AddEvent(TRestEvent* ev);

    void NextTrackVertex(Int_t trkID, TVector3 to);
    void AddTrack(Int_t trkID, Int_t parentID, TVector3 from, TString name);
    void AddParentTrack(Int_t trkID, TVector3 from, TString name);

    void AddText(TString text, TVector3 at);
    void AddMarker(Int_t trkID, TVector3 at, TString name);

    // Constructor
    TRestGeant4EventViewer();
    // Destructor
    ~TRestGeant4EventViewer();

    ClassDef(TRestGeant4EventViewer, 1);  // class inherited from TRestEventViewer
};
#endif
