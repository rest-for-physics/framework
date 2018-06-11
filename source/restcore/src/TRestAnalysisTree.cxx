///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisTree.cxx
///
///             Base class for managing a basic analysis tree. 
///
///             feb 2016:   First concept
///              author :   Javier Galan
///_______________________________________________________________________________


#include "TRestAnalysisTree.h"
#include "TRestEventProcess.h"

#include "TObjArray.h"


using namespace std;


ClassImp(TRestAnalysisTree)
using namespace std;
//______________________________________________________________________________
TRestAnalysisTree::TRestAnalysisTree() : TTree()
{
	SetName("TRestAnalysisTree");
	SetTitle("Analysis tree");

	Initialize();
}

TRestAnalysisTree::TRestAnalysisTree(TString name, TString title) : TTree(name, title)
{
	SetName(name);
	SetTitle(title);

	Initialize();
}

void TRestAnalysisTree::Initialize()
{

	fSubEventTag = new TString();

	fNObservables = 0;

	fConnected = false;
	fBranchesCreated = false;

}


void TRestAnalysisTree::ConnectObservables(TRestAnalysisTree * from)
{
	if (from != NULL&&!fConnected) {

		vector<double*> tmpobsval;
		vector<TString> tmpobsname;
		vector<TString> tmpobsdes;

		for (int i = 0; i < from->GetNumberOfObservables(); i++)
		{
			tmpobsval.push_back(new double(0));
			tmpobsname.push_back("old_" + from->GetObservableName(i));
			tmpobsdes.push_back(from->GetObservableDescription(i));
		}

		fObservableValues.insert(fObservableValues.begin(),tmpobsval.begin(),tmpobsval.end());
		fObservableNames.insert(fObservableNames.begin(), tmpobsname.begin(), tmpobsname.end());
		fObservableDescriptions.insert(fObservableDescriptions.begin(), tmpobsdes.begin(), tmpobsdes.end());
		fNObservables += from->GetNumberOfObservables();

		fConnected = true;
	}


}

Int_t TRestAnalysisTree::AddObservable(TString observableName, Double_t* observableValue, TString description)
{
	if (fBranchesCreated)
	{
		return -1;
	}
	Double_t x = 0;
	if (GetObservableID(observableName) == -1) {
		fObservableNames.push_back(observableName);
		fObservableDescriptions.push_back(description);
		fObservableValues.push_back(observableValue);

		fNObservables++;
	}
	else
	{
		cout << "observable \"" << observableName << "\" has already been created! skipping" << endl;
		return -1;
	}

	return fNObservables - 1;
}

void TRestAnalysisTree::PrintObservables()
{
	cout.precision(15);
	std::cout << "Run origin : " << GetRunOrigin() << std::endl;
	std::cout << "Event ID : " << GetEventID() << std::endl;
	std::cout << "Event Time : " << GetTimeStamp() << std::endl;
	std::cout << "Event Tag : " << GetSubEventTag() << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	if (isConnected()||fBranchesCreated) {
		for (int n = 0; n < GetNumberOfObservables(); n++)
			std::cout << "Observable Name : " << fObservableNames[n] << "    Value : " << *fObservableValues[n] << std::endl;
		std::cout << std::endl;
	}
	else
	{
		for (int n = 0; n < GetNumberOfObservables(); n++)
			std::cout << "Observable Name : " << fObservableNames[n] << "    Value : ..." << std::endl;
		std::cout << std::endl;
	}
	cout.precision(6);
}


Int_t TRestAnalysisTree::FillEvent(TRestEvent *evt)
{
	fEventID = evt->GetID();
	fSubEventID = evt->GetSubID();
	fTimeStamp = evt->GetTimeStamp().AsDouble();
	*fSubEventTag = evt->GetSubEventTag();
	fRunOrigin = evt->GetRunOrigin();
	fSubRunOrigin = evt->GetSubRunOrigin();

	//return this->Fill();
	return 0;
}

void TRestAnalysisTree::SetObservableValue(TString ProcName_ObsName, Double_t value)
{
	Int_t id = GetObservableID(ProcName_ObsName);
	if (id >= 0) SetObservableValue(id, value);
}

void TRestAnalysisTree::SetObservableValue(TRestEventProcess* proc, TString obsName, Double_t value) 
{
	TString name = proc->GetName() + (TString)"_" + obsName;
	SetObservableValue(name, value);
}

void TRestAnalysisTree::CreateEventBranches()
{
	Branch("runOrigin", &fRunOrigin);
	Branch("subRunOrigin", &fSubRunOrigin);
	Branch("eventID", &fEventID);
	Branch("subEventID", &fSubEventID);
	Branch("timeStamp", &fTimeStamp);
	Branch("subEventTag", fSubEventTag);
}

void TRestAnalysisTree::CreateObservableBranches()
{
	if (fBranchesCreated)
	{
		cout << "REST ERROR : Branches have been already created" << endl;
		exit(1);
	}

	for (int n = 0; n < GetNumberOfObservables(); n++)
		Branch(fObservableNames[n], fObservableValues[n]);

	fBranchesCreated = true;
}



//______________________________________________________________________________
TRestAnalysisTree::~TRestAnalysisTree()
{
}

