#include "TRestTask.h"
ClassImp(TRestTask);

TRestTask::TRestTask() {
	Initialize();
	fNRequiredArgument = 0;
}

void TRestTask::BeginOfInit()
{
	int n = GetNumberOfDataMember();
	for (int i = 1; i < n; i++) {
		TStreamerElement* e =GetDataMemberWithID(i);
		SetDataMemberValFromConfig(e);

	}

}

void TRestTask::InitTask(vector<string>argument)
{
	int n = GetNumberOfDataMember();
	if (argument.size() < fNRequiredArgument) {
		PrintHelp();
		exit(0);
	}
	else
	{
		for (int i = 1; (i < argument.size() + 1 && i < n); i++)
		{
			TStreamerElement* e = GetDataMemberWithID(i);
			SetDataMemberVal(e, argument[i-1]);
			debug << "data member " << e->GetName() << " has been set to " << GetDataMemberValString(e);
		}
	}

}

void TRestTask::PrintHelp() 
{
	error << this->ClassName() << " Gets invailed number of input argument!" << endl;
	error << "You should give the following argument :" << endl;
	int n = GetNumberOfDataMember();
	for (int i = 1; (i < fNRequiredArgument + 1 && i < n); i++){
		error << GetDataMemberWithID(i)->GetName() << endl;
	}
}