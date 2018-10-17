#ifndef RestCore_TRestManager
#define RestCore_TRestManager

#include <iostream>

#include "TObject.h"

#include "TRestMetadata.h"
#include "TRestRun.h"
#include "TRestProcessRunner.h"
#include "TRestAnalysisPlot.h"
#include "TApplication.h"

#include "TRestTask.h"
#include <TROOT.h>

/// Managing applications and executing tasks
class TRestManager : public TRestMetadata {

private:
	///app-like metadata objects
	vector<TRestMetadata*> fMetaObjects;//!

public:
	/// Call CINT to generate streamers for this class
	ClassDef(TRestManager, 1);

	TRestManager();
	~TRestManager();

	void Initialize();
	void InitFromConfigFile() { TRestMetadata::InitFromConfigFile(); }
	void BeginOfInit();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);

	//void LaunchTasks();

	void PrintMetadata();

	TRestProcessRunner*GetProcessRunner() { return (TRestProcessRunner*)GetApplication("TRestProcessRunner"); }
	TRestRun *GetRunInfo() { return (TRestRun*)GetApplication("TRestRun"); }
	TRestAnalysisPlot *GetAnaPlot() { return (TRestAnalysisPlot*)GetApplication("TRestAnalysisPlot"); }

	TRestMetadata* GetApplicationWithName(string name);
	TRestMetadata* GetApplication(string type);

};



#endif

