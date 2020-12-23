///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalViewerProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorSignalViewerProcess
#define RestCore_TRestDetectorSignalViewerProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorSignalViewerProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorSignalEvent* fSignalEvent;  //!
    // TODO We must get here a pointer to TRestDaqMetadata
    // In order to convert the parameters to time using the sampling time

    // TCanvas *fCanvas;
    vector<TObject*> fDrawingObjects;  //!
    Double_t fDrawRefresh;             //!

    TVector2 fBaseLineRange;  //!

    int eveCounter = 0;  //!
    int sgnCounter = 0;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        cout << "Refresh value : " << fDrawRefresh << endl;

        EndPrintProcess();
    }

    TPad* DrawSignal(Int_t signal);

    TString GetProcessName() { return (TString) "rawSignalViewer"; }

    // Constructor
    TRestDetectorSignalViewerProcess();
    TRestDetectorSignalViewerProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorSignalViewerProcess();

    ClassDef(TRestDetectorSignalViewerProcess, 1);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
