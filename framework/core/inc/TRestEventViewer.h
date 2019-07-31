///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventViewer.h
///
///             A geometry class to store detector geometry
///
///             jul 2015:   First concept
///                 J. Galan
///		nov 2015: Generic class for event visualization
///		    JuanAn Garcia
///
///_______________________________________________________________________________

#ifndef RestCore_TRestEventViewer
#define RestCore_TRestEventViewer

#include <iostream>

#include <TGLViewer.h>
#include <TGeoManager.h>
#include <TObject.h>

#include "TRestEvent.h"
class TRestBrowser;
class TRestEventViewer {
   protected:
    TGeoManager* fGeometry = 0;     //!
    TRestEvent* fEvent = 0;         //!
    TRestBrowser* fController = 0;  //!

   public:
    virtual void Initialize();

    virtual void AddEvent(TRestEvent* ev) = 0;

    virtual void DeleteCurrentEvent();
    // Setters
    virtual void SetGeometry(TGeoManager* geo) { fGeometry = geo; }
    void SetEvent(TRestEvent* ev) { fEvent = ev; }
    void SetController(TRestBrowser* b) { fController = b; }
    // Getters
    TGeoManager* GetGeometry() { return fGeometry; }
    TRestEvent* GetEvent() { return fEvent; }

    // Construtor
    TRestEventViewer();
    // Destructor
    virtual ~TRestEventViewer();

    ClassDef(TRestEventViewer, 1);  // REST event superclass
};
#endif
