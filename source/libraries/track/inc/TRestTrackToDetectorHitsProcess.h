//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackToDetectorHitsProcess.h
///
///             Apr 2017 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestTrackToDetectorHitsProcess
#define RestCore_TRestTrackToDetectorHitsProcess

#include <TRestDetectorHitsEvent.h>
#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackToDetectorHitsProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;  //!
    TRestDetectorHitsEvent* fOutputHitsEvent;   //!
#endif

    void InitFromConfigFile();

    void Initialize();

   protected:
    Int_t fTrackLevel;

   public:
    any GetInputEvent() { return fInputTrackEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        cout << "Track level : " << fTrackLevel << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "trackToDetectorHits"; }

    // Constructor
    TRestTrackToDetectorHitsProcess();
    TRestTrackToDetectorHitsProcess(char* cfgFileName);
    // Destructor
    ~TRestTrackToDetectorHitsProcess();

    ClassDef(TRestTrackToDetectorHitsProcess, 1);  // Template for a REST "event process" class inherited from
                                                   // TRestEventProcess
};
#endif
