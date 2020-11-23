#ifndef RestCore_TRestBrowser
#define RestCore_TRestBrowser

#include <iostream>

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
#include "TRestEventViewer.h"
#include "TRestRun.h"
#include "TSystem.h"

/// Event browser for different input file
class TRestBrowser {
   protected:
#ifndef __CINT__
    TGMainFrame* frmMain = 0;  //!

    // Frames and buttons

    TGVerticalFrame* fVFrame = 0;             //! < The main vertical frame for browser controlling
    TGLabel* fEventRowLabel = 0;              //!
    TGLabel* fEventIdLabel = 0;               //!
    TGLabel* fEventSubIdLabel = 0;            //!
    TGNumberEntry* fEventRowNumberBox = 0;    //! For row number.
    TGNumberEntry* fEventIdNumberBox = 0;     //! For Event number.
    TGNumberEntry* fEventSubIdNumberBox = 0;  //! For sub Event number.

    TGLabel* fPlotOptionLabel = 0;        //!
    TGTextEntry* fPlotOptionTextBox = 0;  //! TextBox for plot options
    TGTextButton* fButOptPrev = 0;        //! Previous plot option
    TGTextButton* fButOptRefresh = 0;     //! Refresh plot
    TGTextButton* fButOptNext = 0;        //! Next plot option

    TGPictureButton* fMenuOpen = 0;  //! Open file button
    TGTextButton* fExit = 0;         //! Exit button

    TCanvas* fCanDefault = 0;  //!
    Int_t fEventRow = 0;       //!
    Int_t fEventId = 0;        //!
    Int_t fEventSubId = 0;     //!

    TBrowser* b = 0;  //!
    TRestRun* r = 0;  //!
#endif

   private:
#ifndef __CINT__
    Bool_t pureAnalysis;
    TString fInputFileName;

    TRestEventViewer* fEventViewer = 0;  //!

    void SetButtons();
    Bool_t LoadEventId(Int_t id, Int_t subid = -1);
    Bool_t LoadEventEntry(Int_t n);
#endif

   public:
    // tool method
    void Initialize(TString opt = "FI");
    void InitFromConfigFile();
    Bool_t OpenFile(TString filename);

    // setters
    void SetViewer(TRestEventViewer* eV);
    void SetViewer(TString viewerName);
    void SetInputEvent(TRestEvent*);
    void SetWindowName(TString wName) { frmMain->SetWindowName(wName.Data()); }

    // getters
    TRestEventViewer* GetViewer() { return fEventViewer; }

    // actions
    void LoadFileAction();
    void ExitAction();

    void RowValueChangedAction(Long_t val);
    void IdValueChangedAction(Long_t val);

    void NextPlotOptionAction();
    void PreviousPlotOptionAction();
    void PlotAction();

    // Constructors
    TRestBrowser();
    TRestBrowser(TString viewerName);

    // Destructor
    ~TRestBrowser();
};
#endif
