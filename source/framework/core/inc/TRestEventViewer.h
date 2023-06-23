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

#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGeoManager.h>
#include <TObject.h>

#include <iostream>

#include "TRestEvent.h"

class TRestBrowser;
class TRestEventViewer {
   protected:
    TGeoManager* fGeometry = nullptr;  //!
    TRestEvent* fEvent = nullptr;      //!

    Double_t fGeomScale = 0.1;

    TPad* fPad = nullptr;
    TCanvas* fCanvas = nullptr;

    virtual void Initialize();

   public:
    virtual void Embed(TBrowser* b);

    virtual void AddEvent(TRestEvent* ev);

    virtual void Plot(const char* option);

    virtual void DeleteCurrentEvent();
    // Setters
    virtual void SetGeometry(TGeoManager* geo) { fGeometry = geo; }
    void SetEvent(TRestEvent* event) { fEvent = event; }
    // Getters
    TGeoManager* GetGeometry() { return fGeometry; }
    TRestEvent* GetEvent() { return fEvent; }

    void SetGeomScale(Double_t scale) { fGeomScale = scale; }
    Double_t GetGeomScale() const { return fGeomScale; }

    // Constructor
    TRestEventViewer();
    // Destructor
    virtual ~TRestEventViewer();

    ClassDef(TRestEventViewer, 1);  // REST event superclass
};
#endif
