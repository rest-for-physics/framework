///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalViewerProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalViewerProcess
#define RestCore_TRestSignalViewerProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestSignalViewerProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestSignalEvent *fSignalEvent;//!
        // TODO We must get here a pointer to TRestDaqMetadata
        // In order to convert the parameters to time using the sampling time

        //TCanvas *fCanvas;
        vector <TObject *> fDrawingObjects;//!
        Double_t fDrawRefresh;//!

        TVector2 fBaseLineRange;//!

		int eveCounter = 0;//!
		int sgnCounter = 0;//!
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        
        //add here the members of your event process

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

	    cout << "Refresh value : " << fDrawRefresh << endl;

            EndPrintProcess();
        }

	TPad *DrawSignal( Int_t signal );

        TString GetProcessName() { return (TString) "rawSignalViewer"; }

        //Constructor
        TRestSignalViewerProcess();
        TRestSignalViewerProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalViewerProcess();

        ClassDef(TRestSignalViewerProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

