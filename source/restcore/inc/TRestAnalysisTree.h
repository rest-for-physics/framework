///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisTree.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             mar 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestAnalysisTree
#define RestCore_TRestAnalysisTree

#include "TRestEvent.h"

#include "TTree.h"
class TRestBranch :public TBranch {
public:
	void SetBranchAddressQuick(char* addr) {
		fAddress = addr;
	}
};

class TRestAnalysisTree :public TTree {

private:

#ifndef __CINT__
	Bool_t fConnected;//!
	Bool_t fBranchesCreated;//!

	Int_t fEventID;//!
	Int_t fSubEventID;//!
	Double_t fTimeStamp;//!
	TString *fSubEventTag;//!
	Int_t fRunOrigin;//!
	Int_t fSubRunOrigin;//!

	std::vector <Double_t*> fObservableValues;//!
#endif

	Int_t fNObservables;
	std::vector <TString> fObservableNames;
	std::vector <TString> fObservableDescriptions;

protected:
public:
	void Initialize();

	Int_t GetObservableID(TString obsName)
	{
		for (int i = 0; i < GetNumberOfObservables(); i++)
			if (fObservableNames[i] == obsName) return i;
		return -1;
	}

	Bool_t ObservableExists(TString obsName)
	{
		if (GetObservableID(obsName) >= 0) return true;
		return false;
	}

	Bool_t isConnected() { return fConnected; }

	Int_t GetEventID() { return fEventID; }
	Int_t GetSubEventID() { return fSubEventID; }
	Double_t GetTimeStamp() { return fTimeStamp; }
	TString GetSubEventTag() { return *fSubEventTag; }
	Int_t GetRunOrigin() { return fRunOrigin; }
	Int_t GetSubRunOrigin() { return fSubRunOrigin; }

	Int_t GetNumberOfObservables() { return fNObservables; }
	TString GetObservableName(Int_t n) { return fObservableNames[n]; } // TODO implement error message in case n >= fNObservables
	TString GetObservableDescription(Int_t n) { return fObservableDescriptions[n]; }
	Double_t GetObservableValue(Int_t n) { return *fObservableValues[n]; } // TODO implement error message in case n >= fNObservables

	Double_t GetObservableValue(TString obsName) { return this->GetObservableValue(this->GetObservableID(obsName)); }

	void SetObservableValue(Int_t n, Double_t value) { *fObservableValues[n] = value; }

	void SetObservableValue(TString obsName, Double_t value)
	{
		Int_t id = GetObservableID(obsName);
		if (id >= 0) SetObservableValue(id, value);
	}

	void CreateEventBranches();
	void CreateObservableBranches();

	void ConnectObservables(TRestAnalysisTree * from);

	void ConnectEventBranches()
	{
		TBranch *br1 = GetBranch("eventID");
		br1->SetAddress(&fEventID);

		TBranch *br2 = GetBranch("subEventID");
		br2->SetAddress(&fSubEventID);

		TBranch *br3 = GetBranch("timeStamp");
		br3->SetAddress(&fTimeStamp);

		TBranch *br4 = GetBranch("subEventTag");
		br4->SetAddress(&fSubEventTag);

		TBranch *br5 = GetBranch("runOrigin");
		br5->SetAddress(&fRunOrigin);

		TBranch *br6 = GetBranch("subRunOrigin");
		br6->SetAddress(&fSubRunOrigin);
	}

	void ConnectObservables()
	{
		for (int i = 0; i < GetNumberOfObservables(); i++)
		{
			Double_t x = 0;
			fObservableValues.push_back(&x);
		}

		for (int i = 0; i < GetNumberOfObservables(); i++)
		{
			TBranch*branch = GetBranch(fObservableNames[i]);
			branch->SetAddress(fObservableValues[i]);
		}
		fConnected = true;

	}

	void PrintObservables()
	{
		std::cout << "Run origin : " << GetRunOrigin() << std::endl;
		std::cout << "Sub run origin : " << GetSubRunOrigin() << std::endl;
		std::cout << "Event ID : " << GetEventID() << std::endl;
		std::cout << "Event Time : " << GetTimeStamp() << std::endl;
		std::cout << "-----------------------------------------" << std::endl;
		for (int n = 0; n < GetNumberOfObservables(); n++)
			std::cout << "Observable Name : " << fObservableNames[n] << "    Value : " << fObservableValues[n] << std::endl;
		std::cout << std::endl;

	}

	Int_t FillEvent(TRestEvent *evt);

	Int_t AddObservable(TString observableName, Double_t* observableValue, TString description = "");

	//Construtor
	TRestAnalysisTree();
	TRestAnalysisTree(TString name, TString title);
	//Destructor
	virtual ~TRestAnalysisTree();


	ClassDef(TRestAnalysisTree, 1);     // REST run class
};
#endif
