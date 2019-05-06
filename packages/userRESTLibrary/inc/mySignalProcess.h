///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             mySignalProcess.h
///
///_______________________________________________________________________________

#ifndef _mySignalProcess
#define _mySignalProcess

#include "TRestEventProcess.h"
#include "TRestSignalEvent.h"

class mySignalProcess : public TRestEventProcess {
 private:
  // We define specific input/output event data holders
  TRestSignalEvent* fInputSignalEvent;
  TRestSignalEvent* fOutputSignalEvent;

  void InitFromConfigFile();

  void Initialize();

  // add here the metadata members of your event process
  // You can just remove fMyProcessParameter
  Int_t fMyDummyParameter;

 protected:
 public:
  TRestEvent* ProcessEvent(TRestEvent* eventInput);

  void PrintMetadata() {
    BeginPrintProcess();

    std::cout << "A dummy Process parameter : " << fMyDummyParameter
              << std::endl;

    EndPrintProcess();
  }

  // Constructor
  mySignalProcess();
  // Destructor
  ~mySignalProcess();

  ClassDef(mySignalProcess, 1);  // Template for a REST "event process" class
                                 // inherited from TRestEventProcess
};
#endif
