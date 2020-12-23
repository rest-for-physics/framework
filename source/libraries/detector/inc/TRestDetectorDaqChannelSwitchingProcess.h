///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorDaqChannelSwitchingProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorDaqChannelSwitchingProcess
#define RestCore_TRestDetectorDaqChannelSwitchingProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorDaqChannelSwitchingProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestEvent* fEvent;              //!
    TRestDetectorReadout* fReadout;  //!
#endif
    std::map<int, int> fFirstDaqChannelDef;  //[module id, first daq id]
    bool fIgnoreUndefinedModules;

    void InitFromConfigFile();

    void Initialize();

   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "module's daq channel re-definition: " << endl;
        auto iter = fFirstDaqChannelDef.begin();
        while (iter != fFirstDaqChannelDef.end()) {
            metadata << "module id: " << iter->first << " first daq channel: " << iter->second << endl;
            iter++;
        }
        metadata << endl;

        EndPrintProcess();
    }

    // Constructor
    TRestDetectorDaqChannelSwitchingProcess();
    // Destructor
    ~TRestDetectorDaqChannelSwitchingProcess();

    ClassDef(TRestDetectorDaqChannelSwitchingProcess, 1);  // Template for a REST "event process" class inherited from
                                                   // TRestEventProcess
};
#endif
