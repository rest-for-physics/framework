///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestRawToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
///_______________________________________________________________________________


#ifndef RestCore_TRestRawToSignalProcess
#define RestCore_TRestRawToSignalProcess

#include "TRestEventProcess.h"
#include "TRestRawSignalEvent.h"
#include <TRestDetectorSetup.h>

class TRestRawToSignalProcess:public TRestEventProcess {

 protected:
 
   void InitFromConfigFile();
   unsigned int payload;
   unsigned int frameBits;
   TString fElectronicsType;
   Int_t fMinPoints;
   
   Double_t tStart;
   int totalBytesReaded;

   TRestRawSignalEvent *fSignalEvent;
   #ifndef __CINT__
   FILE *fInputBinFile;

   Int_t fRunOrigin;
   Int_t fSubRunOrigin;

   TString fFilenameFormat;

   Int_t fShowSamples;
   #endif
   
   void LoadDefaultConfig();

 public:
   virtual void Initialize();
   virtual void InitProcess()=0;
   virtual TRestEvent *ProcessEvent( TRestEvent *evInput )=0;
   virtual void EndProcess();
   virtual void BeginOfEventProcess();
   virtual void EndOfEventProcess();
   virtual TString GetProcessName()=0;
   TRestMetadata *GetProcessMetadata() { return NULL; }

   void SetRunOrigin( Int_t runOrigin ) { fRunOrigin = runOrigin; }
   void SetSubRunOrigin( Int_t subOrigin ) { fSubRunOrigin = subOrigin; }

   TString GetFilenameFormat() { return fFilenameFormat; }

   void LoadConfig( std::string cfgFilename, std::string name = "" );

   void PrintMetadata();
   
   Bool_t OpenInputBinFile(TString fName);
   
   void printBits(unsigned short num);
   void printBits(unsigned int num);
   
   Int_t GetTotalBytesReaded( ){return totalBytesReaded;}
 //  Int_t GetRunNumber(){return fRunNumber;}
 //  Int_t GetRunIndex(){return fRunIndex;}
   TString GetElectronicsType( ){return fElectronicsType;}
   
   //Constructor
   TRestRawToSignalProcess();
   TRestRawToSignalProcess(char *cfgFileName);
   //Destructor
   ~TRestRawToSignalProcess();

   ClassDef(TRestRawToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
