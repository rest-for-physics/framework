
#ifndef RestCore_TRestThread
#define RestCore_TRestThread

#include <TFile.h>
#include <TFileMerger.h>
#include <TKey.h>
#include <TObject.h>
#include <TString.h>
#include <TTree.h>

#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "TRestAnalysisTree.h"
#include "TRestEvent.h"
#include "TRestEventProcess.h"
#include "TRestMetadata.h"
#include "TRestProcessRunner.h"

/// Threaded worker of a process chain
class TRestThread : public TRestMetadata {
   private:
    Int_t fThreadId;

    TRestProcessRunner* fHostRunner;                //!
    std::vector<TRestEventProcess*> fProcessChain;  //!
    TRestAnalysisTree* fAnalysisTree;               //!
    TRestEvent* fInputEvent;                        //!
    TRestEvent* fOutputEvent;                       //!
    TFile* fOutputFile;                             //!
    TTree* fEventTree;                              //!

    std::thread t;                //!
    Bool_t isFinished;            //!
    Bool_t fProcessNullReturned;  //!

   public:
    void Initialize();
    void InitFromConfigFile() {}

    void AddProcess(TRestEventProcess* process);
    void PrepareToProcess(bool* outputConfig = 0);
    bool TestRun();
    void StartProcess();

    void ProcessEvent();
    // void FillEvent();
    void EndProcess();

    void StartThread();

    Int_t ValidateChain(TRestEvent* input);

    // getter and setter
    void SetThreadId(Int_t id);
    void SetOutputTree(TRestAnalysisTree* analysisTree) { fAnalysisTree = analysisTree; }
    void SetProcessRunner(TRestProcessRunner* r) { fHostRunner = r; }

    Int_t GetThreadId() { return fThreadId; }
    TRestEvent* GetInputEvent() { return fInputEvent; }
    TFile* GetOutputFile() { return fOutputFile; };
    TRestEvent* GetOutputEvent() { return fProcessNullReturned ? nullptr : fOutputEvent; }
    Int_t GetProcessnum() { return fProcessChain.size(); }
    TRestEventProcess* GetProcess(int i) { return fProcessChain[i]; }
    TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
    TTree* GetEventTree() { return fEventTree; }
    Bool_t Finished() { return isFinished; }

    // Constructor & Destructor
    TRestThread() { Initialize(); }
    ~TRestThread(){};

    /// Calling CINT
    ClassDef(TRestThread, 1);
};

#endif
