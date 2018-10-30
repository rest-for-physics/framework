#ifndef RestCore_TRestTask
#define RestCore_TRestTask

#include "TRestRun.h"
#include "TObject.h"
#include "TRestAnalysisTree.h"
#include "TObject.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TInterpreter.h"
class TRestManager;

using namespace std;

enum REST_TASKMODE {
	TASK_ERROR = -1,
	TASK_MACRO = 0,
	TASK_CPPCMD=1,
	TASK_CLASS=2
};

/// Wrapping REST macros into tasks
class TRestTask :public TRestMetadata {

public:
	TRestTask();

	~TRestTask() {};
	
	ClassDef(TRestTask, 1);
	
	//define default values here
	void InitFromConfigFile();

	void SetArgumentValue(vector<string>arg);

	static TRestTask* GetTask(TString Name);
	static TRestTask* ParseCommand(TString cmd);

	virtual void RunTask(TRestManager*);
	virtual void PrintArgumentHelp();

	void SetMode(REST_TASKMODE mod) { fMode = mod; }
	REST_TASKMODE GetMode() { return fMode; }
protected:
	TRestTask(TString TaskString, REST_TASKMODE mode = TASK_MACRO);

	void ConstructCommand();

	int fNRequiredArgument;
	REST_TASKMODE fMode;
	string targetname="";
	string methodname="";
	vector<int>argumenttype;//!
	vector<string>argumentname;//!
	vector<string>argument;
	string cmdstr="";
};





#endif