
///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAFTERToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from
///             TRestProcess
///             How to use: replace TRestAFTERToSignalProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn
///_______________________________________________________________________________

#ifndef RestCore_TRestAFTERToSignalProcess
#define RestCore_TRestAFTERToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestSignalEvent.h"

// ATENTION: new T2K Daq versions 2.X need to read one extra word
#define NEW_DAQ_T2K_2_X

//--------------------------------------------------------
// Structure acquisition data
//--------------------------------------------------------
struct EventHeader {
  int eventSize;
  int eventNumb;
  // int eventTime;
  // unsigned short dummy;
};

#ifdef NEW_DAQ_T2K_2_X
// ATENTION!!!!!
// New verison of the DaqT2K (2.x)
// added 30th July 2012 (JuanAn)
struct DataPacketHeader {
  unsigned short size;
  unsigned short dcc;
  unsigned short hdr;
  unsigned short args;
  unsigned short ts_h;
  unsigned short ts_l;
  unsigned short ecnt;
  unsigned short scnt;
};
#else
struct DataPacketHeader {
  unsigned short size;
  unsigned short hdr;
  unsigned short args;
  unsigned short ts_h;
  unsigned short ts_l;
  unsigned short ecnt;
  unsigned short scnt;
};
#endif

struct DataPacketEnd {
  unsigned short crc1;
  unsigned short crc2;
};

class TRestAFTERToSignalProcess : public TRestRawToSignalProcess {
 protected:
  unsigned int prevTime;
  double reducedTime;

 public:
  void Initialize();
  void InitProcess();
  TRestEvent* ProcessEvent(TRestEvent* evInput);
  TString GetProcessName() { return (TString) "AFTERToSignal"; }
  TRestMetadata* GetProcessMetadata() { return NULL; }

  Bool_t isExternal() { return true; }

  // Constructor
  TRestAFTERToSignalProcess();
  TRestAFTERToSignalProcess(char* cfgFileName);
  // Destructor
  ~TRestAFTERToSignalProcess();

  ClassDef(TRestAFTERToSignalProcess,
           1);  // Template for a REST "event process" class inherited from
                // TRestEventProcess
};
#endif
