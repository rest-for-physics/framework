///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSharedMemoryBufferToRawSignalProcess.h
///
///             jun 2018:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestSharedMemoryBufferToRawSignalProcess
#define RestCore_TRestSharedMemoryBufferToRawSignalProcess

#include "TRestEventProcess.h"
#include "TRestRawSignalEvent.h"

typedef struct {
    unsigned int dataReady;
    unsigned int nSignals;
    unsigned int eventId;
    double timeStamp;
    unsigned int maxSignals;
    unsigned int maxSamples;
    unsigned int bufferSize;
} daqInfo;


class TRestSharedMemoryBufferToRawSignalProcess : public TRestEventProcess {

    private:
#ifndef __CINT__
        TRestRawSignalEvent *fSignalEvent;

        Int_t fMaxSignals;
        Int_t fMaxPointsPerSignal;

        Int_t fNdata;

        daqInfo *fShMem_daqInfo;

        int fSemaphoreId;
        unsigned short int *fShMem_Buffer;
#endif

        void InitFromConfigFile();

    public:
        void InitProcess();
        void Initialize();

        TRestEvent *ProcessEvent( TRestEvent *evInput );
        TString GetProcessName(){ return (TString) "TRestSharedMemoryBufferToRawSignal"; }

        //Constructor
        TRestSharedMemoryBufferToRawSignalProcess();
        TRestSharedMemoryBufferToRawSignalProcess(char *cfgFileName);
        //Destructor
        ~TRestSharedMemoryBufferToRawSignalProcess();

        ClassDef(TRestSharedMemoryBufferToRawSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
