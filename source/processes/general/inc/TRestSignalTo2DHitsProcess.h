///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalTo2DHitsProcess.h
///
///_______________________________________________________________________________

#ifndef RESTCore_TRestSignalTo2DHitsProcess
#define RESTCore_TRestSignalTo2DHitsProcess

#include "TRest2DHitsEvent.h"
#include "TRestEventProcess.h"
#include "TRestSignalEvent.h"

class TRestSignalTo2DHitsProcess : public TRestEventProcess {
   private:
    // We define specific input/output event data holders
    TRestSignalEvent* fInputSignalEvent;   //!
    TRest2DHitsEvent* fOutput2DHitsEvent;  //!

    TRestReadout* fReadout;  //!

    void InitFromConfigFile();

    void Initialize();

    // add here the metadata members of your event process
    // You can just remove fMyProcessParameter
    // int fNoiseReductionLevel;//0: no reduction, 1: subtract baseline, 2:
    // subtract baseline plus threshold

   protected:
   public:
    void InitProcess();

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void MakeCluster();

    TRest2DHitsEvent* SelectTag();

    void MuDepos(TRest2DHitsEvent* eve);

    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    // Constructor
    TRestSignalTo2DHitsProcess();
    // Destructor
    ~TRestSignalTo2DHitsProcess();

    ClassDef(TRestSignalTo2DHitsProcess,
             2);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
