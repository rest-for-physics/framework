#ifndef RestCore_TRestThread
#define RestCore_TRestThread

#include <iostream>

#include <mutex>
#include <thread>

#include "TFile.h"
#include "TObject.h"
#include "TTree.h"

#include <TKey.h>
#include <string>
#include "TFileMerger.h"
#include "TString.h"

#include "TRestEvent.h"
#include "TRestEventProcess.h"
#include "TRestMetadata.h"

#include "TRestAnalysisTree.h"
#include "TRestProcessRunner.h"

// class TRestRun;
// enum REST_Process_Output;
// class TRestThread;

/// Threaded worker of a process chain
class TRestThread : public TRestMetadata {
   private:
    Bool_t isFinished;
    Int_t fThreadId;

    TRestProcessRunner* fHostRunner;  //!

    TRestEvent* fInputEvent;   //!
    TRestEvent* fOutputEvent;  //!

    TTree* fEventTree;                 //!
    TRestAnalysisTree* fAnalysisTree;  //!

    vector<TRestEventProcess*> fProcessChain;  //!

#ifndef __CINT__
    TFile* fOutputFile;             //!
    vector<string> fTreeBranchDef;  //!
    std::thread t;                  //!
#endif

   public:
    void Initialize();
    void InitFromConfigFile() {}

    void AddProcess(TRestEventProcess* process);
    void PrepareToProcess(bool testrun = true);
    bool TestRun(TRestAnalysisTree* tempTree = NULL);
    void StartProcess();

    void ProcessEvent();
    // void FillEvent();
    void WriteFile();

    void StartThread();

    Int_t ValidateInput(TRestEvent* input);
    Int_t ValidateChain();

    // getter and setter
    void SetThreadId(Int_t id);
    void SetBranchConfig(vector<string> i) { fTreeBranchDef = i; }
    void SetOutputTree(TRestAnalysisTree* t) { fAnalysisTree = t; }
    void SetProcessRunner(TRestProcessRunner* r) { fHostRunner = r; }
    TFile* GetOutputFile() { return fOutputFile; };
    TRestEvent* GetOutputEvent() { return fOutputEvent; }
    Int_t GetProcessnum() { return fProcessChain.size(); }
    TRestEventProcess* GetProcess(int i) { return fProcessChain[i]; }
    TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
    TTree* GetEventTree() { return fEventTree; }
    Bool_t Finished() { return isFinished; }

    TRestThread() { Initialize(); }
    ~TRestThread(){};

    /// Calling CINT
    ClassDef(TRestThread, 1);
};

#endif