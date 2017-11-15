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

//in future I think we should write a graphical interface for TRestManager
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
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);

	//void LaunchTasks();

	void PrintMetadata();

	TRestProcessRunner*GetProcessRunner() { return (TRestProcessRunner*)GetMetadataClass("TRestProcessRunner"); }
	TRestRun *GetRunInfo() { return (TRestRun*)GetMetadataClass("TRestRun"); }
	TRestRun *GetAnaPlot() { return (TRestRun*)GetMetadataClass("TRestAnalysisPlot"); }

	TRestMetadata* GetMetadataInfo(string type);
	TRestMetadata* GetMetadataClass(string type) { return GetMetadataInfo(type); }

};



#endif

