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
class TRestEventProcess;
class TRestMetadata;
class TStreamerElement;

class TRestAnalysisTree : public TTree {
   private:
#ifndef __CINT__
    Bool_t fConnected;        //!
    Bool_t fBranchesCreated;  //!

    Int_t fEventID;         //!
    Int_t fSubEventID;      //!
    Double_t fTimeStamp;    //!
    TString* fSubEventTag;  //!
    Int_t fRunOrigin;       //!
    Int_t fSubRunOrigin;    //!

    std::vector<char*> fObservableValues;  //!
#endif

    Int_t fNObservables;
    std::vector<TString> fObservableNames;
    std::vector<TString> fObservableDescriptions;
    std::vector<TString> fObservableTypes;

   protected:
   public:
    void Initialize();

    Int_t GetObservableID(TString obsName) {
        for (int i = 0; i < GetNumberOfObservables(); i++)
            if (fObservableNames[i] == obsName) return i;
        return -1;
    }

    Bool_t ObservableExists(TString obsName) {
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
    TString GetObservableName(Int_t n) {
        return fObservableNames[n];
    }  // TODO implement error message in case n >= fNObservables
    TString GetObservableDescription(Int_t n) { return fObservableDescriptions[n]; }
    Double_t GetObservableValue(Int_t n) {
        return *fObservableValues[n];
    }  // TODO implement error message in case n >= fNObservables
    Double_t GetObservableValue(TString obsName) {
        return this->GetObservableValue(this->GetObservableID(obsName));
    }
    TString GetObservableType(Int_t n) {
        if (fNObservables > 0 && fObservableTypes.size() == 0) return "double";
        return fObservableTypes[n];
    }

    template <class T>
    void SetObservableValue(Int_t n, T value) {
        *(T*)fObservableValues[n] = value;
    }
    template <class T>
    void SetObservableValue(TString ProcName_ObsName, T value) {
        // string name_fixed = Replace((string)ProcName_ObsName, ".", "_", 0);
        Int_t id = GetObservableID(ProcName_ObsName);
        if (id >= 0) SetObservableValue(id, value);
    }

    void CreateEventBranches();
    void CreateObservableBranches();

    void ConnectObservables();

    void CopyObservableList(TRestAnalysisTree* from, string prefix="");

    void ConnectEventBranches() {
        TBranch* br1 = GetBranch("eventID");
        br1->SetAddress(&fEventID);

        TBranch* br2 = GetBranch("subEventID");
        br2->SetAddress(&fSubEventID);

        TBranch* br3 = GetBranch("timeStamp");
        br3->SetAddress(&fTimeStamp);

        TBranch* br4 = GetBranch("subEventTag");
        br4->SetAddress(&fSubEventTag);

        TBranch* br5 = GetBranch("runOrigin");
        br5->SetAddress(&fRunOrigin);

        TBranch* br6 = GetBranch("subRunOrigin");
        br6->SetAddress(&fSubRunOrigin);
    }

    void PrintObservables(TRestEventProcess* proc = 0, int NObservables = 9999);
    void PrintObservable(int N);

    Int_t FillEvent(TRestEvent* evt);

    Int_t AddObservable(TString objName, TRestMetadata* meta, TString description = "");
    Int_t AddObservable(TString observableName, TString observableType = "double", TString description = "");
	template <typename T> 
	Int_t AddObservable(TString observableName, TString description = "") {
        return AddObservable(observableName, (TString)TRestTools::GetTypeName<T>(), description);
	}

    // Construtor
    TRestAnalysisTree();
    TRestAnalysisTree(TString name, TString title);
    // Destructor
    virtual ~TRestAnalysisTree();

    ClassDef(TRestAnalysisTree, 2);  // REST run class
};
#endif
