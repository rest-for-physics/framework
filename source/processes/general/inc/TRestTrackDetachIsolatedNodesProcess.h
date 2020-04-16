//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackDetachIsolatedNodesProcess.h
///
///              Jan 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestTrackDetachIsolatedNodesProcess
#define RestCore_TRestTrackDetachIsolatedNodesProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackDetachIsolatedNodesProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;   //!
    TRestTrackEvent* fOutputTrackEvent;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    Double_t fThresholdDistance;
    Double_t fConnectivityThreshold;

    Double_t fTubeLengthReduction;
    Double_t fTubeRadius;

   protected:
   public:
    any GetInputEvent() { return fInputTrackEvent; }
    any GetOutputEvent() { return fOutputTrackEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "trackDetachIsolatedNode"; }

    // Constructor
    TRestTrackDetachIsolatedNodesProcess();
    TRestTrackDetachIsolatedNodesProcess(char* cfgFileName);
    // Destructor
    ~TRestTrackDetachIsolatedNodesProcess();

    ClassDef(TRestTrackDetachIsolatedNodesProcess, 1);
};
#endif
