//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsShuffleProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorHitsShuffleProcess
#define RestCore_TRestDetectorHitsShuffleProcess

#include <TRandom3.h>
#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsShuffleProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fHitsEvent;  //!

    TRandom3* fRandom;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

   protected:
    Int_t fIterations;

   public:
    any GetInputEvent() { return fHitsEvent; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Iterations : " << fIterations << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "hitsShuffle"; }

    // Constructor
    TRestDetectorHitsShuffleProcess();
    TRestDetectorHitsShuffleProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsShuffleProcess();

    ClassDef(TRestDetectorHitsShuffleProcess, 1);  // Template for a REST "event process" class inherited from
                                           // TRestEventProcess
};
#endif
