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
    TFile* fOutputDataFile;       //! the TFile pointer being used
    TString fOutputDataFileName;  //! indicates the name of the first file created as output data file. The actual
                                  //! output file maybe changed if tree is too large
    TTree* fEventTree;            //!
    TRestAnalysisTree* fAnalysisTree;  //!
    ProcStatus fProcStatus;            //!
    Int_t fNBranches;                  //!
    Int_t fNFilesSplit;                //! Number of files being split.

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

    Long64_t fFileSplitSize;  // in bytes
    Int_t fFileCompression;   // 1~9
    std::map<std::string, std::string> fProcessInfo;

    // bool fOutputItem[4] = {
    //    false};  // the on/off status for item: inputAnalysis, inputEvent, outputEvent, outputAnalysis

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
    void MergeOutputFile();
    void WriteMetadata();

    // tools
    void ResetRunTimes();
    TRestEventProcess* InstantiateProcess(TString type, TiXmlElement* ele);
    void PrintProcessedEvents(Int_t rateE);
    std::string MakeProgressBar(int progress100, int length = 100);

    // getters and setters
    TRestEvent* GetInputEvent();
    TRestAnalysisTree* GetInputAnalysisTree();
    TRestAnalysisTree* GetOutputAnalysisTree() { return fAnalysisTree; }
    TFile* GetOutputDataFile() { return fOutputDataFile; }
    std::string GetProcInfo(std::string infoname) {
        return fProcessInfo[infoname] == "" ? infoname : fProcessInfo[infoname];
    }
    int GetNThreads() { return fThreadNumber; }
    int GetNProcesses() { return fProcessNumber; }
    int GetNProcessedEvents() { return fProcessedEvents; }
    double GetReadingSpeed();
    bool UseTestRun() const { return fUseTestRun; }
    ProcStatus GetStatus() { return fProcStatus; }
    Long64_t GetFileSplitSize() { return fFileSplitSize; }

    // Constructor & Destructor
    TRestProcessRunner();
    ~TRestProcessRunner();

    ClassDef(TRestProcessRunner, 7);
};

#endif