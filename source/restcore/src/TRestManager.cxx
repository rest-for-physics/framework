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
	fRunInfo = new TRestRun();
	fProcessRunner = new TRestProcessRunner();
	fRunInfo->SetHostmgr(this);
	fProcessRunner->SetHostmgr(this);
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
	if (keydeclare == "TRestRun") {

		fRunInfo->LoadConfigFromFile(e, fElementGlobal);

		return 0;
	}

	if (keydeclare == "TRestProcessRunner") {

		fProcessRunner->LoadConfigFromFile(e, fElementGlobal);

		return 0;
	}

	if (keydeclare == "addTask") {
		string active = GetParameter("value", e, "");
		if (active != "ON" && active != "On" && active != "on") return 0;

		const char* type = e->Attribute("type");
		if (type == NULL) {
			cout << "Task type is not given in the addTask section!" << endl;
			return -1;
		}
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
		fTasks.push_back(tsk);
	}


	return -1;
}


///////////////////////////////////////////////
/// \brief Launch all the tasks in the task list in sequence
///
void TRestManager::RunTasks() 
{
	for (int i = 0; i < fTasks.size(); i++) {
		fTasks[i]->RunTask(this);
	}
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


