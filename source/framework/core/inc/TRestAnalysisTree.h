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
    Int_t fStatus = 0;                       //!
    Int_t fSetObservableCalls = 0;           //!
    Int_t fSetObservableIndex = 0;           //!
    Bool_t fQuickSetObservableValue = true;  //!
    std::vector<any> fObservables;           //!
    std::map<string, int> fObservableIdMap;  //!
    TTree* fROOTTree;                        //!

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
    void ReadLeafValueToObservable(TLeaf* lf, any& obs);
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
        //!< first status when constructed from ROOT tree
        ROOTTree = 6
    };

    TRestAnalysisTree(TTree* tree);

   protected:
   public:
    /////////////// Getters ////////////////

    // Get the status of this tree. This call will not evaluate the status.
    int GetStatus() { return fStatus; }
    Int_t GetObservableID(const string& obsName);
    Bool_t ObservableExists(const string& obsName);
    Int_t GetEventID() { return fEventID; }
    Int_t GetSubEventID() { return fSubEventID; }
    Double_t GetTimeStamp() { return fTimeStamp; }
    TString GetSubEventTag() { return *fSubEventTag; }
    Int_t GetRunOrigin() { return fRunOrigin; }
    Int_t GetSubRunOrigin() { return fSubRunOrigin; }
    Int_t GetNumberOfObservables() { return fNObservables; }

    // observable method
    any GetObservable(Int_t n);
    TString GetObservableName(Int_t n);
    TString GetObservableDescription(Int_t n);
    TString GetObservableType(Int_t n);
    TString GetObservableType(string obsName);
    Double_t GetDblObservableValue(string obsName);
    Double_t GetDblObservableValue(Int_t n);

    ///////////////////////////////////////////////
    /// \brief Get observable in a given type, according to its id.
    template <class T>
    T GetObservableValue(Int_t n, bool safe = false) {
        if (safe && REST_Reflection::GetTypeName<T>() != fObservables[n].type) {
            cout << "Error! TRestAnalysisTree::GetObservableValue(): unmatched type!" << endl;
            return T();
        }
        if (safe && n >= fNObservables) {
            cout << "Error! TRestAnalysisTree::GetObservableValue(): index outside limits!" << endl;
            return T();
        }
        return *(T*)fObservables[n];
    }
    ///////////////////////////////////////////////
    /// \brief Get observable in a given type, according to its name.
    ///
    /// The returned value is directly the type. When set safe == true, it will
    /// preform type name check before converting to the specified type, and make
    /// it safer(otherwise there is potential seg.fault if you convert for example
    /// int* to double). Note that type name reflection takes time.
    ///
    /// Example:
    /// `vector<int> v = AnalysisTree->GetObservableValue<vector<int>>("myvec1");`
    /// `double a = AnalysisTree->GetObservableValue<double>("myval");`
    template <class T>
    T GetObservableValue(string obsName, bool safe = false) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) {
            return T();
        }
        return GetObservableValue<T>(id, safe);
    }

    ///////////////////////////////////////////////
    /// \brief Set the value of observable whose index is id
    template <class T>
    void SetObservableValue(const Int_t& id, const T& value) {
        if (id >= fNObservables) {
            cout << "Error! TRestAnalysisTree::SetObservableValue(): index outside limits!" << endl;
            return;
        }
        *(T*)fObservables[id] = value;
    }
    ///////////////////////////////////////////////
    /// \brief Set the value of observable. May not check the name.
    ///
    /// Any type of input value is accepted. We can directly set value from a
    /// vector or map object. If the observable does not exist, it will create
    /// a new one if the tree is not filled yet. If fQuickSetObservableValue == true
    /// we directly set the observable whose index is equal to fSetObservableIndex,
    /// and increase fSetObservableIndex at each call of this method. Otherwise we
    /// first find the index of the observable with given name, and then set its value.
    ///
    /// Note that observable type check is not performed in this method. To make
    /// it safe consider using SetObservable(). If the method is not called in the
    /// loop with equal times, one needs to disable quick observable value setting
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
    /// tree->SetObservableValue("myvec", vector<int>{11,23,37,41});
    /// tree->Fill();
    /// tree->SetObservableValue("myvec", vector<int>{2,3,5,7});
    /// tree->SetObservableValue("myval", 30);
    /// tree->Fill();
    /// tree->SetObservableValue("myval", 40);
    /// tree->Fill();
    ///
    /// \endcode
    template <class T>
    void SetObservableValue(const string& name, const T& value) {
        if (fQuickSetObservableValue && fStatus == Filled && fSetObservableCalls == fNObservables) {
            SetObservableValue(fSetObservableIndex, value);
            fSetObservableIndex++;
            return;
        }

        int id = GetObservableID(name);
        if (id != -1) {
            *(T*)fObservables[id] = value;
        } else {
            AddObservable<T>(name);
            id = GetObservableID(name);
            if (id != -1) {
                *(T*)fObservables[id] = value;
            }
        }
        fSetObservableCalls++;
    }

    void SetObservable(Int_t id, any obs);
    void SetObservable(string name, any value);

    void PrintObservables();
    void PrintObservable(int N);

    void SetRunOrigin(Int_t run_origin) { fRunOrigin = run_origin; }
    void SetSubRunOrigin(Int_t sub_run_origin) { fSubRunOrigin = sub_run_origin; }

    void SetEventInfo(TRestEvent* evt);
    Int_t Fill();

    any AddObservable(TString observableName, TString observableType = "double", TString description = "");
    template <typename T>
    T& AddObservable(TString observableName, TString description = "") {
        return *(T*)AddObservable(observableName, REST_Reflection::GetTypeName<T>(), description);
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

    void EnableQuickObservableValueSetting();
    void DisableQuickObservableValueSetting();

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
