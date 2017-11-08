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

#include "TObjArray.h"
using namespace std;


ClassImp(TRestAnalysisTree)

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
	if (from != NULL) {
		fNObservables = from->GetNumberOfObservables();
		fObservableValues.clear();
		fObservableNames.clear();
		fObservableDescriptions.clear();

		for (int i = 0; i < GetNumberOfObservables(); i++)
		{
			fObservableValues.push_back(new double(0));
			fObservableNames.push_back("old_" + from->GetObservableName(i));
			fObservableDescriptions.push_back(from->GetObservableDescription(i));
		}


		fConnected = true;
	}


}

Int_t TRestAnalysisTree::AddObservable(TString observableName, Double_t* observableValue, TString description)
{
	if (fBranchesCreated)
	{
		cout << "REST ERROR : Branches have been already created" << endl;
		cout << "No more observables can be added" << endl;
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

Int_t TRestAnalysisTree::FillEvent(TRestEvent *evt)
{
	fEventID = evt->GetID();
	fSubEventID = evt->GetSubID();
	fTimeStamp = evt->GetTimeStamp().AsDouble();
	*fSubEventTag = evt->GetSubEventTag();
	fRunOrigin = evt->GetRunOrigin();
	fSubRunOrigin = evt->GetSubRunOrigin();

	return this->Fill();
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

