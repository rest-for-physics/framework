///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFEMINOSToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestFEMINOSToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
///_______________________________________________________________________________


#ifndef RestCore_TRestFEMINOSToSignalProcess
#define RestCore_TRestFEMINOSToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestSignalEvent.h"

class TRestFEMINOSToSignalProcess:public TRestRawToSignalProcess {

 private:
 
 unsigned short pay;
 
 public:
   void InitProcess();
   void Initialize();
   TRestEvent *ProcessEvent( TRestEvent *evInput );
   TString GetProcessName(){ return (TString) "FEMINOSToSignal"; }
   Int_t GetPhysChannel(int channel);
   
   //Constructor
   TRestFEMINOSToSignalProcess();
   TRestFEMINOSToSignalProcess(char *cfgFileName);
   //Destructor
   ~TRestFEMINOSToSignalProcess();

   ClassDef(TRestFEMINOSToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
