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
    Bool_t fConnected;        //!
    Bool_t fBranchesCreated;  //!

    Int_t fEventID;         //!
    Int_t fSubEventID;      //!
    Double_t fTimeStamp;    //!
    TString* fSubEventTag;  //!
    Int_t fRunOrigin;       //!
    Int_t fSubRunOrigin;    //!

    //
    std::vector<any> fObservables;            //!
    std::map<TString, int> fObservableIdMap;  //!
    TTree* fROOTTree;//!

    // for storage
    Int_t fNObservables;
    std::vector<TString> fObservableNames;
    std::vector<TString> fObservableDescriptions;
    std::vector<TString> fObservableTypes;

    void Initialize();
    void ConnectBranches();
    void CreateBranches();
    void InitObservables();
    void MakeObservableIdMap();
    void ReadLeafValueToObservable(TLeaf* lf, any& obs);

    TRestAnalysisTree(TTree* tree);
   protected:


   public:
    // getters
    bool IsConnected() { return fConnected; }
    bool IsBranchesCreated() { return fBranchesCreated; }
    Int_t GetObservableID(TString obsName);
    Bool_t ObservableExists(TString obsName);
    Int_t GetEventID() { return fEventID; }
    Int_t GetSubEventID() { return fSubEventID; }
    Double_t GetTimeStamp() { return fTimeStamp; }
    TString GetSubEventTag() { return *fSubEventTag; }
    Int_t GetRunOrigin() { return fRunOrigin; }
    Int_t GetSubRunOrigin() { return fSubRunOrigin; }
    Int_t GetNumberOfObservables() { return fNObservables; }

    // observable method
    any GetObservable(Int_t n) {
        return fObservables[n];
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
        return *(T*)fObservables[n];
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
        if (REST_Reflection::GetTypeName<T>() != fObservables[n].type) {
            cout << "Error! TRestAnalysisTree::GetObservableValueSafe(): unmatched type!" << endl;
            return T();
        }
        return *(T*)fObservables[n];
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
    void SetObservableValue(TString name, const T& value) {
        any val(value);
        val.name = name;
        SetObservableValue(-1, val);
    }
    void SetObservableValue(Int_t id, any obs);

    void PrintObservables();
    void PrintObservable(int N);

    void SetRunOrigin(Int_t run_origin) { fRunOrigin = run_origin; }
    void SetSubRunOrigin(Int_t sub_run_origin) { fSubRunOrigin = sub_run_origin; }

    void SetEventInfo(TRestEvent* evt);
    Int_t Fill();

    Int_t AddObservable(TString observableName, TString observableType = "double", TString description = "");
    template <typename T>
    Int_t AddObservable(TString observableName, TString description = "") {
        return AddObservable(observableName, REST_Reflection::GetTypeName<T>(), description);
    }

    Int_t GetEntry(Long64_t entry = 0, Int_t getall = 0);

    Bool_t EvaluateCuts(const string expression);
    Bool_t EvaluateCut(const string expression);

    TString GetStringWithObservableNames();

    vector<string> GetCutObservables(const string cut_str);

    void EnableBranches(vector<string> obsNames);
    void DisableBranches(vector<string> obsNames);

    void EnableAllBranches();
    void DisableAllBranches();

    Double_t GetObservableAverage(TString obsName, Double_t xLow = -1, Double_t xHigh = -1,
                                  Int_t nBins = 1000);

    Double_t GetObservableRMS(TString obsName, Double_t xLow = -1, Double_t xHigh = -1, Int_t nBins = 1000);

    Double_t GetObservableMinimum(TString obsName, Double_t xLow = -1, Double_t xHigh = -1,
                                  Int_t nBins = 1000);
    Double_t GetObservableMaximum(TString obsName, Double_t xLow = -1, Double_t xHigh = -1,
                                  Int_t nBins = 1000);

    // Construtor
    TRestAnalysisTree();
    TRestAnalysisTree(TString name, TString title);
    static TRestAnalysisTree* ConvertFromTTree(TTree* tree);
    // Destructor
    virtual ~TRestAnalysisTree();

    ClassDef(TRestAnalysisTree, 3);
};
#endif
