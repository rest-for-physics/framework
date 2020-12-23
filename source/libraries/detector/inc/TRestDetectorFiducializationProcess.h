///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorFiducializationProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorFiducializationProcess
#define RestCore_TRestDetectorFiducializationProcess

#include <TRestDetectorReadout.h>
#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorFiducializationProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    TRestDetectorReadout* fReadout;  //!

#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
   public:
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "fiducialization"; }

    // Constructor
    TRestDetectorFiducializationProcess();
    TRestDetectorFiducializationProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorFiducializationProcess();

    ClassDef(TRestDetectorFiducializationProcess, 1);  // Template for a REST "event process" class inherited from
                                               // TRestEventProcess
};
#endif
