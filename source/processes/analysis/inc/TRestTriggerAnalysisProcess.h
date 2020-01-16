///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTriggerAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestTriggerAnalysisProcess
#define RestCore_TRestTriggerAnalysisProcess

#include <TH1D.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestTriggerAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestSignalEvent* fSignalEvent;  //!
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
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        std::cout << " W : " << fW << std::endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "triggerAnalysis"; }

    // Constructor
    TRestTriggerAnalysisProcess();
    TRestTriggerAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestTriggerAnalysisProcess();

    ClassDef(TRestTriggerAnalysisProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
