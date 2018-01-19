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
class TRestManager;

using namespace std;
//this is only a macro file for running tasks
class TRestTask :public TRestMetadata {

public:
	TRestTask();
	~TRestTask() {};
	
	ClassDef(TRestTask, 1);
	
	int fNRequiredArgument;

	//define default values here
	void Initialize() {}
	//automatic setup values with rml config section
	void BeginOfInit();
	//automatic set values with input argument
	void InitTask(vector<string>argument);


	virtual void RunTask(TRestManager*)=0;
	virtual void PrintHelp();

};





#endif