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

//////////////////////////////////////////////////////////////////////////
///
/// TRestAnalysisTree is TTree but with **managed objects** for the branches to fill.
/// There are six fixed branches of event information in TRestAnalysisTree: runOrigin,
/// subRunOrigin, eventID, subEventID, subEventTag and timeStamp. They are pointing
/// to the corresponding class members inside TRestAnalysisTree. Those branches are
/// called `event branches`. Additional branches could be added by the user, they will
/// point to some objects whose addresses are also stored in this class. Those objects
/// are called `observables`.
///
/// In traditional TTree case, the user defines multiple global variables, and add
/// branches with the address of these variables to the tree. Then the user changes the
/// value of those variables somewhere in the code, and calls `TTree::Fill()` to create
/// and save a new entry in the data list.
///
/// In TRestAnalysisTree, the concept of "Branch" is weakened. We can directly call
/// `SetObservableValue()` and then `TRestAnalysisTree::Fill()` to do the data saving.
/// The code could be simplified while sacrificing a little performance. We can use
/// temporary variable to set observable value directly. We can focus on the analysis
/// code inside the loop, without caring about varaible initialization before that.
///
/// The following is a summary of speed of filling 1000000 entries for TTree and
/// TRestAnalysisTree. Four observables and six event branches are added. We take the
/// average of 3 tests as the result. See the file pipeline/analysistree/testspeed.cpp
/// for more details.
///
/// Condition                           |    time(us)   |
/// A. Do not use observable            |      846,522  |
/// B. Use quick observable (default)   |    1,188,232  |
/// C. Do not use quick observable      |    2,014,646  |
/// D. Use reflected observable         |    8,425,772  |
/// TTree                               |      841,744  |
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-Mar: First implementation
/// 2019-May: Updated to support any type of observables
/// 2020-Oct: Updated to be free from "Branch" concept
///
///
/// \class      TRestAnalysisTree
/// \author     Javier Galan, Ni Kaixiang
///
///______________________________________________________________________________
///
//////////////////////////////////////////////////////////////////////////

#include "TRestAnalysisTree.h"

#include <TFile.h>
#include <TH1F.h>
#include <TLeaf.h>
#include <TObjArray.h>

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"

using namespace std;

ClassImp(TRestAnalysisTree) using namespace std;

TRestAnalysisTree::TRestAnalysisTree() : TTree() {
    SetName("AnalysisTree");
    SetTitle("REST Analysis tree");

    Initialize();
}

TRestAnalysisTree::TRestAnalysisTree(TTree* tree) : TTree() {
    SetName("AnalysisTree");
    SetTitle("Linked to " + (TString)tree->GetName());

    Initialize();

    tree->GetEntry(0);

    TObjArray* lfs = tree->GetListOfLeaves();
    for (int i = 0; i < lfs->GetLast() + 1; i++) {
        TLeaf* lf = (TLeaf*)lfs->UncheckedAt(i);
        RESTValue obs;
        ReadLeafValueToObservable(lf, obs);
        obs.name = lf->GetName();
        SetObservable(-1, obs);
    }
}

TRestAnalysisTree* TRestAnalysisTree::ConvertFromTTree(TTree* tree) {
    if (tree->ClassName() == (TString) "TRestAnalysisTree") {
        return (TRestAnalysisTree*)tree;
    }

    return new TRestAnalysisTree(tree);
}

TRestAnalysisTree::TRestAnalysisTree(TString name, TString title) : TTree(name, title) {
    SetName(name);
    SetTitle(title);

    Initialize();
}

void TRestAnalysisTree::Initialize() {
    fRunOrigin = 0;
    fSubRunOrigin = 0;
    fEventID = 0;
    fSubEventID = 0;
    fSubEventTag = new TString();
    fTimeStamp = 0;

    fStatus = 0;
    fNObservables = 0;
    fChain = nullptr;

    fObservableIdMap.clear();
    fObservableDescriptions.clear();
    fObservableNames.clear();
    fObservables.clear();
}

///////////////////////////////////////////////
/// \brief Get the index of the specified observable.
///
/// If not exist, it will return -1. It will call MakeObservableIdMap() to
/// update observable id map before searching
Int_t TRestAnalysisTree::GetObservableID(const string& obsName) {
    MakeObservableIdMap();
    auto iter = fObservableIdMap.find(obsName);
    if (iter != fObservableIdMap.end()) {
        return iter->second;
    } else {
        return -1;
    }
    // if (!ObservableExists(obsName)) return -1;
    // return fObservableIdMap[obsName];
}

///////////////////////////////////////////////
/// \brief Get if the specified observable exists
///
/// It will call MakeObservableIdMap() to update observable id map before searching
Bool_t TRestAnalysisTree::ObservableExists(const string& obsName) {
    MakeObservableIdMap();
    return fObservableIdMap.count(obsName) > 0;
}

///////////////////////////////////////////////
/// \brief Evaluate the Status of this tree.
///
///////////////////////////////////////////////////////////////////////////////
// Status              | Nbranches  |  NObservables  | Entries |  Description
// 1. Created          |     0      |       0        |    0    |  The AnalysisTree is just created in code
// 2. Retrieved        |    >=6     |       0        |    >0   |  The AnalysisTree is just retrieved from file
// 3. EmptyCloned      |    >=6     |       0        |    0    |  The AnalysisTree is cloned from another
// tree, it is empty
// 4. Connected        |    >=6     |  >=Nbranches-6 |    0    |  Local observables are created and connected
// to the branches. We can still modify observables in this case.
// 5. Filled           |    >=6     |     =NObs      |    >0   |  Local observables are created and connected
// to the branches, and entry > 0. We cannot modify observables in this case. if Nbranches == 6, We regard the
// tree as Connected/Filled
//
// Status              |  AddObs Logic  |  Fill Logic  | GetEntry Logic
// 1. Created          |    Allowed     |   ->4, ->5   |    ->4
// 2. Retrieved        |    Denied      |     ->5      |    ->5
// 3. EmptyCloned      |     ->4        |   ->4, ->5   |    ->4
// 4. Connected        |    Allowed     |     ->5      |    Allowed
// 5. Filled           |    Denied      |     Allowed  |    Allowed
//
int TRestAnalysisTree::EvaluateStatus() {
    int NBranches = GetListOfBranches()->GetEntriesFast();
    int NObsInList = fObservables.size();
    int Entries = GetEntriesFast();

    // if (fROOTTree != nullptr) {
    //    return TRestAnalysisTree_Status::ROOTTree;
    //}

    if (NBranches == 0) {
        return TRestAnalysisTree_Status::Created;
    }

    if (NBranches > 6) {
        if (NObsInList == 0) {
            if (Entries > 0) {
                return TRestAnalysisTree_Status::Retrieved;
            } else {
                return TRestAnalysisTree_Status::EmptyCloned;
            }
        } else {
            if (Entries > 0) {
                return TRestAnalysisTree_Status::Filled;
            } else {
                return TRestAnalysisTree_Status::Connected;
            }
        }
    } else if (NBranches == 6) {
        TBranch* br = (TBranch*)GetListOfBranches()->UncheckedAt(0);
        if ((int*)br->GetAddress() != &fRunOrigin) {
            if (Entries > 0) {
                return TRestAnalysisTree_Status::Retrieved;
            } else {
                return TRestAnalysisTree_Status::EmptyCloned;
            }
        } else {
            if (Entries > 0) {
                return TRestAnalysisTree_Status::Filled;
            } else {
                return TRestAnalysisTree_Status::Connected;
            }
        }
    } else {
        return TRestAnalysisTree_Status::Error;
    }

    return TRestAnalysisTree_Status::Error;
}

///////////////////////////////////////////////
/// \brief Update observables stored in the tree.
///
/// It will first connect the six event data members(i.e. runid, eventid, etc.)
/// to the existing TTree branches. Then it will create new observable objects by
/// reflection, and connect them also to the existing TTree branches. After
/// re-connection, this method will change status 2->5, 3->4
void TRestAnalysisTree::UpdateObservables() {
    // connect basic event branches
    TBranch* br1 = GetBranch("runOrigin");
    TBranch* br2 = GetBranch("subRunOrigin");
    TBranch* br3 = GetBranch("eventID");
    TBranch* br4 = GetBranch("subEventID");
    TBranch* br5 = GetBranch("timeStamp");
    TBranch* br6 = GetBranch("subEventTag");

    if (br1 && br2 && br3 && br4 && br5 && br6) {
        br1->SetAddress(&fRunOrigin);
        br2->SetAddress(&fSubRunOrigin);
        br3->SetAddress(&fEventID);
        br4->SetAddress(&fSubEventID);
        br5->SetAddress(&fTimeStamp);
        br6->SetAddress(&fSubEventTag);
    } else {
        cout << "REST Error: TRestAnalysisTree::ConnectBranches(): event branches does not exist!" << endl;
        exit(1);
    }

    // create observables(no assembly) from stored observable info.
    fObservables = std::vector<RESTValue>(GetNumberOfObservables());
    for (int i = 0; i < GetNumberOfObservables(); i++) {
        fObservables[i] = REST_Reflection::WrapType((string)fObservableTypes[i]);
        fObservables[i].name = fObservableNames[i];
    }
    MakeObservableIdMap();

    if (fChain) {
        fChain->GetEntry(0);
    } else {
        TTree::GetEntry(0);
    }

    for (int i = 0; i < GetNumberOfObservables(); i++) {
        TBranch* branch = GetBranch(fObservableNames[i]);
        if (branch != nullptr) {
            if (branch->GetAddress() != nullptr) {
                if ((string)branch->ClassName() != "TBranch") {
                    // for TBranchElement the saved address is char**
                    fObservables[i].address = *(char**)branch->GetAddress();
                } else {
                    // for TBranch the saved address is char*
                    fObservables[i].address = branch->GetAddress();
                }
            } else {
                fObservables[i].Assembly();
                branch->SetAddress(fObservables[i].address);
            }
        }
    }

    fStatus = EvaluateStatus();
}

///////////////////////////////////////////////
/// \brief Update branches in the tree.
///
/// It will create branches if not exist, both for event data members and
/// observables. Note that this method can be called multiple times during the
/// first loop of observable setting. After branch creation, this method will
/// change status 1->4, or stay 4.
void TRestAnalysisTree::UpdateBranches() {
    if (!GetBranch("runOrigin")) Branch("runOrigin", &fRunOrigin);
    if (!GetBranch("subRunOrigin")) Branch("subRunOrigin", &fSubRunOrigin);
    if (!GetBranch("eventID")) Branch("eventID", &fEventID);
    if (!GetBranch("subEventID")) Branch("subEventID", &fSubEventID);
    if (!GetBranch("timeStamp")) Branch("timeStamp", &fTimeStamp);
    if (!GetBranch("subEventTag")) Branch("subEventTag", fSubEventTag);

    for (int n = 0; n < GetNumberOfObservables(); n++) {
        TString typeName = fObservableTypes[n];
        TString brName = fObservableNames[n];
        char* ref = fObservables[n].address;

        TBranch* branch = GetBranch(brName);
        if (branch == nullptr) {
            if (typeName == "double") {
                this->Branch(brName, (double*)ref);
            } else if (typeName == "float") {
                this->Branch(brName, (float*)ref);
            } else if (typeName == "long double") {
                this->Branch(brName, (long double*)ref);
            } else if (typeName == "bool") {
                this->Branch(brName, (bool*)ref);
            } else if (typeName == "char") {
                this->Branch(brName, (char*)ref);
            } else if (typeName == "int") {
                this->Branch(brName, (int*)ref);
            } else if (typeName == "short") {
                this->Branch(brName, (short*)ref);
            } else if (typeName == "long") {
                this->Branch(brName, (long*)ref);
            } else if (typeName == "long long") {
                this->Branch(brName, (long long*)ref);
            } else if (typeName == "unsigned long long") {
                this->Branch(brName, (unsigned long long*)ref);
            } else {
                this->Branch(brName, typeName, ref);
            }

            this->GetBranch(brName)->SetTitle("(" + typeName + ") " + fObservableDescriptions[n]);
        }
    }

    fStatus = EvaluateStatus();
}

void TRestAnalysisTree::InitObservables() {
    fObservables = std::vector<RESTValue>(GetNumberOfObservables());
    for (int i = 0; i < GetNumberOfObservables(); i++) {
        fObservables[i] = REST_Reflection::WrapType((string)fObservableTypes[i]);
        fObservables[i].name = fObservableNames[i];
    }
    MakeObservableIdMap();
}

///////////////////////////////////////////////
/// \brief Update the map of observable name to observable id.
///
/// Using map will improve the speed of "SetObservableValue"
void TRestAnalysisTree::MakeObservableIdMap() {
    if (fObservableIdMap.size() != fObservableNames.size()) {
        fObservableIdMap.clear();
        for (int i = 0; i < fObservableNames.size(); i++) {
            fObservableIdMap[(string)fObservableNames[i]] = i;
        }
        if (fObservableIdMap.size() != fObservableNames.size()) {
            cout << "REST ERROR! duplicated or blank observable name!" << endl;
        }
    }
}

void TRestAnalysisTree::ReadLeafValueToObservable(TLeaf* lf, RESTValue& obs) {
    if (lf == nullptr || lf->GetLen() == 0) return;

    if (obs.IsZombie()) {
        // this means we need to determine the observable type first
        string type;
        string leafdef(lf->GetBranch()->GetTitle());
        if (leafdef.find("[") == -1) {
            // there is no [] mark in the leaf. The leaf is a single values
            switch (leafdef[leafdef.size() - 1]) {
                case 'C':
                    type = "string";
                    break;
                case 'B':
                    type = "char";
                    break;
                case 'S':
                    type = "stort";
                    break;
                case 'I':
                    type = "int";
                    break;
                case 'F':
                    type = "float";
                    break;
                case 'D':
                    type = "double";
                    break;
                case 'L':
                    type = "long long";
                    break;
                case 'O':
                    type = "bool";
                    break;
                default:
                    ferr << "Unsupported leaf definition: " << leafdef << "! Assuming int" << endl;
                    type = "int";
            }
        } else {
            switch (leafdef[leafdef.size() - 1]) {
                case 'I':
                    type = "vector<int>";
                    break;
                case 'F':
                    type = "vector<float>";
                    break;
                case 'D':
                    type = "vector<double>";
                    break;
                default:
                    ferr << "Unsupported leaf definition: " << leafdef << "! Assuming vector<int>" << endl;
                    type = "vector<int>";
            }
        }
        obs = REST_Reflection::Assembly(type);
    }

    // start to fill value
    if (obs.is_data_type) {
        // the observable is basic type, we directly set it address from leaf
        obs.address = (char*)lf->GetValuePointer();
    } else if (obs.type == "vector<double>") {
        double* ptr = (double*)lf->GetValuePointer();
        vector<double> val(ptr, ptr + lf->GetLen());
        obs.SetValue(val);
    } else if (obs.type == "vector<int>") {
        int* ptr = (int*)lf->GetValuePointer();
        vector<int> val(ptr, ptr + lf->GetLen());
        obs.SetValue(val);
    } else if (obs.type == "vector<float>") {
        float* ptr = (float*)lf->GetValuePointer();
        vector<float> val(ptr, ptr + lf->GetLen());
        obs.SetValue(val);
    } else if (obs.type == "string") {
        char* ptr = (char*)lf->GetValuePointer();
        string val(ptr, lf->GetLen());
        obs.SetValue(val);
    } else {
        warning << "Unsupported observable type to convert from TLeaf!" << endl;
    }
}

RESTValue TRestAnalysisTree::GetObservable(string obsName) {
    Int_t id = GetObservableID(obsName);
    if (id == -1) return RESTValue();
    return GetObservable(id);
}

///////////////////////////////////////////////
/// \brief Get the observable object wrapped with "RESTValue" class, according to the id
RESTValue TRestAnalysisTree::GetObservable(Int_t n) {
    if (n >= fNObservables) {
        cout << "Error! TRestAnalysisTree::GetObservable(): index outside limits!" << endl;
        return RESTValue();
    }

    if (fChain != nullptr) {
        return ((TRestAnalysisTree*)fChain->GetTree())->GetObservable(n);
    }

    return fObservables[n];
}
///////////////////////////////////////////////
/// \brief Get the observable name according to id
TString TRestAnalysisTree::GetObservableName(Int_t n) {
    if (n >= fNObservables) {
        cout << "Error! TRestAnalysisTree::GetObservableName(): index outside limits!" << endl;
        return "";
    }
    return fObservableNames[n];
}
///////////////////////////////////////////////
/// \brief Get the observable description according to id
TString TRestAnalysisTree::GetObservableDescription(Int_t n) {
    if (n >= fNObservables) {
        cout << "Error! TRestAnalysisTree::GetObservableDescription(): index outside limits!" << endl;
        return "";
    }
    return fObservableDescriptions[n];
}
///////////////////////////////////////////////
/// \brief Get the observable type according to id
TString TRestAnalysisTree::GetObservableType(Int_t n) {
    if (n >= fNObservables) {
        cout << "Error! TRestAnalysisTree::GetObservableType(): index outside limits!" << endl;
        return "double";
    }
    return fObservableTypes[n];
}
///////////////////////////////////////////////
/// \brief Get the observable type according to its name
TString TRestAnalysisTree::GetObservableType(string obsName) {
    Int_t id = GetObservableID(obsName);
    if (id == -1) return "NotFound";
    return GetObservableType(id);
}
///////////////////////////////////////////////
/// \brief Get double value of the observable, according to the name.
///
/// It assumes the observable is in double/int type. If not it will print error and return 0
Double_t TRestAnalysisTree::GetDblObservableValue(string obsName) {
    return GetDblObservableValue(GetObservableID(obsName));
}
///////////////////////////////////////////////
/// \brief Get double value of the observable, according to the id.
///
/// It assumes the observable is in double/int type. If not it will print error and return 0
Double_t TRestAnalysisTree::GetDblObservableValue(Int_t n) {
    if (n >= fNObservables) {
        cout << "Error! TRestAnalysisTree::GetDblObservableValue(): index outside limits!" << endl;
        return 0;
    }

    if (GetObservableType(n) == "int") return GetObservableValue<int>(n);
    if (GetObservableType(n) == "double") return GetObservableValue<double>(n);

    cout << "TRestAnalysisTree::GetDblObservableValue. Type " << GetObservableType(n)
         << " not supported! Returning zero" << endl;
    return 0.;
}

RESTValue TRestAnalysisTree::AddObservable(TString observableName, TString observableType,
                                           TString description) {
    if (fStatus == None) fStatus = EvaluateStatus();

    if (fStatus == Created) {
    } else if (fStatus == Retrieved) {
        cout << "REST_Warning: cannot add observable for retrieved tree with data!" << endl;
        return -1;
    } else if (fStatus == EmptyCloned) {
        UpdateObservables();
    } else if (fStatus == Connected) {
    } else if (fStatus == Filled) {
        cout << "REST_Warning: cannot add observable! AnalysisTree is already filled" << endl;
        return -1;
    } else if (fChain != nullptr) {
        cout << "Error! cannot add observable! AnalysisTree is in chain state" << endl;
        return -1;
    }

    Double_t x = 0;
    if (GetObservableID((string)observableName) == -1) {
        RESTValue ptr = REST_Reflection::Assembly((string)observableType);
        ptr.name = observableName;
        if (!ptr.IsZombie()) {
            fObservableNames.push_back(observableName);
            fObservableIdMap[(string)observableName] = fObservableNames.size() - 1;
            fObservableDescriptions.push_back(description);
            fObservableTypes.push_back(observableType);
            fObservables.push_back(ptr);

            fNObservables++;
        } else {
            cout << "Error adding observable \"" << observableName << "\" with type \"" << observableType
                 << "\", type not found!" << endl;
            return -1;
        }
    } else {
        return GetObservableID((string)observableName);
    }

    return fObservables[fNObservables - 1];
}

void TRestAnalysisTree::PrintObservables() {
    if (fStatus == None) fStatus = EvaluateStatus();

    if (fStatus == Created) {
    } else if (fStatus == Retrieved) {
        cout << "TRestAnalysisTree::PrintObservables(): Reading entry 0" << endl;
        GetEntry(0);
    } else if (fStatus == EmptyCloned) {
        cout << "REST_Warning: AnalysisTree is empty" << endl;
        UpdateObservables();
    } else if (fStatus == Connected) {
        cout << "REST_Warning: AnalysisTree is empty" << endl;
    } else if (fStatus == Filled) {
    }

    cout.precision(15);
    std::cout << "Entry : " << GetReadEntry() << std::endl;
    std::cout << "> Event ID : " << GetEventID() << std::endl;
    std::cout << "> Event Time : " << GetTimeStamp() << std::endl;
    std::cout << "> Event Tag : " << GetSubEventTag() << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    cout.precision(6);

    for (int n = 0; n < GetNumberOfObservables(); n++) {
        PrintObservable(n);
    }
}

// print the Nth observable in the observal list
void TRestAnalysisTree::PrintObservable(int n) {
    if (n < 0 || n >= fNObservables) {
        return;
    }
    string obsVal = GetObservable(n).ToString();
    int lengthRemaining = Console::GetWidth() - 14 - 45 - 13;

    std::cout << "Observable : " << ToString(fObservableNames[n], 45)
              << "    Value : " << ToString(obsVal, lengthRemaining) << std::endl;
}

Int_t TRestAnalysisTree::GetEntry(Long64_t entry, Int_t getall) {
    if (fStatus == None) fStatus = EvaluateStatus();

    if (fStatus == Created) {
        UpdateBranches();
    } else if (fStatus == Retrieved) {
        UpdateObservables();
    } else if (fStatus == EmptyCloned) {
        UpdateObservables();
    } else if (fStatus == Connected) {
    } else if (fStatus == Filled) {
    }

    // if (fROOTTree != nullptr) {
    //    // if it is connected to an external tree, we get entry on that
    //    int result = fROOTTree->GetEntry(entry, getall);

    //    TObjArray* lfs = fROOTTree->GetListOfLeaves();
    //    if (fObservables.size() != lfs->GetLast() + 1) {
    //        cout << "Warning: external TTree may have changed!" << endl;
    //        for (int i = 0; i < lfs->GetLast() + 1; i++) {
    //            TLeaf* lf = (TLeaf*)lfs->UncheckedAt(i);
    //            RESTValue obs;
    //            ReadLeafValueToObservable(lf, obs);
    //            obs.name = lf->GetName();
    //            SetObservable(-1, obs);
    //        }
    //    } else {
    //        for (int i = 0; i < lfs->GetLast() + 1; i++) {
    //            TLeaf* lf = (TLeaf*)lfs->UncheckedAt(i);
    //            ReadLeafValueToObservable(lf, fObservables[i]);
    //        }
    //    }

    //    return result;
    //}
    if (fChain != nullptr) {
        return fChain->GetEntry(entry, getall);
    } else {
        return TTree::GetEntry(entry, getall);
    }
}

void TRestAnalysisTree::SetEventInfo(TRestAnalysisTree* tree) {
    if (fChain != NULL) {
        cout << "Error! cannot fill tree. AnalysisTree is in chain state" << endl;
        return;
    }

    if (tree != NULL) {
        fEventID = tree->GetEventID();
        fSubEventID = tree->GetSubEventID();
        fTimeStamp = tree->GetTimeStamp();
        *fSubEventTag = tree->GetSubEventTag();
        fRunOrigin = tree->GetRunOrigin();
        fSubRunOrigin = tree->GetSubRunOrigin();
    }
}

void TRestAnalysisTree::SetEventInfo(TRestEvent* evt) {
    if (fChain != nullptr) {
        cout << "Error! cannot fill tree. AnalysisTree is in chain state" << endl;
        return;
    }

    if (evt != nullptr) {
        fEventID = evt->GetID();
        fSubEventID = evt->GetSubID();
        fTimeStamp = evt->GetTimeStamp().AsDouble();
        *fSubEventTag = evt->GetSubEventTag();
        fRunOrigin = evt->GetRunOrigin();
        fSubRunOrigin = evt->GetSubRunOrigin();
    }
}

Int_t TRestAnalysisTree::Fill() {
    if (fStatus == None) fStatus = EvaluateStatus();

    if (fChain != nullptr) {
        cout << "Error! cannot fill tree. AnalysisTree is in chain state" << endl;
        return -1;
    }

    fSetObservableIndex = 0;

    if (fStatus == Filled) {
        return TTree::Fill();
    } else if (fStatus == Created) {
        UpdateBranches();
        int a = TTree::Fill();
        fStatus = EvaluateStatus();
        return a;
    } else if (fStatus == Retrieved) {
        UpdateObservables();
        int a = TTree::Fill();
        fStatus = EvaluateStatus();
        return a;
    } else if (fStatus == EmptyCloned) {
        UpdateObservables();
        int a = TTree::Fill();
        fStatus = EvaluateStatus();
        return a;
    } else if (fStatus == Connected) {
        UpdateBranches();
        int a = TTree::Fill();
        fStatus = EvaluateStatus();
        return a;
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief Set the value of observable whose id is as specified
///
void TRestAnalysisTree::SetObservable(Int_t id, RESTValue obs) {
    if (fChain != nullptr) {
        cout << "Error! cannot set observable! AnalysisTree is in chain state" << endl;
        return;
    }
    if (id == -1) {
        // this means we want to find observable id by its name
        id = GetObservableID(obs.name);
        // if not found, we have a chance to create observable
        if (id == -1) {
            AddObservable(obs.name, obs.type);
            id = GetObservableID(obs.name);
        }
    } else if (id == fObservables.size()) {
        // this means we want to add observable directly
        AddObservable(obs.name, obs.type);
        id = GetObservableID(obs.name);
    }
    if (id != -1) {
        if (obs.type != fObservables[id].type) {
            // if the observable branches are not created, and the type doesn't match,
            // we still have the chance to fix. We reset fObservableTypes and fObservableMemory
            // according to the input type value.
            cout << "Warning: SetObservableValue(): setting value with different type. Observable : "
                 << obs.name << ", existing type: " << fObservables[id].type
                 << ", value to add is in type: " << obs.type << ", trying to update to the new type."
                 << endl;
            fObservableTypes[id] = obs.type;
            string name = fObservables[id].name;
            fObservables[id].Destroy();
            fObservables[id] = REST_Reflection::Assembly(obs.type);
            fObservables[id].name = name;
        }
        obs >> fObservables[id];
    }
}

///////////////////////////////////////////////
/// \brief Set the value of observable whose name is as specified
///
/// The input type is "RESTValue". This class is able to be constructed from any object,
/// Therefore the input could be equal to SetObservableValue(). But in this case it
/// would be much slower than `SetObservableValue()`, since converting to "RESTValue" contains
/// a type name reflection which takes time. This method is better used to copy directly
/// "RESTValue" objects, e.g., observables from another TRestAnalysisTree.
///
/// Example:
/// \code
///
///     TFile* fin = new TFile("simpleTree.root", "open");
///     TRestAnalysisTree* treein = (TRestAnalysisTree*)fin->Get("AnalysisTree");
///     TRestAnalysisTree* treeout = (TRestAnalysisTree*)tmp_tree->CloneTree(0);
///
///     RESTValue c = treein->GetObservable("vec");
///     for (int i = 0; i < 10; i++) {
///         treein->GetEntry(i);
///         treeout->SetObservable("interitedvec", c);
///         treeout->Fill();
///     }
///
///
/// \endcode
///
void TRestAnalysisTree::SetObservable(string name, RESTValue value) {
    value.name = name;
    SetObservable(-1, value);
}

///////////////////////////////////////////////
/// \brief This method will receive a string with several analysis observables/cuts in the same fashion as
/// used by ROOT. The string must be constructed as follows "obsName1>value1 && obsName2>value2 && ...".
///
/// It will evaluate the given conditions and return the result. Valid operators are "==", "<=", ">=", "!=",
/// "=", ">" and "<".
///
Bool_t TRestAnalysisTree::EvaluateCuts(const string cut) {
    std::vector<string> cuts = Split(cut, "&&", false, true);

    for (int n = 0; n < cuts.size(); n++)
        if (!EvaluateCut(cuts[n])) return false;

    return true;
}

///////////////////////////////////////////////
/// \brief This method will evaluate a condition on one analysis *observable* constructed as
/// "observable>value". For example, "rawAna_NumberOfSignals>10".
///
/// It will evaluate the given conditions and return the result. Valid operators are "==", "<=", ">=", "!=",
/// "=", ">" and "<".
Bool_t TRestAnalysisTree::EvaluateCut(const string cut) {
    const std::vector<string> validOperators = {"==", "<=", ">=", "!=", "=", ">", "<"};

    if (cut.find("&&") != string::npos) return EvaluateCuts(cut);

    string oper = "", observable = "";
    Double_t value;
    for (int j = 0; j < validOperators.size(); j++) {
        if (cut.find(validOperators[j]) != string::npos) {
            oper = validOperators[j];
            observable = (string)cut.substr(0, cut.find(oper));
            value = std::stod((string)cut.substr(cut.find(oper) + oper.length(), string::npos));
            break;
        }
    }

    if (oper == "")
        cout << "TRestAnalysisTree::EvaluateCut. Invalid operator in cut definition! " << cut << endl;

    Double_t val = GetDblObservableValue(observable);
    if (oper == "==" && value == val) return true;
    if (oper == "!=" && value != val) return true;
    if (oper == "<=" && val <= value) return true;
    if (oper == ">=" && val >= value) return true;
    if (oper == "=" && val == value) return true;
    if (oper == ">" && val > value) return true;
    if (oper == "<" && val < value) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief It will return a list with the names found in a string with conditions, as given in methods as
/// EvaluateCuts. I.e. a construction as "obsName1==value1&&obsName2<=value2" will return {obsName1,obsName2}.
///
vector<string> TRestAnalysisTree::GetCutObservables(const string cut_str) {
    const std::vector<string> validOperators = {"==", "<=", ">=", "!=", "=", ">", "<"};

    std::vector<string> cuts = Split(cut_str, "&&", false, true);

    vector<string> obsNames;
    for (int n = 0; n < cuts.size(); n++) {
        string oper = "", observable = "";
        Double_t value;
        for (int j = 0; j < validOperators.size(); j++) {
            if (cuts[n].find(validOperators[j]) != string::npos) {
                obsNames.push_back((string)cuts[n].substr(0, cuts[n].find(validOperators[j])));
                break;
            }
        }
    }
    return obsNames;
}

///////////////////////////////////////////////
/// \brief It will enable the branches given by argument
///
void TRestAnalysisTree::EnableBranches(vector<string> obsNames) {
    for (int n = 0; n < obsNames.size(); n++) this->SetBranchStatus(obsNames[n].c_str(), true);
}

///////////////////////////////////////////////
/// \brief It will disable the branches given by argument
///
void TRestAnalysisTree::DisableBranches(vector<string> obsNames) {
    for (int n = 0; n < obsNames.size(); n++) this->SetBranchStatus(obsNames[n].c_str(), false);
}

///////////////////////////////////////////////
/// \brief It will enable all branches in the tree
///
void TRestAnalysisTree::EnableAllBranches() { this->SetBranchStatus("*", true); }

///////////////////////////////////////////////
/// \brief It will disable all branches in the tree
///
void TRestAnalysisTree::DisableAllBranches() { this->SetBranchStatus("*", false); }

///////////////////////////////////////////////
/// \brief It will enable quick observable value setting
///
/// When enabled, when calling SetObservableValue(string, value), we skip the
/// index searching and directly set the observable at n-th call of the method.
/// This is possible when multiple SetObservableValue() is called in sequence in
/// a loop. This will significantally improve the speed of that method.
void TRestAnalysisTree::EnableQuickObservableValueSetting() { this->fQuickSetObservableValue = true; }

///////////////////////////////////////////////
/// \brief It will disable quick observable value setting
void TRestAnalysisTree::DisableQuickObservableValueSetting() { this->fQuickSetObservableValue = false; }

///////////////////////////////////////////////
/// \brief It returns the average of the observable considering the given range. If no range is given
/// the full histogram range will be considered.
///
Double_t TRestAnalysisTree::GetObservableAverage(TString obsName, Double_t xLow, Double_t xHigh,
                                                 Int_t nBins) {
    TString histDefinition = Form("htemp(%5d,%lf,%lf)", nBins, xLow, xHigh);
    if (xHigh == -1)
        this->Draw(obsName);
    else
        this->Draw(obsName + ">>" + histDefinition);
    TH1F* htemp = (TH1F*)gPad->GetPrimitive("htemp");
    return htemp->GetMean();
}

///////////////////////////////////////////////
/// \brief It returns the RMS of the observable considering the given range. If no range is given
/// the full histogram range will be considered.
///
Double_t TRestAnalysisTree::GetObservableRMS(TString obsName, Double_t xLow, Double_t xHigh, Int_t nBins) {
    TString histDefinition = Form("htemp(%5d,%lf,%lf)", nBins, xLow, xHigh);
    if (xHigh == -1)
        this->Draw(obsName);
    else
        this->Draw(obsName + ">>" + histDefinition);
    TH1F* htemp = (TH1F*)gPad->GetPrimitive("htemp");
    return htemp->GetRMS();
}

///////////////////////////////////////////////
/// \brief It returns the maximum value of obsName considering the given range. If no range is given
/// the full histogram range will be considered.
///
Double_t TRestAnalysisTree::GetObservableMaximum(TString obsName, Double_t xLow, Double_t xHigh,
                                                 Int_t nBins) {
    TString histDefinition = Form("htemp(%5d,%lf,%lf)", nBins, xLow, xHigh);
    if (xHigh == -1)
        this->Draw(obsName);
    else
        this->Draw(obsName + ">>" + histDefinition);
    TH1F* htemp = (TH1F*)gPad->GetPrimitive("htemp");
    return htemp->GetMaximumStored();
}

///////////////////////////////////////////////
/// \brief It returns the minimum value of obsName considering the given range. If no range is given
/// the full histogram range will be considered.
///
Double_t TRestAnalysisTree::GetObservableMinimum(TString obsName, Double_t xLow, Double_t xHigh,
                                                 Int_t nBins) {
    TString histDefinition = Form("htemp(%5d,%lf,%lf)", nBins, xLow, xHigh);
    if (xHigh == -1)
        this->Draw(obsName);
    else
        this->Draw(obsName + ">>" + histDefinition);
    TH1F* htemp = (TH1F*)gPad->GetPrimitive("htemp");
    return htemp->GetMinimumStored();
}

///////////////////////////////////////////////
/// \brief It returns a string containing all the observables that exist in the analysis tree.
///
TString TRestAnalysisTree::GetStringWithObservableNames() {
    Int_t nEntries = GetEntries();
    auto branches = GetListOfBranches();
    std::string branchNames = "";
    for (int i = 0; i < branches->GetEntries(); i++) {
        if (i > 0) branchNames += " ";
        branchNames += (string)branches->At(i)->GetName();
    }

    return (TString)branchNames;
}

///////////////////////////////////////////////
/// \brief It writes TRestAnalysisTree as TTree, in order to migrate files to pure ROOT users
///
Int_t TRestAnalysisTree::WriteAsTTree(const char* name, Int_t option, Int_t bufsize) {
    TTree* tree = new TTree();

    char* dest = (char*)tree + sizeof(TObject);
    char* from = (char*)this + sizeof(TObject);

    constexpr size_t n = sizeof(TTree) - sizeof(TObject);

    // copy the data of the created TTree to a temporary location
    char* temp = (char*)malloc(n);
    memcpy(temp, dest, n);

    // copy the data of this TRestAnalysisTree to TTree, and call the new TTree to write
    memcpy(dest, from, n);
    int result = tree->Write(name, option, bufsize);

    // restore the data of the created TTree, then we can free it!
    memcpy(dest, temp, n);
    free(temp);
    delete tree;

    return result;
}

/// <summary>
/// Add a series output file like TChain.
/// </summary>
/// <param name="file"> The input file that contains another AnalysisTree with same run id </param>
/// <returns></returns>
Bool_t TRestAnalysisTree::AddChainFile(string _file) {
    auto file = std::unique_ptr<TFile>{TFile::Open(_file.c_str(), "update")};
    if (!file->IsOpen()) {
        warning << "TRestAnalysisTree::AddChainFile(): failed to open file " << _file << endl;
        return false;
    }

    TRestAnalysisTree* tree = (TRestAnalysisTree*)file->Get("AnalysisTree");
    if (tree != nullptr && tree->GetEntry(0)) {
        if (tree->GetEntry(0) > 0) {
            if (tree->GetRunOrigin() == this->GetRunOrigin()) {
                // this is a valid tree
                delete tree;

                if (fChain == nullptr) {
                    fChain = new TChain("AnalysisTree", "Chained AnalysisTree");
                    fChain->Add(this->GetCurrentFile()->GetName());
                }
                return fChain->Add(_file.c_str()) == 1;
            }
            warning << "TRestAnalysisTree::AddChainFile(): invalid file, AnalysisTree in file has different "
                       "run id!"
                    << endl;
        }
        warning << "TRestAnalysisTree::AddChainFile(): invalid file, AnalysisTree in file is empty!" << endl;
        delete tree;
    }
    warning << "TRestAnalysisTree::AddChainFile(): invalid file, file does not contain an AnalysisTree!"
            << endl;

    return false;
}

/// <summary>
/// Overrides TTree::GetTree(), to get the actual tree used in case of chain operation(fCurrentTree !=
/// nullptr)
/// </summary>
/// <returns></returns>
TTree* TRestAnalysisTree::GetTree() const {
    if (fChain == nullptr) {
        return TTree::GetTree();
    }
    return (TTree*)fChain;
}

/// <summary>
/// Overrides TTree::LoadTree(), to set current tree according to the given entry number, in case of chain
/// operation
/// </summary>
/// <param name="entry"></param>
/// <returns></returns>
Long64_t TRestAnalysisTree::LoadTree(Long64_t entry) {
    if (fChain == nullptr) {
        return TTree::LoadTree(entry);
    } else {
        return fChain->LoadTree(entry);
    }

    return -2;
}

Long64_t TRestAnalysisTree::GetEntries() const {
    if (fChain == nullptr) {
        return TTree::GetEntries();
    }
    return fChain->GetEntries();
}

Long64_t TRestAnalysisTree::GetEntries(const char* sel) {
    if (fChain == nullptr) {
        return TTree::GetEntries(sel);
    }
    return fChain->GetEntries(sel);
}

TRestAnalysisTree::~TRestAnalysisTree() {
    if (fSubEventTag != nullptr) delete fSubEventTag;
}
