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
    kNormal,    //!< normal processing
    kPause,     //!< pausing the process, showing a menu
    kStep,      //!< step to next event. this state will then fall back to kPause
    kStopping,  //!< caller to stop the process during the run
    kIgnore,    //!< do not response to keyboard orders
    kFinished,  //!< finished state of process running
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
    std::vector<TRestThread*> fThreads;  //!
    TFile* fTempOutputDataFile;          //!
    TTree* fEventTree;                   //!
    TRestAnalysisTree* fAnalysisTree;    //!
    ProcStatus fProcStatus;              //!
    Int_t fNBranches;                    //!

    // metadata
    Bool_t fUseTestRun;
    Bool_t fUsePauseMenu;
    Bool_t fValidateObservables;
    Bool_t fSortOutputEvents;
    Bool_t fInputAnalysisStorage;
    Bool_t fInputEventStorage;
    Bool_t fOutputEventStorage;
    Bool_t fOutputAnalysisStorage;
    Int_t fThreadNumber;
    Int_t fProcessNumber;
    Int_t fFirstEntry;
    Int_t fEventsToProcess;
    Int_t fProcessedEvents;
    std::map<std::string, std::string> fProcessInfo;

   public:
    /// REST run class
    void Initialize();
    void InitFromConfigFile() {
        BeginOfInit();
        if (fElement != nullptr) {
            TiXmlElement* e = fElement->FirstChildElement();
            while (e != nullptr) {
                std::string value = e->Value();
                if (value == "variable" || value == "myParameter" || value == "constant") {
                    e = e->NextSiblingElement();
                    continue;
                }
                ReadConfig((std::string)e->Value(), e);
                e = e->NextSiblingElement();
            }
        }
        EndOfInit();
    }
    void BeginOfInit();
    Int_t ReadConfig(std::string keydeclare, TiXmlElement* e);
    void EndOfInit();
    void PrintMetadata();

    // core functionality
    void ReadProcInfo();
    void RunProcess();
    void PauseMenu();
    Int_t GetNextevtFunc(TRestEvent* targetevt, TRestAnalysisTree* targettree);
    void FillThreadEventFunc(TRestThread* t);
    void ConfigOutputFile();

    // tools
    void ResetRunTimes();
    TRestEventProcess* InstantiateProcess(TString type, TiXmlElement* ele);
    void PrintProcessedEvents(Int_t rateE);
    std::string MakeProgressBar(int progress100, int length = 100);

    // getters and setters
    TRestEvent* GetInputEvent();
    TRestAnalysisTree* GetInputAnalysisTree();
    TRestAnalysisTree* GetOutputAnalysisTree() { return fAnalysisTree; }
    TFile* GetTempOutputDataFile() { return fTempOutputDataFile; }
    std::string GetProcInfo(std::string infoname) {
        return fProcessInfo[infoname] == "" ? infoname : fProcessInfo[infoname];
    }
    inline int GetNThreads() const { return fThreadNumber; }
    inline int GetNProcesses() const { return fProcessNumber; }
    inline int GetNProcessedEvents() const { return fProcessedEvents; }
    double GetReadingSpeed();
    bool UseTestRun() const { return fUseTestRun; }
    inline ProcStatus GetStatus() const { return fProcStatus; }

    // Constructor & Destructor
    TRestProcessRunner();
    ~TRestProcessRunner();

    ClassDef(TRestProcessRunner, 6);
};

#endif