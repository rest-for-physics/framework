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
    Int_t fThreadId;

    TRestProcessRunner* fHostRunner;           //!
    vector<TRestEventProcess*> fProcessChain;  //!
    TRestAnalysisTree* fAnalysisTree;          //!
    TRestEvent* fInputEvent;                   //!
    TRestEvent* fOutputEvent;                  //!
    TFile* fOutputFile;                        //!
    TTree* fEventTree;                         //!

    std::thread t;      //!
    Bool_t isFinished;  //!

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