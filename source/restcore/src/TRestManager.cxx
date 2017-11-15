#include "TRestManager.h"


ClassImp(TRestManager);


TRestManager::TRestManager()
{
	Initialize();
}


TRestManager::~TRestManager()
{
}

///////////////////////////////////////////////
/// \brief Set the class name as section name during initialization.
///
void TRestManager::Initialize()
{
	SetSectionName(this->ClassName());

	fMetaObjects.clear();
}

///////////////////////////////////////////////
/// \brief Respond to the input xml element.
///
/// If the declaration of the input element is:
/// 1. TRestRun: Startup the TRestRun class with this xml element.
/// 2. TRestAnalysisPlot: Startup the TRestAnalysisPlot class with this xml element.
/// 3. addTask: Add the name of this element into the task list, ready for the method
/// Launch() to use. If the field value is not "on", the task will be omitted.
///
/// Other types of declarations will be omitted.
///
Int_t TRestManager::ReadConfig(string keydeclare, TiXmlElement* e)
{
	//if (keydeclare == "TRestRun") {
	//	TRestRun* fRunInfo = new TRestRun();
	//	fRunInfo->SetHostmgr(this);
	//	fRunInfo->LoadConfigFromFile(e, fElementGlobal);
	//	fMetaObjects.push_back(fRunInfo);
	//	gROOT->Add(fRunInfo);
	//	return 0;
	//}

	//else if (keydeclare == "TRestProcessRunner") {
	//	TRestProcessRunner* fProcessRunner = new TRestProcessRunner();
	//	fProcessRunner->SetHostmgr(this);
	//	fProcessRunner->LoadConfigFromFile(e, fElementGlobal);
	//	fMetaObjects.push_back(fProcessRunner);
	//	gROOT->Add(fProcessRunner);
	//	return 0;
	//}

	if(Count(keydeclare,"TRest")>0)
	{
		TClass*c = TClass::GetClass(keydeclare.c_str());
		if (c == NULL) {
			cout << " " << endl;
			cout << "REST ERROR. Class : " << keydeclare << " not found!!" << endl;
			cout << "This class will be skipped." << endl;
			return -1;
		}
		TRestMetadata*meta = (TRestMetadata*)c->New();
		meta->SetHostmgr(this);
		meta->LoadConfigFromFile(e, fElementGlobal);
		fMetaObjects.push_back(meta);
		gROOT->Add(meta);
		return 0;
	}

	else if (keydeclare == "addTask") {
		string active = GetParameter("value", e, "");
		if (active != "ON" && active != "On" && active != "on") {
			debug << "skipping task... " << endl;
			return 0;
		}
		debug << "Loading Task..." << endl;

		const char* type = e->Attribute("type");
		const char* cmd = e->Attribute("command");
		if (type == NULL && cmd == NULL) {
			cout << "command or type should be given!" << endl;
			return -1;
		}
		if (type != NULL) {
			TClass*c = TClass::GetClass(type);
			if (c == NULL) {
				cout << " " << endl;
				cout << "REST ERROR. Task : " << type << " not found!!" << endl;
				cout << "This process will be skipped." << endl;
				return -1;
			}
			TRestTask*tsk = (TRestTask*)c->New();
			tsk->LoadConfigFromFile(e, fElementGlobal);
			tsk->InitTask();
			tsk->RunTask(this);
		}
		else if (cmd != NULL) {
			
			gROOT->ProcessLine(cmd);
		}
	}

	return -1;
}


TRestMetadata* TRestManager::GetMetadataInfo(string type)
{
	for (int i = 0; i < fMetaObjects.size(); i++)
	{
		if (fMetaObjects[i]->ClassName() == type)
		{
			return fMetaObjects[i];
		}
	}
	return NULL;
}


///////////////////////////////////////////////
/// \brief PrintMetadata of this class
///
/// Not implemented.
///
void TRestManager::PrintMetadata()
{
	return; 
}


