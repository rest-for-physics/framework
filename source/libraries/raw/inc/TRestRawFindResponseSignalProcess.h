///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawFindResponseSignalProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestRawFindResponseSignalProcess
#define RestCore_TRestRawFindResponseSignalProcess

#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

class TRestRawFindResponseSignalProcess : public TRestEventProcess {
   private:
    TRestRawSignalEvent* fInputSignalEvent;   //!
    TRestRawSignalEvent* fOutputSignalEvent;  //!

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fInputSignalEvent; }
    any GetOutputEvent() { return fOutputSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return NULL; }

    TString GetProcessName() { return (TString) "findResponseSignal"; }

    // Constructor
    TRestRawFindResponseSignalProcess();
    TRestRawFindResponseSignalProcess(char* cfgFileName);
    // Destructor
    ~TRestRawFindResponseSignalProcess();

    ClassDef(TRestRawFindResponseSignalProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
