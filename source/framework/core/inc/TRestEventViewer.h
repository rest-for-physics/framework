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

#include <TGeoManager.h>
#include <TObject.h>

#include <iostream>

#include "TGButton.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TRestEvent.h"

class TRestBrowser;
class TRestEventViewer {
   protected:
    TGeoManager* fGeometry = 0;     //!
    TRestEvent* fEvent = 0;      //!

    TPad* fPad = 0;
    TCanvas* fCanvas = 0;

    virtual void Initialize();

   public:
    virtual void Embed(TBrowser* b);

    virtual void AddEvent(TRestEvent* ev);

    virtual void Plot(const char* option);

    virtual void DeleteCurrentEvent();
    // Setters
    virtual void SetGeometry(TGeoManager* geo) { fGeometry = geo; }
    void SetEvent(TRestEvent* ev) { fEvent = ev; }
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
