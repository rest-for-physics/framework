///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorReadoutAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorReadoutAnalysisProcess
#define RestCore_TRestDetectorReadoutAnalysisProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestDetectorReadout.h>
#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorReadoutAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestRawSignalEvent* fSignalEvent;  //!

    TRestDetectorReadout* fReadout;  //!

#endif

    void InitFromConfigFile();

    void Initialize();

    string fModuleCanvasSave;  //!

    // plots (saved directly in root file)
    map<int, TH2D*> fModuleHitMaps;    //! [MM id, channel activity]
    map<int, TH1D*> fModuleActivityX;  //! [MM id, channel activity]
    map<int, TH1D*> fModuleActivityY;  //! [MM id, channel activity]
    map<int, TH2D*> fModuleBSLSigmaX;  //! [MM id, channel activity]
    map<int, TH2D*> fModuleBSLSigmaY;  //! [MM id, channel activity]
                                       //

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "channel activity and hitmap histograms required for module: ";
        auto iter2 = fModuleHitMaps.begin();
        while (iter2 != fModuleHitMaps.end()) {
            metadata << iter2->first << ", ";
            iter2++;
        }
        metadata << endl;

        metadata << "path for output plots: " << fModuleCanvasSave << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "readoutAnalysis"; }

    // Constructor
    TRestDetectorReadoutAnalysisProcess();
    TRestDetectorReadoutAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorReadoutAnalysisProcess();

    ClassDef(TRestDetectorReadoutAnalysisProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
