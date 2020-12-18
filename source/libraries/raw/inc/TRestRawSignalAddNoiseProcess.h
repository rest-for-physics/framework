///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalAddNoiseProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestRawSignalAddNoiseProcess
#define RestCore_TRestRawSignalAddNoiseProcess

#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

class TRestRawSignalAddNoiseProcess : public TRestEventProcess {
   private:
    TRestRawSignalEvent* fInputSignalEvent;
    TRestRawSignalEvent* fOutputSignalEvent;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

    Double_t fNoiseLevel;

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

        metadata << "Noise Level : " << fNoiseLevel << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return NULL; }

    TString GetProcessName() { return (TString) "rawSignalAddNoise"; }

    // Constructor
    TRestRawSignalAddNoiseProcess();
    TRestRawSignalAddNoiseProcess(char* cfgFileName);
    // Destructor
    ~TRestRawSignalAddNoiseProcess();

    ClassDef(TRestRawSignalAddNoiseProcess, 1);  // Template for a REST "event process" class inherited from
                                                 // TRestEventProcess
};
#endif
