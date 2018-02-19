#include "TRestManager.h"

#include "TInterpreter.h"
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

	fMetaObjects.push_back(this);

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

	//cout << "----------- " << gROOT->FindObject("SJTU_Proto") << endl;

	if (Count(keydeclare, "TRest") > 0)
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

		return 0;
	}

	else if (keydeclare == "addTask") {
		string active = GetParameter("value", e, "");
		if (active != "ON" && active != "On" && active != "on") {
			warning << "skipping task... " << endl;
			return 0;
		}
		debug << "Loading Task...";

		const char* type = e->Attribute("type");
		const char* cmd = e->Attribute("command");
		if (type == NULL && cmd == NULL) {
			warning << "command or type should be given!" << endl;
			return -1;
		}
		if (type != NULL) {
			debug << " \"" << type << "\" " << endl;
			if (type == "processEvents")
			{
				auto pr = GetProcessRunner();
				if (pr != NULL)
					pr->RunProcess();
			}
			else if (type == "analysisPlot")
			{
				auto ap = GetAnaPlot();
				if (ap != NULL)
					ap->PlotCombinedCanvas();
			}
			else if (type == "saveMetadata")
			{
				auto ri = GetRunInfo();
				if (ri != NULL) {
					ri->FormOutputFile();
					ri->CloseFile();
				}
			}
			else
			{
				TClass*c = TClass::GetClass(type);
				if (c == NULL) {
					c = TClass::GetClass(("REST_" + (string)type).c_str());
					if (c == NULL) {
						warning << " " << endl;
						warning << "REST ERROR. Metadata Task : " << type << " not found!!" << endl;
						warning << "This task will be skipped." << endl;
						return -1;
					}

				}
				TRestTask*tsk = (TRestTask*)c->New();
				tsk->LoadConfigFromFile(e, fElementGlobal);

				tsk->RunTask(this);
				return 0;
			}
		}
		else if (cmd != NULL) {
			debug << " \"" << cmd << "\" " << endl;

			string name;
			string call;
			string method;
			string arg;
			if (Spilt(cmd, "->").size() != 2) {
				if (Spilt(cmd, ".").size() != 2) {
					warning << "command" << " \"" << cmd << "\" " << "is illegal!" << endl;
					return -1;
				}
				else
				{
					name = Spilt(cmd, ".")[0];
					call = Spilt(cmd, ".")[1];
				}
			}
			else
			{
				name = Spilt(cmd, "->")[0];
				call = Spilt(cmd, "->")[1];
			}
			if (Count(call, "(") != 1 || Count(call, ")") != 1) //we can only use one bracket
			{
				warning << "command" << " \"" << cmd << "\" " << "is illegal!" << endl;
				return -1;
			}
			method = Spilt(call, "(")[0];
			arg = Spilt(Spilt(call, "(")[1], ")").size() == 0 ? "" : Spilt(Spilt(call, "(")[1], ")")[0];

			for (int i = 0; i < fMetaObjects.size(); i++)
			{
				if (fMetaObjects[i]->GetName() == name) {
					debug << "processing..." << endl;
					gInterpreter->Execute(fMetaObjects[i], fMetaObjects[i]->IsA(), method.c_str(), arg.c_str());
					//((TRestProcessRunner*)fMetaObjects[i])->RunProcess();
					break;
				}
				else if (i == fMetaObjects.size() - 1)
				{
					warning << "Object \"" << name << "\" " << " is not defined in current scope!" << endl;
					return -1;
				}
			}








			return 0;
		}
	}



	return -1;
}


TRestMetadata* TRestManager::GetApplication(string type)
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

TRestMetadata* TRestManager::GetApplicationWithName(string name)
{
	for (int i = 0; i < fMetaObjects.size(); i++)
	{
		if (fMetaObjects[i]->GetName() == name)
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


