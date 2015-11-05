///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAGETToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestAGETToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
///_______________________________________________________________________________


#ifndef RestCore_TRestAGETToSignalProcess
#define RestCore_TRestAGETToSignalProcess

#include "TRestEventProcess.h"
#include "TRestSignalEvent.h"

class TRestAGETToSignalProcess:public TRestEventProcess {

 protected:
 
   void InitFromConfigFile();
   unsigned short payload;
   int frameBits;
   
 
 private:
   
   Double_t tStart;
   int totalBytesReaded;
   
   TRestSignalEvent *fSignalEvent;
   #ifndef __CINT__
   FILE *fInputBinFile;
   int fRunNumber;
   int fRunIndex;
   #endif
   
   void LoadDefaultConfig();

 public:
   void Initialize();
   void InitProcess();
   TRestEvent *ProcessEvent( TRestEvent *evInput );
   void EndProcess();
   void BeginOfEventProcess();
   void EndOfEventProcess();
   TString GetProcessName(){ return (TString) "AGETToSignal"; }
   TRestMetadata *GetMetadata() { return NULL; }
   
   void PrintMetadata() { cout << "TODO : Needs to be implemented" << endl; }
   
   Bool_t OpenInputBinFile(TString fName);
   
   void printBits(unsigned short num);
   void printBits(unsigned int num);
   
   Int_t GetTotalBytesReaded( ){return totalBytesReaded;}
   Int_t GetRunNumber(){return fRunNumber;}
   Int_t GetRunIndex(){return fRunIndex;}
   
   //Constructor
   TRestAGETToSignalProcess();
   TRestAGETToSignalProcess(char *cfgFileName);
   //Destructor
   ~TRestAGETToSignalProcess();

   ClassDef(TRestAGETToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
