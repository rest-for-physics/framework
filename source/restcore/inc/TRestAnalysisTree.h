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
#include "TRestReflector.h"

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

    std::vector<any> fObservableMemory;  //!
#endif

    Int_t fNObservables;
    std::vector<TString> fObservableNames;
    std::vector<TString> fObservableDescriptions;
    std::vector<TString> fObservableTypes;

    void PrintObservable(int N);
    void ConnectEventBranches();
    void ConnectObservables();
    void CreateEventBranches();
    void CreateObservableBranches();

   protected:
   public:
    void Initialize();

    bool IsConnected() { return fConnected; }
    bool IsBranchesCreated() { return fBranchesCreated; }

    Int_t GetObservableID(TString obsName) {
        for (int i = 0; i < GetNumberOfObservables(); i++)
            if (fObservableNames[i] == obsName) return i;
        return -1;
    }

    Bool_t ObservableExists(TString obsName) {
        if (GetObservableID(obsName) >= 0) return true;
        return false;
    }

    Int_t GetEventID() { return fEventID; }
    Int_t GetSubEventID() { return fSubEventID; }
    Double_t GetTimeStamp() { return fTimeStamp; }
    TString GetSubEventTag() { return *fSubEventTag; }
    Int_t GetRunOrigin() { return fRunOrigin; }
    Int_t GetSubRunOrigin() { return fSubRunOrigin; }

    Int_t GetNumberOfObservables() { return fNObservables; }

    // observable method
    any GetObservable(Int_t n) {
        return fObservableMemory[n];
    }  // TODO implement error message in case n >= fNObservables
    TString GetObservableName(Int_t n) {
        return fObservableNames[n];
    }  // TODO implement error message in case n >= fNObservables
    TString GetObservableDescription(Int_t n) { return fObservableDescriptions[n]; }

    TString GetObservableType(Int_t n) {
        if (fNObservables > 0 && fObservableTypes.size() == 0) return "double";
        return fObservableTypes[n];
    }

    TString GetObservableType(TString obsName) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) return "NotFound";
        return GetObservableType(id);
    }

    Double_t GetDblObservableValue(TString obsName) {
        return GetDblObservableValue(GetObservableID(obsName));
    }

    Double_t GetDblObservableValue(Int_t n) {
        if (GetObservableType(n) == "int") return GetObservableValue<int>(n);
        if (GetObservableType(n) == "double") return GetObservableValue<double>(n);

        cout << "TRestAnalysisTree::GetDblObservableValue. Type " << GetObservableType(n)
             << " not supported! Returning zero" << endl;
        return 0.;
    }

    template <class T>
    T GetObservableValue(Int_t n) {
        return *(T*)fObservableMemory[n];
    }
    template <class T>
    T GetObservableValue(TString obsName) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) {
            return T();
        }
        return GetObservableValue<T>(id);
    }

    template <class T>
    T GetObservableValueSafe(Int_t n) {
        if (REST_Reflection::GetTypeName<T>() != fObservableMemory[n].type) {
            cout << "Error! TRestAnalysisTree::GetObservableValueSafe(): unmatched type!" << endl;
            return T();
        }
        return *(T*)fObservableMemory[n];
    }
    template <class T>
    T GetObservableValueSafe(TString obsName) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) {
            return T();
        }
        return GetObservableValueSafe<T>(id);
    }

    template <class T>
    void SetObservableValue(Int_t n, const T& value) {
        if (!fBranchesCreated) {
            // if the observable branches are not created, we still have the chance to change
            // we check whether the value in the specified type(saved in fObservableTypes)
            // if not, we reset fObservableTypes and fObservableMemory according to the
            // value. Memory leak is inevitable.
            TString type = REST_Reflection::GetTypeName<T>();
            if (type != fObservableTypes[n]) {
                fObservableTypes[n] = type;
                fObservableMemory[n] = REST_Reflection::Assembly((string)type);
            }
        }
        if (n != -1 && n < fObservableMemory.size()) fObservableMemory[n].SetValue(value);
    }
    template <class T>
    void SetObservableValue(TString name, const T& value) {
        // string name_fixed = Replace((string)ProcName_ObsName, ".", "_", 0);
        Int_t id = GetObservableID(name);
        if (id == -1 && !fBranchesCreated) {
            id = AddObservable<T>(name);
        }
        SetObservableValue(id, value);
    }
    void SetObservableValue(Int_t n, any value) { value >> fObservableMemory[n]; }

    void CreateBranches();

    void CopyObservableList(TRestAnalysisTree* from, string prefix = "");

    void PrintObservables(TRestEventProcess* proc = 0, int NObservables = 9999);

    void SetRunOrigin(Int_t run_origin) { fRunOrigin = run_origin; }
    void SetSubRunOrigin(Int_t sub_run_origin) { fSubRunOrigin = sub_run_origin; }

    void SetEventInfo(TRestEvent* evt);
    Int_t Fill(TRestEvent* evt = 0);

    Int_t AddObservable(TString objName, TRestMetadata* meta, TString description = "");
    Int_t AddObservable(TString observableName, TString observableType = "double", TString description = "");
    template <typename T>
    Int_t AddObservable(TString observableName, TString description = "") {
        return AddObservable(observableName, REST_Reflection::GetTypeName<T>(), description);
    }

    Bool_t EvaluateExpression(const string expression);

    Int_t GetEntry(Long64_t entry = 0, Int_t getall = 0);

    // Construtor
    TRestAnalysisTree();
    TRestAnalysisTree(TString name, TString title);
    // Destructor
    virtual ~TRestAnalysisTree();

    ClassDef(TRestAnalysisTree, 3);
};
#endif
