///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestBrowser.h : public TRestRun
///
///             G4 class description
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#ifndef RestCore_TRestBrowser
#define RestCore_TRestBrowser

#include "TObject.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TGFileDialog.h"
#include "TApplication.h"
#include "TGLabel.h"
#include "TGDimension.h"
#include "TBrowser.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TCanvas.h"

#include "TRestRun.h"
#include "TRestEventViewer.h"

#include <iostream>

class TRestBrowser : public TRestRun {

protected:

#ifndef __CINT__
	TGMainFrame* frmMain = 0;//!

	//Frames and buttons

	TGVerticalFrame *fVFrame = 0;   	//! < Vertical frame.
	TGNumberEntry   *fNEvent = 0;  	//! Event number.
	TGLabel			*fLabel = 0;		//! label(content"plot options:")
	TGPictureButton	*fButPrev = 0;  	//! Previous event.
	TGPictureButton	*fLoadEvent = 0;	//! Load Event button
	TGPictureButton	*fButNext = 0;  	//! Next number.
	TGPictureButton	*fMenuOpen = 0;	//! Open file
	TGTextButton	*fExit = 0;		//! Load Event button

	TCanvas*fCanDefault = 0;//!

	TBrowser*b = 0;//!
#endif     


private:
#ifndef __CINT__
	Bool_t isFile;

	TRestEventViewer *fEventViewer = 0;//!
#endif

public:
	//Constructors
	TRestBrowser();

	//Destructor
	~TRestBrowser();

	void Initialize();
	void InitFromConfigFile();

	void SetViewer(TRestEventViewer *eV);

	void setWindowName(TString wName) { frmMain->SetWindowName(wName.Data()); }

	void setButtons();

	TGVerticalFrame *generateNewFrame();

	void addFrame(TGFrame*f);

	void LoadEventAction();
	void LoadNextEventAction();
	void LoadPrevEventAction();
	void LoadFileAction();
	void ExitAction();

	Bool_t OpenFile(TString fName);
	Bool_t LoadEvent(Int_t n);

	ClassDef(TRestBrowser, 1);     //
};
#endif
