#ifndef RestCore_TRestProcessRunner
#define RestCore_TRestProcessRunner

#include <mutex>
#include <thread>
#include "TRestAnalysisTree.h"
#include "TRestEvent.h"
#include "TRestEventProcess.h"
#include "TRestMetadata.h"
#include "TRestRun.h"

#define TIME_MEASUREMENT

class TRestThread;
class TRestManager;

enum ProcStatus {
    kNormal,  //!< normal processing
    kPause,   //!< pausing the process, showing a menu
    kStep,    //!< step to next event. this state will then fall back to kPause
    kStop,    //!< stopping the process, files will be saved
    kIgnore,  //!< do not response to keyboard orders
};

/// Running the processes efficiently with fantastic display.
class TRestProcessRunner : public TRestMetadata {
   private:
    // global variable
    TRestRun* fRunInfo;  //!

    // event variables
    TRestEvent* fInputEvent;   //!
    TRestEvent* fOutputEvent;  //!

    // self variables for processing
    vector<TRestThread*> fThreads;     //!
    TFile* fTempOutputDataFile;        //!
    TTree* fEventTree;                 //!
    TRestAnalysisTree* fAnalysisTree;  //!
    ProcStatus fProcStatus;
    Int_t nBranches;
    Int_t fThreadNumber;
    Int_t fProcessNumber;
    Int_t firstEntry;
    Int_t eventsToProcess;
    Int_t fProcessedEvents;
    map<string, string> ProcessInfo;
    bool fOutputItem[4] = {false};     // the on/off status for item: inputAnalysis, inputEvent, outputEvent, outputAnalysis

   public:
    /// REST run class
    void Initialize();
    void InitFromConfigFile() {
        BeginOfInit();
        if (fElement != NULL) {
            TiXmlElement* e = fElement->FirstChildElement();
            while (e != NULL) {
                string value = e->Value();
                if (value == "variable" || value == "myParameter" || value == "constant") {
                    e = e->NextSiblingElement();
                    continue;
                }
                ReadConfig((string)e->Value(), e);
                e = e->NextSiblingElement();
            }
        }
        EndOfInit();
    }
    void BeginOfInit();
    Int_t ReadConfig(string keydeclare, TiXmlElement* e);
    void EndOfInit();
    void PrintMetadata();

    // core functionality
    void ReadProcInfo();
    void RunProcess();
    void PauseMenu();
    Int_t GetNextevtFunc(TRestEvent* targetevt, TRestAnalysisTree* targettree);
    void FillThreadEventFunc(TRestThread* t);
    void WriteThreadFileFunc(TRestThread* t);
    void ConfigOutputFile();

    // tools
    void ResetRunTimes();
    TRestEventProcess* InstantiateProcess(TString type, TiXmlElement* ele);
    void PrintProcessedEvents(Int_t rateE);
    string MakeProgressBar(int progress100, int length = 100);
    void SetProcStatus(ProcStatus s) { fProcStatus = s; }

    // getters and setters
    TRestEvent* GetInputEvent();
    TRestAnalysisTree* GetInputAnalysisTree();
    TRestAnalysisTree* GetOutputAnalysisTree() { return fAnalysisTree; }
    TFile* GetTempOutputDataFile() { return fTempOutputDataFile; }
    string GetProcInfo(string infoname) {
        return ProcessInfo[infoname] == "" ? infoname : ProcessInfo[infoname];
    }
    int GetNThreads() { return fThreadNumber; }
    int GetNProcesses() { return fProcessNumber; }
    int GetNProcessedEvents() { return fProcessedEvents; }
    double GetReadingSpeed();

    // Construtor & Destructor
    TRestProcessRunner();
    ~TRestProcessRunner();

    ClassDef(TRestProcessRunner, 2);
};

#endif