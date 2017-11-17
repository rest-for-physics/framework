#ifndef RestCore_TRestTask
#define RestCore_TRestTask

#include "TRestRun.h"
#include "TObject.h"
#include "TRestAnalysisTree.h"
#include "TRestGas.h"
#include "TObject.h"
#include "TLegend.h"
#include "TRestG4Metadata.h"
#include "TRestG4Event.h"
#include "TCanvas.h"
#include "TRestSignal.h"
#include "TF1.h"
#include "TH2D.h"
#include "TStyle.h"

class TRestManager;

using namespace std;
//this is only a macro file for running tasks
class TRestTask :public TRestMetadata {

public:
	TRestTask();
	~TRestTask() {};
	
	ClassDef(TRestTask, 1);
	
	void BeginOfInit();

	virtual void InitTask() {}
	virtual void RunTask(TRestManager*mgr)=0;


};





#endif