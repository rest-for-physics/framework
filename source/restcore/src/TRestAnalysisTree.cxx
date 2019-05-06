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
#include "TRestMetadata.h"

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
	fRunOrigin = 0;
	fSubRunOrigin = 0;
	fEventID = 0;
	fSubEventID = 0;
	fSubEventTag = new TString();
	fTimeStamp = 0;

	fNObservables = 0;

	fObservableDescriptions.clear();
	fObservableNames.clear();
	fObservableValues.clear();
	fConnected = false;
	fBranchesCreated = false;

}


void TRestAnalysisTree::ConnectObservables(TRestAnalysisTree * from)
{
	if (from != NULL && !fConnected) {

		vector<char*> tmpobsval;
		vector<TString> tmpobsname;
		vector<TString> tmpobsdes;
		vector<TString> tmptypes;

		for (int i = 0; i < from->GetNumberOfObservables(); i++)
		{
			tmpobsval.push_back(TRestTools::Assembly(from->GetObservableType(i)));
			tmpobsname.push_back("old_" + from->GetObservableName(i));
			tmpobsdes.push_back(from->GetObservableDescription(i));
			tmptypes.push_back(from->GetObservableType(i));
		}

		fObservableValues.insert(fObservableValues.begin(), tmpobsval.begin(), tmpobsval.end());
		fObservableNames.insert(fObservableNames.begin(), tmpobsname.begin(), tmpobsname.end());
		fObservableDescriptions.insert(fObservableDescriptions.begin(), tmpobsdes.begin(), tmpobsdes.end());
		fObservableTypes.insert(fObservableTypes.begin(), tmptypes.begin(), tmptypes.end());
		fNObservables += from->GetNumberOfObservables();

		fConnected = true;
	}
	else {
		cout << "REST ERROR: AnalysisTree Observables is already connected!" << endl;
	}

}

void TRestAnalysisTree::ConnectObservables()
{
	if (!fConnected) {
		for (int i = 0; i < GetNumberOfObservables(); i++)
		{
			double* x = new double(0);
			fObservableValues.push_back(TRestTools::Assembly(GetObservableType(i)));
		}

		for (int i = 0; i < GetNumberOfObservables(); i++)
		{
			TBranch*branch = GetBranch(fObservableNames[i]);
			if (branch != NULL)
				branch->SetAddress(fObservableValues[i]);
		}
		fConnected = true;
	}
	else {
		cout << "REST ERROR: AnalysisTree Observables is already connected!" << endl;
	}
}

Int_t TRestAnalysisTree::AddObservable(TString observableName, TString description) {
	if (fBranchesCreated)
	{
		return -1;
	}
	Double_t x = 0;
	if (GetObservableID(observableName) == -1) {
		fObservableNames.push_back(observableName);
		fObservableDescriptions.push_back(description);
		fObservableValues.push_back((char*)new double(0));
		fObservableTypes.push_back("double");

		fNObservables++;
	}
	else
	{
		//cout << "observable \"" << observableName << "\" has already been created! skipping" << endl;
		return GetObservableID(observableName);
	}

	return fNObservables - 1;
}

Int_t TRestAnalysisTree::AddObservable(TString objName, TRestMetadata* meta, TString description) {

	if (fBranchesCreated)
	{
		return -1;
	}
	TStreamerElement*ele = meta->GetDataMember((string)objName);
	TString brName = meta->GetName() + (TString)"." + ele->GetName();
	//cout << ele->GetTypeName() << " " << ele->GetName() << " " << ele->ClassName() << endl;
	if (GetObservableID(brName) == -1) {
		if (ele != NULL && !ele->IsaPointer() && ele->GetName()[0] != 'f') {
			fObservableNames.push_back(brName);
			fObservableDescriptions.push_back(description);
			fObservableValues.push_back(meta->GetDataMemberRef(ele));
			fObservableTypes.push_back(ele->GetTypeName());
			fNObservables++;
		}
		else
		{
			cout << "Data member \"" << objName << "\" not found in class: \"" << meta->ClassName() << "\"" << endl;
			return -1;
		}
	}
	else {
		//cout << "observable \"" << observableName << "\" has already been created! skipping" << endl;
		return GetObservableID(brName);
	}
	return fNObservables - 1;
}

void TRestAnalysisTree::PrintObservables(TRestEventProcess* proc, int NObs)
{
	if (!isConnected() || !fBranchesCreated)
	{
		if (fNObservables > 0 && fObservableValues.size() == 0) //the object may be just retrieved from root file
		{
			ConnectEventBranches();
			ConnectObservables();
		}
	}

	cout.precision(15);
	if (proc == NULL) {
		std::cout << "Run origin : " << GetRunOrigin() << std::endl;
		std::cout << "Event ID : " << GetEventID() << std::endl;
		std::cout << "Event Time : " << GetTimeStamp() << std::endl;
		std::cout << "Event Tag : " << GetSubEventTag() << std::endl;
		std::cout << "-----------------------------------------" << std::endl;
	}
	else {
		std::cout << "---- AnalysisTree Observable for process: " << proc->GetName() << " ----" << std::endl;
	}

	for (int n = 0; n < GetNumberOfObservables() && n < NObs; n++) {
		if (proc == NULL || (proc != NULL && ((string)fObservableNames[n]).find(proc->GetName()) == 0))
			PrintObservable(n);
	}

	std::cout << std::endl;
	cout.precision(6);
}

//print the Nth observable in the observal list
void TRestAnalysisTree::PrintObservable(int n) {
	if (n < 0 || n >= fNObservables) { return; }
	if (isConnected() || fBranchesCreated) {
		if (GetObservableType(n) == "double") {
			std::cout << "Observable : " << ToString(fObservableNames[n], 25) << "    Value : " << *(double*)fObservableValues[n] << std::endl;
		}
		else {
			std::cout << "Observable : " << ToString(fObservableNames[n], 25) << "    (" << fObservableTypes[n] << ")" << std::endl;
		}
	}
	else {
		std::cout << "Observable : " << ToString(fObservableNames[n], 25) << "    Value : ???" << std::endl;
	}
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
	{

		TString typeName = fObservableTypes[n];
		TString brName = fObservableNames[n];
		char* ref = fObservableValues[n];

		if (typeName == "double") {
			this->Branch(brName, (double*)ref);
		}
		else if (typeName == "float") {
			this->Branch(brName, (float*)ref);
		}
		else if (typeName == "long double") {
			this->Branch(brName, (long double*)ref);
		}
		else if (typeName == "bool") {
			this->Branch(brName, (bool*)ref);
		}
		else if (typeName == "char") {
			this->Branch(brName, (char*)ref);
		}
		else if (typeName == "int") {
			this->Branch(brName, (int*)ref);
		}
		else if (typeName == "short") {
			this->Branch(brName, (short*)ref);
		}
		else if (typeName == "long") {
			this->Branch(brName, (long*)ref);
		}
		else if (typeName == "long long") {
			this->Branch(brName, (long long*)ref);
		}
		else {
			this->Branch(brName, typeName, ref);
		}
	}

	//Branch(fObservableNames[n], fObservableValues[n]);

	fBranchesCreated = true;
}



//______________________________________________________________________________
TRestAnalysisTree::~TRestAnalysisTree()
{
}

