//////////////////////////////////////////////////////////////////////////
///
/// REST macros are actually ROOT scripts written in C++. TRestTask wraps
/// them and turns them into application mateadata class. This is done by 
/// calling method TInterpreter::LoadFile() and TInterpreter::ProcessLine().
/// Turning REST-macro-defined functions into classes means that we
/// are able to run the macros from rml file, by adding a section under the 
/// section "TRestManager". Another way to launch TRestTask is through 
/// restManager executable. By directly typing "restManager TASKNAME [ARG]"
/// in bash we can run the macros like executables. We also allow users to 
/// write TRestTask inherted class in REST macros. This can enable more functionalities.
///
/// \class TRestTask
///
///--------------------------------------------------------------------------
/// 
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///            code (REST v2)
///            Igor G. Irastorza
/// 
/// 2017-Aug:  Major change to xml reading and class startup procedure
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////


#include "TRestTask.h"
ClassImp(TRestTask);

///////////////////////////////////////////////
/// \brief TRestTask default constructor
///
TRestTask::TRestTask() {
	Initialize();
	fNRequiredArgument = 0;
}

///////////////////////////////////////////////
/// \brief TRestTask constructor with macro file name
///
/// The first method in the file is identified with its name and require
/// arguments saved in the class. They will be used in forming the command 
/// line in the method TRestTask::InitTask()
TRestTask::TRestTask(TString MacroFileName)
{
	Initialize();
	fNRequiredArgument = 0;
	ifstream in(MacroFileName);
	if (!in.is_open()) {
		cout << "Error opening file" << endl;
		exit(1);
	}
	char buffer[256];
	while (!in.eof()) {
		in.getline(buffer, 256);
		string line = buffer;

		if (line.find('(') != -1 && line.find(' ') > 0) {
			//this line is a definition of the macro method
			methodname = line.substr(line.find(' ') + 1, line.find('(') - line.find(' ') - 1);
			SetName(methodname.c_str());

			while (line.find(')') == -1) {
				if (in.eof()) {
					error << "invaild macro file!" << endl;
					exit(1);
				}
				in.getline(buffer, 256);
				line = line + (string)buffer;
			}

			string args = line.substr(line.find('(') + 1, line.find(')') - line.find('(') - 1);
			auto list = Spilt(args, ",");
			argumentname.clear();
			argumenttype.clear();
			for (int i = 0; i < list.size(); i++)
			{
				auto tmp = Spilt(list[i], " ");
				if (Count(tmp[0], "TString") > 0)
				{
					argumenttype.push_back(65);
				}
				else if (Count(ToUpper(tmp[0]), "INT") > 0)
				{
					argumenttype.push_back(3);
				}
				else if (Count(ToUpper(tmp[0]), "DOUBLE") > 0)
				{
					argumenttype.push_back(8);
				}
				else
				{
					argumenttype.push_back(-1);
				}

				argumentname.push_back(Spilt(tmp[1], "=")[0]);
				if (Spilt(list[i], "=").size() > 1) {
					argument.push_back(Spilt(list[i], "=")[1]);
				}
				else
				{
					argument.push_back("NOT SET");
					fNRequiredArgument++;
				}

			}

			break;
		}

	}
}

///////////////////////////////////////////////
/// \brief Starter method. Looks through the rml sections and set argument/datamenber value
///
void TRestTask::InitFromConfigFile()
{
	if ((string)this->ClassName() == (string)"TRestTask")
	{
		TiXmlElement*ele = fElement->FirstChildElement("parameter");
		while (ele != NULL) {
			if (ele->Attribute("name") == NULL || ele->Attribute("value") == NULL)
				continue;
			SetArgumentValue(ele->Attribute("name"), ele->Attribute("value"));
			ele = ele->NextSiblingElement("parameter");
		}

	}
	else
	{
		int n = GetNumberOfDataMember();
		for (int i = 1; i < n; i++) {
			TStreamerElement* e = GetDataMemberWithID(i);
			SetDataMemberValFromConfig(e);
		}
	}
	ConstructCommand();
}

///////////////////////////////////////////////
/// \brief Set argument value with name and value. 
///
void TRestTask::SetArgumentValue(string name, string value)
{
	for (int i = 0; i < argumentname.size(); i++) {
		if (name == argumentname[i]) {
			argument[i] = name;
		}
	}
}

///////////////////////////////////////////////
/// \brief Set argument directly with a list of string
///
/// Argument list will be overwritten by the input list. It will also set the
/// data member value for derived class
void TRestTask::SetArgumentValue(vector<string>arg)
{
	if (arg.size() < fNRequiredArgument) {
		PrintHelp();
		exit(0);
	}
	argument = arg;
	if (this->ClassName() != (string)"TRestTask")
	{
		int n = GetNumberOfDataMember();
		for (int i = 1; (i < argument.size() + 1 && i < n); i++)
		{
			TStreamerElement* e = GetDataMemberWithID(i);
			SetDataMemberVal(e, argument[i - 1]);
			debug << "data member " << e->GetName() << " has been set to " << GetDataMemberValString(e);
		}

	}
}

///////////////////////////////////////////////
/// \brief Static method to instantiate a TRestTask object with "MacroName"
///
/// REST macros are saved in the directory ./macros. They follow a naming 
/// logic, the naming style is like following:
/// REST_[MacroName].hh
/// 
/// e.g. REST_ViewEvents.hh. Here we must add a prefix "REST_" with a macro name
/// usually in verb form. When given the macro name, this method first calls
/// TClass::GetClass() to find if there is a TRestTask-inherted class which has
/// this name. If so, it returns the found class, if not, it finds a corresponding
/// macro file and calls gInterpreter to load it, and then instaintiates a 
/// TRestTask class wrapping this file.
TRestTask* TRestTask::GetTask(TString MacroName)
{
	TClass*c = TClass::GetClass(MacroName);
	if (c == NULL) 
		c = TClass::GetClass("REST_" + MacroName);
	
	if(c==NULL){
		string macfilelists = ExecuteShellCommand("find $REST_PATH/macros -name *" + (string)MacroName + (string)".*");
		auto macfiles = Spilt(macfilelists, "\n");
		//string command = (string)"find $REST_PATH/macros -name *" + (string)MacroName + (string)"* > /tmp/macros.list";
		//system(command.c_str());
		//FILE *f = fopen("/tmp/macros.list", "r");
		//char str[256];
		//fscanf(f, "%s\n", str);
		//if (feof(f) == 0)
		//{
		//	cout << "REST Warning : multi matching of macro \"" << MacroName << "\" found!" << endl;
		//}
		//fclose(f);
		//system("rm /tmp/macros.list");
		if (macfiles.size()==0|| macfiles[0]=="")
		{
			return NULL;
		}
		cout << "Found MacroFile " << macfiles[0] << endl;
		system("echo \"#define REST_MANAGER\" > /tmp/tmpMacro.c");
		system(("cat " + macfiles[0] + " >> /tmp/tmpMacro.c").c_str());
		if (gInterpreter->LoadFile("/tmp/tmpMacro.c") != 0)
		{
			return NULL;
		}

		auto tsk = new TRestTask(macfiles[0].c_str());
		system("rm /tmp/tmpMacro.c");
		return tsk;
	}
	else if(c->InheritsFrom("TRestTask"))
	{
		return (TRestTask * )c->New();
	}
	return NULL;
}

///////////////////////////////////////////////
/// \brief Forms the command string
///
void TRestTask::ConstructCommand() {

	for (int i = 0; i < argument.size(); i++) {
		if (argument[i] == "NOT SET") {
			error << "TRestTask : argument " << i << " not set! Task will not run!" << endl;
		}
	}

	if ((string)this->ClassName() == (string)"TRestTask") //this class
	{
		string methodname = GetName();
		cmdstr = methodname + "(";
		for (int i = 0; i < argument.size(); i++)
		{
			if (argumenttype[i] == 65) {
				cmdstr += "\"" + argument[i] + "\"";
			}
			else
			{
				cmdstr += argument[i];
			}

			if (i < argument.size() - 1)
				cmdstr += ",";
		}
		cmdstr += ")";

		cout << "Command : " << cmdstr << endl;
	}

}



///////////////////////////////////////////////
/// \brief Run the task with command line
///
void TRestTask::RunTask(TRestManager*a)
{
	if (a == NULL && cmdstr != "") {
		gInterpreter->ProcessLine(cmdstr);
	}
}

///////////////////////////////////////////////
/// \brief Defalut helper method both for TRestTask and any TRestTask-inherted class
///
void TRestTask::PrintHelp()
{
	if ((string)this->ClassName() == (string)"TRestTask")
	{
		error << GetName() << "() Gets invailed input!" << endl;
		error << "You should give the following arguments ( * : necessary input):" << endl;
		int n = argument.size();
		for (int i = 0; i < n; i++) {
			if (i < fNRequiredArgument)
				error << "*";
			error << argumentname[i] << endl;
		}
	}
	else
	{
		error << "Macro class \"" << this->ClassName() << "\" gets invailed input!" << endl;
		error << "You should give the following arguments ( * : necessary input):" << endl;
		int n = GetNumberOfDataMember();
		for (int i = 1; i < n; i++) {
			if (i < fNRequiredArgument + 1)
				error << "*";
			error << GetDataMemberWithID(i)->GetName() << endl;
		}
	}

}