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
#include "TObjArray.h"
#include "TRestEventProcess.h"
#include "TRestMetadata.h"
#include "TStreamerInfo.h"

using namespace std;

ClassImp(TRestAnalysisTree) using namespace std;
//______________________________________________________________________________
TRestAnalysisTree::TRestAnalysisTree() : TTree() {
    SetName("TRestAnalysisTree");
    SetTitle("Analysis tree");

    Initialize();
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

    fObservableDescriptions.clear();
    fObservableNames.clear();
    fObservableMemory.clear();
    fConnected = false;
    fBranchesCreated = false;
}

void TRestAnalysisTree::CopyObservableList(TRestAnalysisTree* from, string prefix) {
    if (from != NULL && !fConnected) {
        vector<any> tmpobsval;
        vector<TString> tmpobsname;
        vector<TString> tmpobsdes;
        vector<TString> tmptypes;

        for (int i = 0; i < from->GetNumberOfObservables(); i++) {
            tmpobsval.push_back(REST_Reflection::Assembly((string)from->GetObservableType(i)));
            tmpobsname.push_back(prefix + from->GetObservableName(i));
            tmpobsdes.push_back(from->GetObservableDescription(i));
            tmptypes.push_back(from->GetObservableType(i));
        }

        fObservableMemory.insert(fObservableMemory.begin(), tmpobsval.begin(), tmpobsval.end());
        fObservableNames.insert(fObservableNames.begin(), tmpobsname.begin(), tmpobsname.end());
        fObservableDescriptions.insert(fObservableDescriptions.begin(), tmpobsdes.begin(), tmpobsdes.end());
        fObservableTypes.insert(fObservableTypes.begin(), tmptypes.begin(), tmptypes.end());
        fNObservables += from->GetNumberOfObservables();

        // fConnected = true;
    } else {
        cout << "REST ERROR: AnalysisTree Observables is already connected!" << endl;
    }
}

void TRestAnalysisTree::ConnectEventBranches() {
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

void TRestAnalysisTree::ConnectObservables() {
    if (!fConnected) {
        fObservableMemory = std::vector<any>(GetNumberOfObservables());
        for (int i = 0; i < GetNumberOfObservables(); i++) {
            fObservableMemory[i] = REST_Reflection::WrapType((string)fObservableTypes[i]);
        }

        TTree::GetEntry(0);

        for (int i = 0; i < GetNumberOfObservables(); i++) {
            TBranch* branch = GetBranch(fObservableNames[i]);
            if (branch != NULL) {
                if (branch->GetAddress() != NULL) {
                    if ((string)branch->ClassName() != "TBranch") {
                        // for TBranchElement the saved address is char**
                        fObservableMemory[i].address = *(char**)branch->GetAddress();
                    } else {
                        // for TBranch the saved address is char*
                        fObservableMemory[i].address = branch->GetAddress();
                    }
                } else {
                    fObservableMemory[i].Assembly();
                    branch->SetAddress(fObservableMemory[i].address);
                }
            }
        }
        fConnected = true;
    } else {
        cout << "REST ERROR: AnalysisTree Observables is already connected!" << endl;
    }
}

Int_t TRestAnalysisTree::AddObservable(TString observableName, TString observableType, TString description) {
    if (fBranchesCreated) {
        return -1;
    }
    Double_t x = 0;
    if (GetObservableID(observableName) == -1) {
        any ptr = REST_Reflection::Assembly((string)observableType);
        if (!ptr.IsZombie()) {
            fObservableNames.push_back(observableName);
            fObservableDescriptions.push_back(description);
            fObservableMemory.push_back(ptr);
            fObservableTypes.push_back(observableType);

            fNObservables++;
        } else {
            cout << "Error adding observable \"" << observableName << "\" with type \"" << observableType
                 << "\", type not found!" << endl;
            return -1;
        }
    } else {
        // cout << "observable \"" << observableName << "\" has already been created! skipping" << endl;
        return GetObservableID(observableName);
    }

    return fNObservables - 1;
}

Int_t TRestAnalysisTree::AddObservable(TString objName, TRestMetadata* meta, TString description) {
    if (fBranchesCreated) {
        return -1;
    }
    any ptr = any(meta).GetDataMember((string)objName);
    if (ptr.IsZombie()) return -1;

    TString brName = meta->GetName() + (TString) "." + ptr.name;
    // cout << ele->GetTypeName() << " " << ele->GetName() << " " <<
    // ele->ClassName() << endl;
    if (GetObservableID(brName) == -1) {
        if (ptr.name[0] != 'f') {
            fObservableNames.push_back(brName);
            fObservableDescriptions.push_back(description);
            fObservableMemory.push_back(ptr);
            fObservableTypes.push_back(ptr.type);
            fNObservables++;
        } else {
            cout << "Data member \"" << objName << "\" not found in class: \"" << meta->ClassName() << "\""
                 << endl;
            return -1;
        }
    } else {
        // cout << "observable \"" << observableName << "\" has already been
        // created! skipping" << endl;
        return GetObservableID(brName);
    }
    return fNObservables - 1;
}

void TRestAnalysisTree::PrintObservables(TRestEventProcess* proc, int NObs) {
    // if (!isConnected() || !fBranchesCreated) {
    //    if (fNObservables > 0 &&
    //        fObservableMemory.size() == 0)  // the object may be just retrieved from root file
    //    {
    //        ConnectEventBranches();
    //        ConnectObservables();
    //        GetEntry(0);
    //    }
    //}

    if (!fConnected && !fBranchesCreated) {
        cout << "Error in PrintObservables, please connect or create the tree branch first!" << endl;
        return;
    }

    cout.precision(15);
    if (proc == NULL) {
        std::cout << "Run origin : " << GetRunOrigin() << std::endl;
        std::cout << "Event ID : " << GetEventID() << std::endl;
        std::cout << "Event Time : " << GetTimeStamp() << std::endl;
        std::cout << "Event Tag : " << GetSubEventTag() << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    } else {
        std::cout << "---- AnalysisTree Observable for process: " << proc->GetName() << " ----" << std::endl;
    }

    for (int n = 0; n < GetNumberOfObservables() && n < NObs; n++) {
        if (proc == NULL || (proc != NULL && ((string)fObservableNames[n]).find(proc->GetName()) == 0))
            PrintObservable(n);
    }

    std::cout << std::endl;
    cout.precision(6);
}

// print the Nth observable in the observal list
void TRestAnalysisTree::PrintObservable(int n) {
    if (n < 0 || n >= fNObservables) {
        return;
    }
    if (fConnected || fBranchesCreated) {
        std::cout << "Observable : " << ToString(fObservableNames[n], 30)
                  << "    Value : " << fObservableMemory[n] << std::endl;

    } else {
        std::cout << "Observable : " << ToString(fObservableNames[n], 30) << "    Value : ???" << std::endl;
    }
}

Int_t TRestAnalysisTree::GetEntry(Long64_t entry, Int_t getall) {
    if (!fConnected && !fBranchesCreated) {
        ConnectEventBranches();
        ConnectObservables();
    } else if (fNObservables != fObservableMemory.size()) {
        // the object is just retrieved from root file, we connect the branches
        ConnectEventBranches();
        ConnectObservables();
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

Int_t TRestAnalysisTree::Fill(TRestEvent* evt) {
    if (evt != NULL) {
        SetEventInfo(evt);
    }

    if (!fBranchesCreated) {
        CreateBranches();
    }

    return TTree::Fill();
}

void TRestAnalysisTree::CreateEventBranches() {
    if (fBranchesCreated) {
        return;
    }

    Branch("runOrigin", &fRunOrigin);
    Branch("subRunOrigin", &fSubRunOrigin);
    Branch("eventID", &fEventID);
    Branch("subEventID", &fSubEventID);
    Branch("timeStamp", &fTimeStamp);
    Branch("subEventTag", fSubEventTag);
}

void TRestAnalysisTree::CreateObservableBranches() {
    if (fBranchesCreated) {
        return;
    }

    for (int n = 0; n < GetNumberOfObservables(); n++) {
        TString typeName = fObservableTypes[n];
        TString brName = fObservableNames[n];
        char* ref = fObservableMemory[n].address;

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

    // Branch(fObservableNames[n], fObservableMemory[n]);
}

void TRestAnalysisTree::CreateBranches() {
    if (!fBranchesCreated) {
        if (GetListOfBranches()->GetEntriesFast() > 0) {
            fBranchesCreated = true;
            return;
        }
        CreateEventBranches();
        CreateObservableBranches();
        fBranchesCreated = true;
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

TRestAnalysisTree::~TRestAnalysisTree() {}
