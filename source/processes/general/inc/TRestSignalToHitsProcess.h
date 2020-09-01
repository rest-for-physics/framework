///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalToHitsProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestSignalToHitsProcess
#define RestCore_TRestSignalToHitsProcess

#include <TRestGas.h>
#include <TRestReadout.h>

#include <TRestHitsEvent.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalToHitsProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestHitsEvent* fHitsEvent;      //!
    TRestSignalEvent* fSignalEvent;  //!

#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    Double_t fDriftVelocity;  // mm/us

    TString fSignalToHitMethod;

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Drift velocity : " << fDriftVelocity << " mm/us" << endl;

        metadata << "Signal to hits method : " << fSignalToHitMethod << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "signalToHits"; }

    // Constructor
    TRestSignalToHitsProcess();
    TRestSignalToHitsProcess(char* cfgFileName);
    // Destructor
    ~TRestSignalToHitsProcess();

    ClassDef(TRestSignalToHitsProcess, 2);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
