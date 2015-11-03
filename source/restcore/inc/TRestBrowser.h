///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4EventViewer.h
///
///             aug 2015:   First concept. Javier Galan
//  
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
#include "TCanvas.h"

#include "TRestRun.h"
#include "TRestEvent.h"

#include <iostream>
using namespace std;

class TRestBrowser {
    private:

        TGMainFrame* frmMain;

        Int_t currentEvent;

        //Frames and buttons
        TGVerticalFrame *fVFrame;   	///< Vertical frame.
        TGNumberEntry   *fNEvent;  	///< Event number.
        TGTextButton	*fLoadEvent;	///< Load Event button
        TGTextButton	*fExit;		///< Load Event button
        TGPictureButton	*fButNext;  	///< Next number.
        TGPictureButton	*fButPrev;  	///< Previous event.
        TGPictureButton	*fMenuOpen;	///<Open file
        
        TCanvas *canvas;
        TPad *pad;
        
        TRestRun *run ;
    	TRestEvent *ev;
    	TTree *tr;
	
	Bool_t isFile;
	
    public:
        //Constructors
        TRestBrowser();

        //Destructor
        ~TRestBrowser();

        void Initialize();                
	
	void SetInputEvent(TRestEvent *rE){ev=rE;}
	
        void setWindowName(TString wName ){frmMain->SetWindowName(wName.Data());}

        void setButtons( );    

        void LoadEventAction();
        void LoadNextEventAction();
        void LoadPrevEventAction();
        void LoadFileAction( );
        void ExitAction( );

        Bool_t LoadEvent( Int_t n );
        Bool_t OpenFile( TString fName );



        ClassDef(TRestBrowser, 1);     // REST event superclass
};
#endif
