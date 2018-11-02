#ifndef RestCore_TRestManager
#define RestCore_TRestManager

#include <iostream>

#include "TObject.h"

#include "TRestMetadata.h"
#include "TRestRun.h"
#include "TRestProcessRunner.h"
#include "TRestAnalysisPlot.h"
#include "TApplication.h"

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
	void InitFromConfigFile() {
		if (fElement != NULL)
		{
			TiXmlElement*e = fElement->FirstChildElement();
			while (e != NULL)
			{
				string value = e->Value();
				if (value == "variable" || value == "myParameter" || value == "constant") { e = e->NextSiblingElement(); continue; }
				ReadConfig((string)e->Value(), e);
				e = e->NextSiblingElement();
			}
		}
	}
	int LoadSectionMetadata();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);

	void InitFromTask(string taskName, vector<string> arguments);

	//void LaunchTasks();

	void PrintMetadata();

	TRestProcessRunner*GetProcessRunner() { return (TRestProcessRunner*)GetApplication("TRestProcessRunner"); }
	TRestRun *GetRunInfo() { return (TRestRun*)GetApplication("TRestRun"); }
	TRestAnalysisPlot *GetAnaPlot() { return (TRestAnalysisPlot*)GetApplication("TRestAnalysisPlot"); }

	TRestMetadata* GetApplicationWithName(string name);
	TRestMetadata* GetApplication(string type);

};



#endif

