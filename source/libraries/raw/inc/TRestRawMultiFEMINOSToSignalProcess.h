///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawMultiFEMINOSToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from
///             TRestProcess
///             How to use: replace TRestRawMultiFEMINOSToSignalProcess by your
///             name, fill the required functions following instructions and add
///             all needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn
///_______________________________________________________________________________

#ifndef RestCore_TRestRawMultiFEMINOSToSignalProcess
#define RestCore_TRestRawMultiFEMINOSToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestSignalEvent.h"

class TRestRawMultiFEMINOSToSignalProcess : public TRestRawToSignalProcess {
   private:
    unsigned short pay;

    unsigned int fLastEventId;
    Double_t fLastTimeStamp;
    void LoadDetectorSetupData();

    Int_t fCounter = 0;  //!

   public:
    void InitProcess();
    void Initialize();
    TRestEvent* ProcessEvent(TRestEvent* evInput);
    TString GetProcessName() { return (TString) "MultiFEMINOSToSignal"; }

    Bool_t ReadFrame(void* fr, int fr_sz);

    // Constructor
    TRestRawMultiFEMINOSToSignalProcess();
    TRestRawMultiFEMINOSToSignalProcess(char* cfgFileName);
    // Destructor
    ~TRestRawMultiFEMINOSToSignalProcess();

    ClassDef(TRestRawMultiFEMINOSToSignalProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
