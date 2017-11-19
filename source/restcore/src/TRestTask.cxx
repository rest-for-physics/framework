#include "TRestTask.h"
ClassImp(TRestTask);

TRestTask::TRestTask() {
	Initialize();
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
	for (int i = 1; (i < argument.size()+1 && i < n); i++)
	{
		TStreamerElement* e = GetDataMemberWithID(i);
		SetDataMemberVal(e,argument[i]);
		debug<<"data member "<<e->GetName()<< " has been set to " << GetDataMemberValString(e);
	}
}