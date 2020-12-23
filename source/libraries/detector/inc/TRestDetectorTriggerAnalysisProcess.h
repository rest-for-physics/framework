///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorTriggerAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorTriggerAnalysisProcess
#define RestCore_TRestDetectorTriggerAnalysisProcess

#include <TH1D.h>

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorTriggerAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorSignalEvent* fSignalEvent;  //!
    // TODO We must get here a pointer to TRestDaqMetadata
    // In order to convert the parameters to time using the sampling time

    std::vector<std::string> fIntegralObservables;  //!
    std::vector<double> fThreshold;                 //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    //

    Double_t fW;
    Double_t fSampling;
    Int_t fADCLength;

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " W : " << fW << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "triggerAnalysis"; }

    // Constructor
    TRestDetectorTriggerAnalysisProcess();
    TRestDetectorTriggerAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorTriggerAnalysisProcess();

    ClassDef(TRestDetectorTriggerAnalysisProcess, 1);  // Template for a REST "event process" class inherited from
                                               // TRestEventProcess
};
#endif
