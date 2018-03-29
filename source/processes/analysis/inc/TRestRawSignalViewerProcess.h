///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalViewerProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestRawSignalViewerProcess
#define RestCore_TRestRawSignalViewerProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestRawSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestRawSignalViewerProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestRawSignalEvent *fSignalEvent;
        // TODO We must get here a pointer to TRestDaqMetadata
        // In order to convert the parameters to time using the sampling time

        //TCanvas *fCanvas;
        vector <TObject *> fDrawingObjects;
        Double_t fDrawRefresh;

        TVector2 fBaseLineRange;
        TVector2 fThresholdRange;
        TVector2 fYRange;

        Double_t fPeakThreshold;
        Int_t fNSignalsThreshold;
#endif

        TPad *DrawSignals( );

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
            cout << "Baseline range : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) " << endl;
            cout << "Drawing Y-range : ( " << fYRange.X() << " , " << fYRange.Y() << " ) " << endl;
            cout << "Threshold range: ( " << fThresholdRange.X() << " , " << fThresholdRange.Y() << " ) " << endl;
            cout << "Max peak amplitude threshold : " << fPeakThreshold << endl;
            cout << "Max number of signals : " << fNSignalsThreshold << endl;
            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "rawSignalViewer"; }

        //Constructor
        TRestRawSignalViewerProcess();
        TRestRawSignalViewerProcess( char *cfgFileName );
        //Destructor
        ~TRestRawSignalViewerProcess();

        ClassDef(TRestRawSignalViewerProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

