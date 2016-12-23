///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestCoBoAsAdToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestCoBoAsAdToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Xinglong
///_______________________________________________________________________________


#ifndef RestCore_TRestCoBoAsAdToSignalProcess
#define RestCore_TRestCoBoAsAdToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestSignalEvent.h"

class TRestCoBoAsAdToSignalProcess:public TRestRawToSignalProcess {

 private:
 
   TTimeStamp fStartTimeStamp;
 
 public:
   void InitProcess();
   void Initialize();
   Bool_t OpenInputCoBoAsAdBinFile(TString fName);
   TRestEvent *ProcessEvent( TRestEvent *evInput );
   TString GetProcessName(){ return (TString) "CoBoAsAdToSignal"; }
   
   //Constructor
   TRestCoBoAsAdToSignalProcess();
   TRestCoBoAsAdToSignalProcess(char *cfgFileName);
   //Destructor
   ~TRestCoBoAsAdToSignalProcess();

   ClassDef(TRestCoBoAsAdToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
