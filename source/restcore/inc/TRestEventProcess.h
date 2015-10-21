///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventProcess.h
///
///             Base class from which to inherit all REST process classes inherit
///             A process class in REST deals with one input, ouput (or both) single REST
///             events. RestMetadata controls the access to config files.
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#ifndef RestCore_TRestEventProcess
#define RestCore_TRestEventProcess

#include "TNamed.h"
#include "TRestEvent.h"
#include "TRestMetadata.h"

class TRestEventProcess:public TRestMetadata {
 protected:
   Int_t fStatusOfProcess;		// integer to hold the status of task: 0 = successful >0 = some error happened
   Int_t fVerbose;              //! Verbose level of the process execution [0...3]

   virtual void InitFromConfigFile() = 0;

   TRestEvent *fInputEvent;
   TRestEvent *fOutputEvent;

 public:
   virtual TRestEvent *GetInputEvent() { return fInputEvent; }
   virtual TRestEvent *GetOutputEvent() { return fOutputEvent; }

   virtual void InitProcess() = 0;
   virtual TRestEvent *ProcessEvent( TRestEvent *evInput ) = 0;
   virtual void EndProcess() = 0;
   virtual void BeginOfEventProcess() = 0;
   virtual void EndOfEventProcess() = 0;
   virtual TString GetProcessName() = 0;
   virtual TRestMetadata *GetMetadata() { return NULL; }


   //Getters
   Int_t GetStatus() { return fStatusOfProcess; }
   Int_t GetVerboseLevel() { return fVerbose; } 
   //Setters
   void SetVerboseLevel(Int_t verbose) { fVerbose = verbose; }

   //Constructor
   TRestEventProcess();
   //Destructor
   ~TRestEventProcess();

   ClassDef(TRestEventProcess, 1);      // Base class for a REST process
};
#endif
