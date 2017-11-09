#ifndef RestCore_TRestManager
#define RestCore_TRestManager

#include <iostream>

#include "TObject.h"

#include "TRestMetadata.h"
#include "TRestRun.h"
#include "TRestProcessRunner.h"
#include "TRestTask.h"


//in future I think we should write a graphical interface for TRestManager
class TRestManager : public TRestMetadata{

private:
	/// Pointer to TRestRun class.
	TRestRun *fRunInfo;
	
	TRestProcessRunner* fProcessRunner;

	vector<TRestTask*> fTasks;
	
public:
	/// Call CINT to generate streamers for this class
	ClassDef(TRestManager, 1);

	TRestManager();
	~TRestManager();

	void Initialize();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);

	void LaunchTasks();

	void PrintMetadata();

	TRestProcessRunner*GetProcessRunner() { return fProcessRunner; }
	TRestRun *GetRunInfo(){ return fRunInfo; }
};



#endif

