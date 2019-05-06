//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackToHitsProcess.h
///
///             Apr 2017 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestTrackToHitsProcess
#define RestCore_TRestTrackToHitsProcess

#include <TRestHitsEvent.h>
#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackToHitsProcess : public TRestEventProcess {
 private:
#ifndef __CINT__
  TRestTrackEvent* fInputTrackEvent;  //!
  TRestHitsEvent* fOutputHitsEvent;   //!
#endif

  void InitFromConfigFile();

  void Initialize();

 protected:
  Int_t fTrackLevel;

 public:
  void InitProcess();
  TRestEvent* ProcessEvent(TRestEvent* eventInput);
  void EndOfEventProcess();
  void EndProcess();
  void LoadDefaultConfig();

  void LoadConfig(std::string cfgFilename, std::string name = "");

  void PrintMetadata() {
    BeginPrintProcess();

    cout << "Track level : " << fTrackLevel << endl;

    EndPrintProcess();
  }

  TString GetProcessName() { return (TString) "trackToHits"; }

  // Constructor
  TRestTrackToHitsProcess();
  TRestTrackToHitsProcess(char* cfgFileName);
  // Destructor
  ~TRestTrackToHitsProcess();

  ClassDef(TRestTrackToHitsProcess,
           1);  // Template for a REST "event process" class inherited from
                // TRestEventProcess
};
#endif
