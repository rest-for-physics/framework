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
        Error = -1,
        None = 0,
        Created = 1,
        Retrieved = 2,
        EmptyCloned = 3,
        Connected = 4,
        Filled = 5,
        ROOTTree = 6
    };

    TRestAnalysisTree(TTree* tree);

   protected:
   public:
    // getters
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
    TString GetObservableType(string obsName) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) return "NotFound";
        return GetObservableType(id);
    }

    Double_t GetDblObservableValue(string obsName) { return GetDblObservableValue(GetObservableID(obsName)); }
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
    T GetObservableValue(string obsName) {
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
    T GetObservableValueSafe(const string& obsName) {
        Int_t id = GetObservableID(obsName);
        if (id == -1) {
            return T();
        }
        return GetObservableValueSafe<T>(id);
    }

    template <class T>
    void SetObservableValue(const string& name, const T& value) {
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
    }
    template <class T>
    void SetObservableValue(const Int_t& id, const T& value) {
        *(T*)fObservables[id] = value;
    }

    void SetObservable(string name, any value) {
        value.name = name;
        SetObservable(-1, value);
    }
    void SetObservable(Int_t id, any obs);

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
