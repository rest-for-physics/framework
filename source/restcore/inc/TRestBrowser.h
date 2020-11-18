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
    TGNumberEntry* fNEvent = 0;       //! Event number.
    TGLabel* fLabel = 0;              //! label(content"plot options:")
    TGPictureButton* fButPrev = 0;    //! Previous event.
    TGPictureButton* fLoadEvent = 0;  //! Load Event button
    TGPictureButton* fButNext = 0;    //! Next number.
    TGPictureButton* fMenuOpen = 0;   //! Open file
    TGTextButton* fExit = 0;          //! Load Event button

    TCanvas* fCanDefault = 0;  //!

    TBrowser* b = 0;  //!
    TRestRun* r = 0;  //!
#endif

   private:
#ifndef __CINT__
    Bool_t pureAnalysis;
    TString fInputFileName;
    Int_t fCurrentEntry = 0;
    Int_t fCurrentId = 0;

    TRestEventViewer* fEventViewer = 0;  //!
#endif

   public:
    // Constructors
    TRestBrowser();
    TRestBrowser(TString viewerName);

    // Destructor
    ~TRestBrowser();

    void Initialize(TString opt = "FI");
    void InitFromConfigFile();

    void SetViewer(TRestEventViewer* eV);
    void SetViewer(TString viewerName);
    void SetInputEvent(TRestEvent*);

    void setWindowName(TString wName) { frmMain->SetWindowName(wName.Data()); }

    void setButtons();

    TRestEventViewer* GetViewer() { return fEventViewer; }

    TGVerticalFrame* generateNewFrame();

    void addFrame(TGFrame* f);

    void LoadEventAction();
    void LoadNextEventAction();
    void LoadPrevEventAction();
    void LoadFileAction();
    void ExitAction();

    Bool_t OpenFile(TString fName);
    Bool_t LoadEventId(Int_t id);
    Bool_t LoadEventEntry(Int_t n);
};
#endif
