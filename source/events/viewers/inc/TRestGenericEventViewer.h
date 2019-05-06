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

#include <TCanvas.h>
#include <TPad.h>

#include "TRestEventViewer.h"

#include "TGButton.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
class TRestGenericEventViewer : public TRestEventViewer {
 protected:
  TPad* fPad = 0;
  TCanvas* fCanvas = 0;
  TRestEvent* fEvent = 0;

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

  // Constructor
  TRestGenericEventViewer();
  // Destructor
  ~TRestGenericEventViewer();

  ClassDef(TRestGenericEventViewer, 1);  // class inherited from
                                         // TRestEventViewer
};
#endif
