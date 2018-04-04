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
//this is only a macro file for running tasks
class TRestTask :public TRestMetadata {

public:
	TRestTask();
	TRestTask(TString MacroFileName);
	~TRestTask() {};
	
	ClassDef(TRestTask, 1);
	
	int fNRequiredArgument;//!

	//define default values here
	void InitFromConfigFile() { TRestMetadata::InitFromConfigFile(); }
	//automatic setup values with rml config section
	void BeginOfInit();

	void SetArgumentValue(string name, string value);
	//automatic set values with input argument
	static TRestTask* GetTask(TString Name);

	bool InitTask(vector<string>arg);


	virtual void RunTask(TRestManager*);
	virtual void PrintHelp();

private:
	string methodname;
	vector<int>argumenttype;
	vector<string>argumentname;
	vector<string>argument;//!
	TString cmdstr="";//!
};





#endif