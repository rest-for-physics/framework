///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMultiFEMINOSToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from
///             TRestProcess
///             How to use: replace TRestMultiFEMINOSToSignalProcess by your
///             name, fill the required functions following instructions and add
///             all needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn
///_______________________________________________________________________________

#ifndef RestCore_TRestMultiFEMINOSToSignalProcess
#define RestCore_TRestMultiFEMINOSToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestSignalEvent.h"

class TRestMultiFEMINOSToSignalProcess : public TRestRawToSignalProcess {
   private:
    unsigned short pay;

    unsigned int fLastEventId;
    Double_t fLastTimeStamp;
    void LoadDetectorSetupData();

   public:
    void InitProcess();
    void Initialize();
    TRestEvent* ProcessEvent(TRestEvent* evInput);
    TString GetProcessName() { return (TString) "MultiFEMINOSToSignal"; }

    Bool_t ReadFrame(void* fr, int fr_sz);

    // Constructor
    TRestMultiFEMINOSToSignalProcess();
    TRestMultiFEMINOSToSignalProcess(char* cfgFileName);
    // Destructor
    ~TRestMultiFEMINOSToSignalProcess();

    ClassDef(TRestMultiFEMINOSToSignalProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
