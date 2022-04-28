
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
    void Initialize() override;
    void InitFromConfigFile() override {}

    void AddProcess(TRestEventProcess* process);
    void PrepareToProcess(bool* outputConfig = nullptr);
    bool TestRun();
    void StartProcess();

    void ProcessEvent();
    void EndProcess();
    void StartThread();

    Int_t ValidateChain(TRestEvent* input);

    // getter and setter
    void SetThreadId(Int_t id);
    inline void SetOutputTree(TRestAnalysisTree* analysisTree) { fAnalysisTree = analysisTree; }
    inline void SetProcessRunner(TRestProcessRunner* r) { fHostRunner = r; }

    inline Int_t GetThreadId() const { return fThreadId; }
    inline TRestEvent* GetInputEvent() { return fInputEvent; }
    inline TFile* GetOutputFile() { return fOutputFile; };
    inline TRestEvent* GetOutputEvent() { return fProcessNullReturned ? 0 : fOutputEvent; }
    inline Int_t GetProcessnum() const { return fProcessChain.size(); }
    inline TRestEventProcess* GetProcess(int i) const { return fProcessChain[i]; }
    inline TRestAnalysisTree* GetAnalysisTree() const { return fAnalysisTree; }
    inline TTree* GetEventTree() const { return fEventTree; }
    inline Bool_t Finished() const { return isFinished; }

    // Constructor & Destructor
    TRestThread() { Initialize(); }
    ~TRestThread(){};

    ClassDefOverride(TRestThread, 1);
};

#endif
