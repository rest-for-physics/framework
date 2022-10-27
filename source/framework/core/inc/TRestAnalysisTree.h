/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestAnalysisTree
#define RestCore_TRestAnalysisTree

#include <limits>
#include "TChain.h"
#include "TRestEvent.h"
#include "TRestReflector.h"
#include "TTree.h"

//! REST core data-saving helper based on TTree
class TRestAnalysisTree : public TTree {
   private:
    Int_t fEventID;         //!
    Int_t fSubEventID;      //!
    Double_t fTimeStamp;    //!
    TString* fSubEventTag;  //!
    Int_t fRunOrigin;       //!
    Int_t fSubRunOrigin;    //!

    //
    Int_t fStatus = 0;                                  //!
    Int_t fSetObservableCalls = 0;                      //!
    Int_t fSetObservableIndex = 0;                      //!
    Bool_t fQuickSetObservableValue = true;             //!
    std::vector<RESTValue> fObservables;                //!
    std::map<std::string, int> fObservableIdMap;        //!
    std::map<std::string, int> fObservableIdSearchMap;  //! used for quick search of certain observables
    TChain* fChain = nullptr;                           //! in case multiple files for reading

    // for storage
    Int_t fNObservables;
    std::vector<TString> fObservableNames;
    std::vector<TString> fObservableDescriptions;
    std::vector<TString> fObservableTypes;

    void Initialize();
    int EvaluateStatus();
    void UpdateObservables();
    void UpdateBranches();
    void InitObservables();
    void MakeObservableIdMap();
    void ReadLeafValueToObservable(TLeaf* lf, RESTValue& obs);
    bool BranchesExist() { return GetListOfBranches()->GetEntriesFast() > 0; }

    enum TRestAnalysisTree_Status {
        //!< Error state
        Error = -1,
        //!< First status when constructed as empty tree
        None = 0,
        //!< Status during the first loop of SetObservableValue() before Fill() and after
        //! construction. No branches are created and filled. Some observables may be added.
        //! We will create branches when we call Fill(). We will update branches when we call
        //! PrintObservables(). The status will become "Filled" and "Connected" respectivelly
        Created = 1,
        //!< There are branches in the tree with data, but no observables are added.
        //! This means the tree was just retrieved from root file. In this case, when calling
        //! GetEntry() or Fill(), we will first recover the observables and connect them to the
        //! branches. We forbid to add new observable to the tree. After reconnection, the
        //! status will become "Filled"
        Retrieved = 2,
        //!< There are branches in the tree but with no data(i.e. entry number is 0).
        //! No observables are added either. This means the tree could be an empty clone
        //! of another TRestAnalysisTree. Adding new observable is allowed in this case.
        EmptyCloned = 3,
        //!< There are branches in the tree with no data. There are observables in the list.
        //! same as "Created"
        Connected = 4,
        //!< There are branches in the tree with data. There are observables in the list.
        //! Once filled, it is forbidden to add new observable to the tree.
        Filled = 5,
    };

    TRestAnalysisTree(TTree* tree);

   protected:
   public:
    /////////////// Getters ////////////////

    // Get the status of this tree. This call will not evaluate the status.
    inline int GetStatus() const { return fStatus; }
    Int_t GetObservableID(const std::string& obsName);
    Int_t GetMatchedObservableID(const std::string& obsName);
    Bool_t ObservableExists(const std::string& obsName);
    // six basic event prameters
    Int_t GetEventID() { return fChain ? ((TRestAnalysisTree*)fChain->GetTree())->GetEventID() : fEventID; }
    Int_t GetSubEventID() {
        return fChain ? ((TRestAnalysisTree*)fChain->GetTree())->GetSubEventID() : fSubEventID;
    }
    Double_t GetTimeStamp() {
        return fChain ? ((TRestAnalysisTree*)fChain->GetTree())->GetTimeStamp() : fTimeStamp;
    }
    TString GetSubEventTag() {
        return fChain ? ((TRestAnalysisTree*)fChain->GetTree())->GetSubEventTag() : *fSubEventTag;
    }
    // we suppose all the chained trees have same run and sub run id.
    // so there is no need to call fChain->GetTree()
    Int_t GetRunOrigin() { return fRunOrigin; }
    Int_t GetSubRunOrigin() { return fSubRunOrigin; }
    Int_t GetNumberOfObservables() { return fNObservables; }

    // observable method
    RESTValue GetObservable(std::string obsName);
    RESTValue GetObservable(Int_t n);
    TString GetObservableName(Int_t n);
    TString GetObservableDescription(Int_t n);
    TString GetObservableType(Int_t n);
    TString GetObservableType(std::string obsName);
    Double_t GetDblObservableValue(std::string obsName);
    Double_t GetDblObservableValue(Int_t n);

    ///////////////////////////////////////////////
    /// \brief Get observable in a given type, according to its id.
    template <class T>
    T GetObservableValue(Int_t n) {
        // id check
        if (n >= fNObservables) {
            std::cout << "Error! TRestAnalysisTree::GetObservableValue(): index outside limits!" << std::endl;
            return std::numeric_limits<T>::quiet_NaN();
        }
        if (fChain != nullptr) {
            return ((TRestAnalysisTree*)fChain->GetTree())->GetObservableValue<T>(n);
        }
        return fObservables[n].GetValue<T>();
    }
    ///////////////////////////////////////////////
    /// \brief Get observable in a given type, according to its name.
    ///
    /// The returned value is directly the type.
    ///
    /// Example:
    /// `std::vector<int> v = AnalysisTree->GetObservableValue<std::vector<int>>("myvec1");`
    /// `double a = AnalysisTree->GetObservableValue<double>("myval");`
    template <class T>
    T GetObservableValue(std::string obsName) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) {
            // try to find matched observables
            id = GetMatchedObservableID(obsName);
            if (id == -1) {
                return std::numeric_limits<T>::quiet_NaN();
            }
        }
        return GetObservableValue<T>(id);
    }

    ///////////////////////////////////////////////
    /// \brief Set the value of observable whose index is id
    template <class T>
    void SetObservableValue(const Int_t& id, const T& value) {
        // id check
        if (id >= fNObservables) {
            std::cout << "Error! TRestAnalysisTree::SetObservableValue(): index outside limits!" << std::endl;
            return;
        }
        if (fChain != nullptr) {
            std::cout << "Error! cannot set observable! AnalysisTree is in chain state" << std::endl;
            return;
        }
        fObservables[id].SetValue(value);
    }
    ///////////////////////////////////////////////
    /// \brief Set the value of observable. May not check the name.
    ///
    /// Any type of input value is accepted. We can directly set value from a
    /// std::vector or std::map object. If the observable does not exist, it will create
    /// a new one if the tree is not filled yet. If fQuickSetObservableValue == true
    /// we directly set the observable whose index is equal to fSetObservableIndex,
    /// and increase fSetObservableIndex at each call of this method. Otherwise we
    /// first find the index of the observable with given name, and then set its value.
    ///
    /// If the method is not called in the loop with equal times(e.g. when manually typing
    /// the code in ROOT prompt), one needs to disable quick observable value setting
    /// before calling the method, or consider to use SetObservable()
    ///
    /// Example:
    /// \code
    ///
    /// TRestAnalysisTree* tree = new TRestAnalysisTree();
    /// for (int i = 0; i < 1000000; i++) {
    ///    tree->SetObservableValue("gaus", gRandom->Gaus(100, 20));
    ///    tree->SetObservableValue("poisson", gRandom->Poisson(36));
    ///    tree->SetObservableValue("rndm", gRandom->Rndm());
    ///    tree->SetObservableValue("landau", gRandom->Landau(10, 2));
    ///    tree->Fill();
    /// }
    ///
    /// TRestAnalysisTree* tree = new TRestAnalysisTree();
    /// tree->DisableQuickObservableValueSetting()
    /// tree->SetObservableValue("myval", 20);
    /// tree->SetObservableValue("myvec", std::vector<int>{11,23,37,41});
    /// tree->Fill();
    /// tree->SetObservableValue("myvec", std::vector<int>{2,3,5,7});
    /// tree->SetObservableValue("myval", 30);
    /// tree->Fill();
    /// tree->SetObservableValue("myval", 40);
    /// tree->Fill();
    ///
    /// \endcode
    template <class T>
    void SetObservableValue(const std::string& name, const T& value) {
        if (fQuickSetObservableValue && fStatus == Filled && fSetObservableCalls == fNObservables) {
            SetObservableValue(fSetObservableIndex, value);
            fSetObservableIndex++;
            return;
        }

        int id = GetObservableID(name);
        if (id != -1) {
            SetObservableValue(id, value);
        } else {
            AddObservable<T>(name);
            id = GetObservableID(name);
            if (id != -1) {
                SetObservableValue(id, value);
                fSetObservableCalls++;
            }
        }
    }

    void SetObservable(Int_t id, RESTValue obs);
    void SetObservable(std::string name, RESTValue value);

    void PrintObservables();
    void PrintObservable(int N);

    void SetRunOrigin(Int_t run_origin) { fRunOrigin = run_origin; }
    void SetSubRunOrigin(Int_t sub_run_origin) { fSubRunOrigin = sub_run_origin; }

    void SetEventInfo(TRestAnalysisTree* tree);
    void SetEventInfo(TRestEvent* evt);
    Int_t Fill();

    RESTValue AddObservable(TString observableName, TString observableType = "double",
                            TString description = "");
    template <typename T>
    T& AddObservable(TString observableName, TString description = "") {
        return *(T*)AddObservable(observableName, REST_Reflection::GetTypeName<T>(), description);
    }

    Int_t GetEntry(Long64_t entry = 0, Int_t getall = 0);

    Bool_t EvaluateCuts(const std::string expression);
    Bool_t EvaluateCut(const std::string expression);

    TString GetStringWithObservableNames();

    std::vector<std::string> GetCutObservables(const std::string cut_str);

    void EnableBranches(std::vector<std::string> obsNames);
    void DisableBranches(std::vector<std::string> obsNames);

    void EnableAllBranches();
    void DisableAllBranches();

    void EnableQuickObservableValueSetting();
    void DisableQuickObservableValueSetting();

    Double_t GetObservableAverage(TString obsName, Double_t xLow = -1, Double_t xHigh = -1,
                                  Int_t nBins = 1000);

    Double_t GetObservableRMS(TString obsName, Double_t xLow = -1, Double_t xHigh = -1, Int_t nBins = 1000);

    Double_t GetObservableMinimum(TString obsName, Double_t xLow = -1, Double_t xHigh = -1,
                                  Int_t nBins = 1000);
    Double_t GetObservableMaximum(TString obsName, Double_t xLow = -1, Double_t xHigh = -1,
                                  Int_t nBins = 1000);

    Int_t WriteAsTTree(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);

    Bool_t AddChainFile(std::string file);

    TTree* GetTree() const;

    TChain* GetChain() { return fChain; }

    Long64_t LoadTree(Long64_t entry);

    Long64_t GetEntries() const;

    Long64_t GetEntries(const char* sel);

    void Browse(TBrowser* b) { fChain ? fChain->Browse(b) : TTree::Browse(b); }
    Long64_t Draw(const char* varexp, const TCut& selection, Option_t* option = "",
                  Long64_t nentries = kMaxEntries, Long64_t firstentry = 0) {
        return fChain ? fChain->Draw(varexp, selection, option, nentries, firstentry)
                      : TTree::Draw(varexp, selection, option, nentries, firstentry);
    }
    Long64_t Draw(const char* varexp, const char* selection, Option_t* option = "",
                  Long64_t nentries = kMaxEntries, Long64_t firstentry = 0) {
        return fChain ? fChain->Draw(varexp, selection, option, nentries, firstentry)
                      : TTree::Draw(varexp, selection, option, nentries, firstentry);
    }
    void Draw(Option_t* opt) { fChain ? fChain->Draw(opt) : TTree::Draw(opt); }
    TBranch* FindBranch(const char* name) {
        return fChain ? fChain->FindBranch(name) : TTree::FindBranch(name);
    }
    TLeaf* FindLeaf(const char* name) { return fChain ? fChain->FindLeaf(name) : TTree::FindLeaf(name); }
    TBranch* GetBranch(const char* name) { return fChain ? fChain->GetBranch(name) : TTree::GetBranch(name); }
    Bool_t GetBranchStatus(const char* branchname) const {
        return fChain ? fChain->GetBranchStatus(branchname) : TTree::GetBranchStatus(branchname);
    }
    Long64_t GetCacheSize() const { return fChain ? fChain->GetCacheSize() : TTree::GetCacheSize(); }
    Long64_t GetChainEntryNumber(Long64_t entry) const {
        return fChain ? fChain->GetChainEntryNumber(entry) : TTree::GetChainEntryNumber(entry);
    }
    Long64_t GetEntryNumber(Long64_t entry) const {
        return fChain ? fChain->GetEntryNumber(entry) : TTree::GetEntryNumber(entry);
    }
    Long64_t GetReadEntry() const { return fChain ? fChain->GetReadEntry() : TTree::GetReadEntry(); }

    TH1* GetHistogram() { return fChain ? fChain->GetHistogram() : TTree::GetHistogram(); }
    TLeaf* GetLeaf(const char* branchname, const char* leafname) {
        return fChain ? fChain->GetLeaf(branchname, leafname) : TTree::GetLeaf(branchname, leafname);
    }
    TLeaf* GetLeaf(const char* name) { return fChain ? fChain->GetLeaf(name) : TTree::GetLeaf(name); }
    TObjArray* GetListOfBranches() {
        return fChain ? fChain->GetListOfBranches() : TTree::GetListOfBranches();
    }
    TObjArray* GetListOfLeaves() { return fChain ? fChain->GetListOfLeaves() : TTree::GetListOfLeaves(); }
    Long64_t Process(const char* filename, Option_t* option = "", Long64_t nentries = kMaxEntries,
                     Long64_t firstentry = 0) {
        return fChain ? fChain->Process(filename, option, nentries, firstentry)
                      : TTree::Process(filename, option, nentries, firstentry);
    }
    Long64_t Process(TSelector* selector, Option_t* option = "", Long64_t nentries = kMaxEntries,
                     Long64_t firstentry = 0) {
        return fChain ? fChain->Process(selector, option, nentries, firstentry)
                      : TTree::Process(selector, option, nentries, firstentry);
    }
    Long64_t Scan(const char* varexp = "", const char* selection = "", Option_t* option = "",
                  Long64_t nentries = kMaxEntries, Long64_t firstentry = 0) {
        return fChain ? fChain->Scan(varexp, selection, option, nentries, firstentry)
                      : TTree::Scan(varexp, selection, option, nentries, firstentry);
    }
    Int_t SetBranchAddress(const char* bname, void* add, TBranch** ptr = 0) {
        return fChain ? fChain->SetBranchAddress(bname, add, ptr) : TTree::SetBranchAddress(bname, add, ptr);
    }
    Int_t SetBranchAddress(const char* bname, void* add, TBranch** ptr, TClass* realClass, EDataType datatype,
                           Bool_t isptr) {
        return fChain ? fChain->SetBranchAddress(bname, add, ptr, realClass, datatype, isptr)
                      : TTree::SetBranchAddress(bname, add, ptr, realClass, datatype, isptr);
    }
    Int_t SetBranchAddress(const char* bname, void* add, TClass* realClass, EDataType datatype,
                           Bool_t isptr) {
        return fChain ? fChain->SetBranchAddress(bname, add, realClass, datatype, isptr)
                      : TTree::SetBranchAddress(bname, add, realClass, datatype, isptr);
    }
    void SetBranchStatus(const char* bname, Bool_t status = 1, UInt_t* found = 0) {
        fChain ? fChain->SetBranchStatus(bname, status, found) : TTree::SetBranchStatus(bname, status, found);
    }
    void SetDirectory(TDirectory* dir) { fChain ? fChain->SetDirectory(dir) : TTree::SetDirectory(dir); }

    void ResetBranchAddress(TBranch* br) {
        fChain ? fChain->ResetBranchAddress(br) : TTree::ResetBranchAddress(br);
    }
    void ResetBranchAddresses() { fChain ? fChain->ResetBranchAddresses() : TTree::ResetBranchAddresses(); }

    // Constructor
    TRestAnalysisTree();
    TRestAnalysisTree(TString name, TString title);
    static TRestAnalysisTree* ConvertFromTTree(TTree* tree);
    // Destructor
    virtual ~TRestAnalysisTree();

    ClassDef(TRestAnalysisTree, 3);
};

#endif
