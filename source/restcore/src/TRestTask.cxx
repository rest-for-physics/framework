#include "TRestTask.h"
ClassImp(TRestTask);



void TRestTask::BeginOfInit()
{
	int n = GetNumberOfDataMember();
	for (int i = 1; i < n; i++) {
		TStreamerElement* e =GetDataMemberWithID(i);
		SetDataMemberValFromConfig(e);

	}

}