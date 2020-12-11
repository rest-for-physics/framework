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

#include "TBranchElement.h"
#include "TH1F.h"
#include "TLeaf.h"
#include "TLeafB.h"
#include "TLeafC.h"
#include "TLeafD.h"
#include "TLeafElement.h"
#include "TLeafF.h"
#include "TLeafI.h"
#include "TLeafL.h"
#include "TLeafObject.h"
#include "TLeafS.h"
#include "TObjArray.h"
#include "TRestEventProcess.h"
#include "TRestMetadata.h"
#include "TStreamerInfo.h"

using namespace std;

ClassImp(TRestAnalysisTree) using namespace std;
//______________________________________________________________________________
TRestAnalysisTree::TRestAnalysisTree() : TTree() {
    SetName("AnalysisTree");
    SetTitle("REST Analysis tree");

    Initialize();
}

TRestAnalysisTree::TRestAnalysisTree(TTree* tree) : TTree() {
    SetName("AnalysisTree");
    SetTitle("Linked to " + (TString)tree->GetName());

    Initialize();

    fROOTTree = tree;
    tree->GetEntry(0);

    TObjArray* lfs = tree->GetListOfLeaves();
    for (int i = 0; i < lfs->GetLast() + 1; i++) {
        TLeaf* lf = (TLeaf*)lfs->UncheckedAt(i);
        any obs;
        ReadLeafValueToObservable(lf, obs);
        obs.name = lf->GetName();
        SetObservableValueSafe(-1, obs);
    }

    fConnected = true;
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

    fNObservables = 0;
    fROOTTree = NULL;

    fObservableIdMap.clear();
    fObservableDescriptions.clear();
    fObservableNames.clear();
    fObservables.clear();
    fConnected = false;
    fBranchesCreated = false;
}

Int_t TRestAnalysisTree::GetObservableID(TString obsName) {
    if (!ObservableExists(obsName)) return -1;
    return fObservableIdMap[obsName];
}

Bool_t TRestAnalysisTree::ObservableExists(TString obsName) {
    if (fObservableIdMap.size() == 0 && fObservableNames.size() > 0) MakeObservableIdMap();
    return fObservableIdMap.count(obsName) > 0;
}

void TRestAnalysisTree::ConnectBranches() {
    if (fConnected) {
        cout << "REST Warning: AnalysisTree observable already connected!" << endl;
        return;
    }

    // connect basic event branches
    TBranch* br1 = GetBranch("eventID");
    TBranch* br2 = GetBranch("subEventID");
    TBranch* br3 = GetBranch("timeStamp");
    TBranch* br4 = GetBranch("subEventTag");
    TBranch* br5 = GetBranch("runOrigin");
    TBranch* br6 = GetBranch("subRunOrigin");

    if (br1 && br2 && br3 && br4 && br5 && br6) {
        br1->SetAddress(&fEventID);
        br2->SetAddress(&fSubEventID);
        br3->SetAddress(&fTimeStamp);
        br4->SetAddress(&fSubEventTag);
        br5->SetAddress(&fRunOrigin);
        br6->SetAddress(&fSubRunOrigin);
    } else {
        cout << "REST ERROR: AnalysisTree lacks event branches! Call CreateBranches() first!" << endl;
        return;
    }

    // create observables
    InitObservables();

    TTree::GetEntry(0);

    for (int i = 0; i < GetNumberOfObservables(); i++) {
        TBranch* branch = GetBranch(fObservableNames[i]);
        if (branch != NULL) {
            if (branch->GetAddress() != NULL) {
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
    fConnected = true;
}

void TRestAnalysisTree::CreateBranches() {
    if (fBranchesCreated) {
        cout << "REST Warning: AnalysisTree branches already created!" << endl;
        return;
    }

    Branch("runOrigin", &fRunOrigin);
    Branch("subRunOrigin", &fSubRunOrigin);
    Branch("eventID", &fEventID);
    Branch("subEventID", &fSubEventID);
    Branch("timeStamp", &fTimeStamp);
    Branch("subEventTag", fSubEventTag);

    for (int n = 0; n < GetNumberOfObservables(); n++) {
        TString typeName = fObservableTypes[n];
        TString brName = fObservableNames[n];
        char* ref = fObservables[n].address;

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

    fBranchesCreated = true;
    fConnected = true;
}

void TRestAnalysisTree::InitObservables() {
    fObservables = std::vector<any>(GetNumberOfObservables());
    for (int i = 0; i < GetNumberOfObservables(); i++) {
        fObservables[i] = REST_Reflection::WrapType((string)fObservableTypes[i]);
        fObservables[i].name = fObservableNames[i];
    }
    MakeObservableIdMap();
}

void TRestAnalysisTree::MakeObservableIdMap() {
    fObservableIdMap.clear();
    for (int i = 0; i < fObservableNames.size(); i++) {
        fObservableIdMap[fObservableNames[i]] = i;
    }
}

void TRestAnalysisTree::ReadLeafValueToObservable(TLeaf* lf, any& obs) {
    if (lf == NULL || lf->GetLen() == 0) return;

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
    if (obs.dt != 0) {
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

Int_t TRestAnalysisTree::AddObservable(TString observableName, TString observableType, TString description) {
    if (fBranchesCreated) {
        return -1;
    }
    Double_t x = 0;
    if (GetObservableID(observableName) == -1) {
        any ptr = REST_Reflection::Assembly((string)observableType);
        ptr.name = observableName;
        if (!ptr.IsZombie()) {
            fObservableNames.push_back(observableName);
            fObservableIdMap[observableName] = fObservableNames.size() - 1;
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
        return GetObservableID(observableName);
    }

    return fNObservables - 1;
}

void TRestAnalysisTree::PrintObservables() {
    if (fNObservables != fObservables.size()) {
        InitObservables();
    }

    cout.precision(15);
    std::cout << "Entry : " << fReadEntry << std::endl;
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
    string obsVal = fObservables[n].ToString();
    int lengthRemaining = Console::GetWidth() - 14 - 45 - 13;

    std::cout << "Observable : " << ToString(fObservableNames[n], 45)
              << "    Value : " << ToString(obsVal, lengthRemaining) << std::endl;
}

Int_t TRestAnalysisTree::GetEntry(Long64_t entry, Int_t getall) {
    if (!fConnected) {
        if (!fBranchesCreated && GetListOfBranches()->GetEntriesFast() > 0 &&
            (fNObservables == 0 || fNObservables != fObservables.size())) {
            // the object is just retrieved from root file, we connect the branches
            fBranchesCreated = true;
            ConnectBranches();
        } else {
            // the tree is initialized but not filled yet. There is no entries to get
            return 0;
        }
    }

    if (fROOTTree != NULL) {
        // if it is connected to an external tree, we get entry on that
        int result = fROOTTree->GetEntry(entry, getall);

        TObjArray* lfs = fROOTTree->GetListOfLeaves();
        if (fObservables.size() != lfs->GetLast() + 1) {
            cout << "Warning: external TTree may have changed!" << endl;
            for (int i = 0; i < lfs->GetLast() + 1; i++) {
                TLeaf* lf = (TLeaf*)lfs->UncheckedAt(i);
                any obs;
                ReadLeafValueToObservable(lf, obs);
                obs.name = lf->GetName();
                SetObservableValueSafe(-1, obs);
            }
        } else {
            for (int i = 0; i < lfs->GetLast() + 1; i++) {
                TLeaf* lf = (TLeaf*)lfs->UncheckedAt(i);
                ReadLeafValueToObservable(lf, fObservables[i]);
            }
        }

        return result;
    }

    return TTree::GetEntry(entry, getall);
}

void TRestAnalysisTree::SetEventInfo(TRestEvent* evt) {
    if (evt != NULL) {
        fEventID = evt->GetID();
        fSubEventID = evt->GetSubID();
        fTimeStamp = evt->GetTimeStamp().AsDouble();
        *fSubEventTag = evt->GetSubEventTag();
        fRunOrigin = evt->GetRunOrigin();
        fSubRunOrigin = evt->GetSubRunOrigin();
    }
}

Int_t TRestAnalysisTree::Fill() {
    if (!fBranchesCreated) {
        if (GetListOfBranches()->GetEntriesFast() > 0) {
            // Branches are already created but the flag is false
            // This means the tree is just retrieved from root file
            // We need to connect observables for it
            fBranchesCreated = true;
            if (!fConnected) {
                ConnectBranches();
            }
        } else {
            CreateBranches();
        }
    }

    if (fROOTTree != NULL) {
        cout << "Warning: this is a read-only tree!" << endl;
        return -1;
    }

    return TTree::Fill();
}

void TRestAnalysisTree::SetObservableValueSafe(Int_t id, any obs) {
    if (id == -1) {
        // this means we want to find observable id by its name
        id = GetObservableID(obs.name);
        // if not found, we have a chance to create observable
        if (id == -1) {
            if (!fBranchesCreated) {
                id = AddObservable(obs.name, obs.type);
            } else {
                cout << "error: SetObservableValue(): branches already created!" << endl;
            }
        }
    } else if (id == fObservables.size()) {
        // this means we want to add observable directly
        if (!fBranchesCreated) {
            id = AddObservable(obs.name, obs.type);
        } else {
            cout << "error: SetObservableValue(): branches already created!" << endl;
        }
    }
    if (id != -1) {
        if (!fBranchesCreated) {
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
        }
        obs >> fObservables[id];
    }
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
/// \brief It returns a string containning all the observables that exist in the analysis tree.
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

TRestAnalysisTree::~TRestAnalysisTree() {
    if (fSubEventTag != NULL) delete fSubEventTag;
}
