#ifndef RestCore_TRestProcessRunner
#define RestCore_TRestProcessRunner

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestEventProcess.h"
#include "TRestRun.h"
#include "TRestAnalysisTree.h"
#include <mutex>
#include <thread>


class TRestThread;
class TRestManager;

enum ProcStatus {
	kNormal,//!< normal processing
	kPause,//!< pausing the process, showing a menu
	kStep, //!< step to next event. this state will then fall back to kPause
	kStop,//!< stopping the process, files will be saved
	kIgnore,//!< do not response to keyboard orders
};

class TRestProcessRunner :public TRestMetadata {
public:
	/// REST run class
	ClassDef(TRestProcessRunner, 1);

	TRestProcessRunner();
	~TRestProcessRunner();
	void Initialize();
	void InitFromConfigFile() { TRestMetadata::InitFromConfigFile(); }
	void BeginOfInit();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);
	void EndOfInit();

	//core functionality
	void ReadProcInfo();
	void RunProcess();
	void PauseMenu();
	Int_t GetNextevtFunc(TRestEvent* targetevt, TRestAnalysisTree* targettree);
	void FillThreadEventFunc(TRestThread* t);
	void WriteThreadFileFunc(TRestThread* t);
	void ConfigOutputFile();


	//tools
	void ResetRunTimes();
	TRestEventProcess* InstantiateProcess(TString type, TiXmlElement* ele);
	void PrintProcessedEvents(Int_t rateE);
	string MakeProgressBar(int progress100,int length=100);
	void SetProcStatus(ProcStatus s) { fProcStatus = s; }


	//getters and setters
	TRestEvent* GetInputEvent();
	TFile* GetTempOutputDataFile() { return fTempOutputDataFile; }
	string GetProcInfo(string infoname) { return ProcessInfo[infoname] == "" ? infoname : ProcessInfo[infoname]; }
	TRestAnalysisTree* GetAnalysisTree();
private:
	//global variable
	TRestRun *fRunInfo;//!


	//event variables
	TRestEvent *fInputEvent;//!
	TRestEvent *fOutputEvent;//!

	//self variables for processing
	vector<TRestThread*> fThreads;//!
	TFile* fTempOutputDataFile;//!
	TTree* fEventTree;//!
	TRestAnalysisTree* fAnalysisTree;//!
	ProcStatus fProcStatus;
	Int_t nBranches;
	Int_t fThreadNumber;
	Int_t fProcessNumber;
	Int_t firstEntry;
	Int_t eventsToProcess;
	Int_t fProcessedEvents;
	map<string, string> ProcessInfo;
	vector<string> fOutputItem;





};

#endif