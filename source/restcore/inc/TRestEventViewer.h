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
    TRestEvent* fEvent = 0;         //!
    TRestBrowser* fController = 0;  //!

    TPad* fPad = 0;
    TCanvas* fCanvas = 0;

    TGTextButton* fButNext = 0;   //! Next signal.
    TGTextButton* fButPrev = 0;   //! Previous signal.
    TGLabel* fLabel = 0;          //! label(content"plot options:")
    TGTextEntry* fOptwindow = 0;  //! option input window.
    TGTextButton* fButOpt = 0;    //! draw with option.

   public:
    virtual void Initialize();

    virtual void AddEvent(TRestEvent* ev);

    void NextOption();

    void PreviousOption();

    void OptionPlot();

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
