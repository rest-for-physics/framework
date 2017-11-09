#ifndef RestCore_TRestThread
#define RestCore_TRestThread

#include <iostream>

#include <mutex>
#include <thread>


#include "TObject.h"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include "TString.h"
#include <TKey.h>
#include "TFileMerger.h"

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestEventProcess.h"

#include "TRestAnalysisTree.h"
#include "TRestProcessRunner.h"

//class TRestRun;
//enum REST_Process_Output;
//class TRestThread;


class TRestThread : public TRestMetadata
{
public:
	/// Calling CINT
	ClassDef(TRestThread, 1);

	TRestThread() { Initialize(); }
	~TRestThread() {};

	void Initialize();

	void PrepareToProcess();

	void StartProcess();


	void ProcessEvent();
	//void FillEvent();
	void WriteFile();

	void StartThread();


	void AddProcess(TRestEventProcess *process) { fProcessChain.push_back(process); }

	Int_t ValidateInput(TRestEventProcess* procinput);
	Int_t ValidateChain();


	//getter and setter
	void SetThreadId(Int_t id) { fThreadId = id; }
	void SetBranchConfig(vector<string> i) { fTreeBranches = i; }
	void SetOutputTree(TRestAnalysisTree* t) { fTree = t; }
	void SetTRestRunner(TRestProcessRunner* r) { fHostRunner = r; }
	TFile* GetOutputFile() { return fOutputFile; };
	Int_t GetProcessnum() { return fProcessChain.size(); }
	TRestEventProcess* GetProcess(int i) { return fProcessChain[i]; }
	TRestAnalysisTree* GetAnalysisTree() { return fTree; }
	Bool_t Finished() { return isFinished; }

private:

	Bool_t isFinished;
	Int_t fThreadId;

	TRestProcessRunner* fHostRunner;//!

	TRestEvent* fInputEvent;//!
	TRestEvent* fOutputEvent;//!

	TRestAnalysisTree* fTree;//!



	vector<TRestEventProcess*> fProcessChain;//!
	
#ifndef __CINT__
	TFile* fOutputFile;//!
	vector<string> fTreeBranches;//!
	std::thread t;//!
#endif
};



#endif