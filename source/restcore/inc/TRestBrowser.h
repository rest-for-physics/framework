#ifndef RestCore_TRestBrowser
#define RestCore_TRestBrowser

#include "TApplication.h"
#include "TBrowser.h"
#include "TCanvas.h"
#include "TGButton.h"
#include "TGDimension.h"
#include "TGFileDialog.h"
#include "TGFrame.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TObject.h"
#include "TROOT.h"
#include "TSystem.h"

#include "TRestEventViewer.h"
#include "TRestRun.h"

#include <iostream>

/// Event browser for different input file
class TRestBrowser {
   protected:
#ifndef __CINT__
    TGMainFrame* frmMain = 0;  //!

    // Frames and buttons

    TGVerticalFrame* fVFrame = 0;     //! < Vertical frame.
    TGLabel* fEventRowLabel = 0;           //! label(content"plot options:")
    TGLabel* fEventIdLabel = 0;                //! label(content"plot options:")
    TGLabel* fEventSubIdLabel = 0;        //! label(content"plot options:")
    TGNumberEntry* fEventRowNumberBox = 0;      //! Event number.
    TGNumberEntry* fEventIdNumberBox = 0;       //! Event number.
    TGNumberEntry* fEventSubIdNumberBox = 0;  //! Event number.
    TGPictureButton* fButEventPrev = 0;    //! Previous event.
    TGPictureButton* fButEventLoad = 0;  //! Load Event button
    TGPictureButton* fButEventNext = 0;    //! Next number.
    TGPictureButton* fMenuOpen = 0;   //! Open file
    TGTextButton* fExit = 0;          //! Load Event button

    TGLabel* fPlotOptionLabel = 0;  //! label(content"plot options:")
    TGTextEntry* fPlotOptionTextBox = 0; 
    TGTextButton* fButOptPrev = 0;     //! Previous event.
    TGTextButton* fButOptLoad = 0;     //! Load Event button
    TGTextButton* fButOptNext = 0;     //! Next number.

    TCanvas* fCanDefault = 0;  //!

    TBrowser* b = 0;  //!
    TRestRun* r = 0;  //!
#endif

   private:
#ifndef __CINT__
    Bool_t pureAnalysis;
    TString fInputFileName;

    TRestEventViewer* fEventViewer = 0;  //!

    Bool_t LoadEventId(Int_t id, Int_t subid = -1);
    Bool_t LoadEventEntry(Int_t n);
#endif

   public:
    // Constructors
    TRestBrowser();
    TRestBrowser(TString viewerName);

    // Destructor
    ~TRestBrowser();

    void Initialize(TString opt = "FI");
    void InitFromConfigFile();
    Bool_t OpenFile(TString filename);

    void SetViewer(TRestEventViewer* eV);
    void SetViewer(TString viewerName);
    void SetInputEvent(TRestEvent*);

    void setWindowName(TString wName) { frmMain->SetWindowName(wName.Data()); }

    void setButtons();

    TRestEventViewer* GetViewer() { return fEventViewer; }

    // actions
    void LoadFileAction();
    void ExitAction();

    void RowValueChangedAction(Long_t val);
    void IdValueChangedAction(Long_t val);

    void NextPlotOptionAction();
    void PreviousPlotOptionAction();
    void PlotAction();


};
#endif
