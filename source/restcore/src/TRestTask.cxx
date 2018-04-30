#include "TRestTask.h"
ClassImp(TRestTask);

TRestTask::TRestTask() {
	Initialize();
	fNRequiredArgument = 0;
}

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
					argument.push_back("");
					fNRequiredArgument++;
				}

			}

			break;
		}

	}
}


void TRestTask::BeginOfInit()
{
	if (this->ClassName() == (string)"TRestTask")
	{
	}
	else
	{
		int n = GetNumberOfDataMember();
		for (int i = 1; i < n; i++) {
			TStreamerElement* e = GetDataMemberWithID(i);
			SetDataMemberValFromConfig(e);
		}
	}
}

void TRestTask::SetArgumentValue(string name, string value)
{
	for (int i = 0; i < argumentname.size(); i++) {
		if (name == argumentname[i]) {
			argument[i] = name;
		}
	}
}


TRestTask* TRestTask::GetTask(TString MacroName)
{
	TClass*c = TClass::GetClass(MacroName);
	if (c == NULL) 
		c = TClass::GetClass("REST_" + MacroName);
	
	if(c==NULL){
		string macfilelists = ExecuteShellCommand("find $REST_PATH/macros -name *" + (string)MacroName + (string)"*");
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
		if (gInterpreter->LoadFile(macfiles[0].c_str()) != 0)
		{
			return NULL;
		}

		return new TRestTask(macfiles[0].c_str());
	}
	else if(c->InheritsFrom("TRestTask"))
	{
		return (TRestTask * )c->New();
	}
	return NULL;
}

bool TRestTask::InitTask(vector<string>arg)
{
	if (arg.size() < fNRequiredArgument) {
		PrintHelp();
		exit(0);
	}
	argument = arg;
	if (this->ClassName() == (string)"TRestTask") //this class
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
	else//call from inherted class
	{
		int n = GetNumberOfDataMember();
		for (int i = 1; (i < argument.size() + 1 && i < n); i++)
		{
			TStreamerElement* e = GetDataMemberWithID(i);
			SetDataMemberVal(e, argument[i - 1]);
			debug << "data member " << e->GetName() << " has been set to " << GetDataMemberValString(e);
		}

	}

	return true;
}


void TRestTask::RunTask(TRestManager*a)
{
	if (a == NULL && cmdstr != "") {
		gInterpreter->ProcessLine(cmdstr);
	}
}


void TRestTask::PrintHelp()
{
	if (this->ClassName() == (string)"TRestTask")
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