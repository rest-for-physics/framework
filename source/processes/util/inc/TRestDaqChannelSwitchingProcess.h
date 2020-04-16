///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDaqChannelSwitchingProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDaqChannelSwitchingProcess
#define RestCore_TRestDaqChannelSwitchingProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDaqChannelSwitchingProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestEvent* fEvent;      //!
    TRestReadout* fReadout;  //!
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
    TRestDaqChannelSwitchingProcess();
    // Destructor
    ~TRestDaqChannelSwitchingProcess();

    ClassDef(TRestDaqChannelSwitchingProcess, 1);  // Template for a REST "event process" class inherited from
                                                   // TRestEventProcess
};
#endif
