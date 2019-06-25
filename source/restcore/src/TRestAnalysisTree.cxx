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
#include "TRestEventProcess.h"
#include "TRestMetadata.h"
#include "TStreamerInfo.h"

#include "TObjArray.h"

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
        cout << "Connecting observables..." << endl;

        for (int i = 0; i < GetNumberOfObservables(); i++) {
            TBranch* branch = GetBranch(fObservableNames[i]);
            if (branch != NULL) {
                // cout << fObservableNames[i];
                if (branch->GetAddress() != NULL) {
                    fObservableMemory[i].address = *(char**)branch->GetAddress();
                    // cout << " --> ";
                } else {
                    fObservableMemory[i].Assembly();
                    branch->SetAddress(fObservableMemory[i].address);
                    // cout << " ==> ";
                }
                // cout << static_cast<const void*>(branch->GetAddress()) << endl;
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
    REST_Reflection::AnyPtr_t ptr = REST_Reflection::GetDataMember(meta, (string)objName);
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
                  << "    Value : " << fObservableMemory[n].ToString() << std::endl;

    } else {
        std::cout << "Observable : " << ToString(fObservableNames[n], 30) << "    Value : ???" << std::endl;
    }
}

Int_t TRestAnalysisTree::GetEntry(Long64_t entry, Int_t getall) {
    if (!fConnected && !fBranchesCreated) {
        if (fNObservables > 0 &&
            fObservableMemory.size() ==
                0)  // the object is just retrieved from root file, we connect the branches
        {
            ConnectEventBranches();
            ConnectObservables();
        }
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

Int_t TRestAnalysisTree::FillEvent(TRestEvent* evt) {
    SetEventInfo(evt);

    if (!fBranchesCreated) {
        CreateEventBranches();
        CreateObservableBranches();
    }

    return this->Fill();
}

void TRestAnalysisTree::CreateEventBranches() {
    Branch("runOrigin", &fRunOrigin);
    Branch("subRunOrigin", &fSubRunOrigin);
    Branch("eventID", &fEventID);
    Branch("subEventID", &fSubEventID);
    Branch("timeStamp", &fTimeStamp);
    Branch("subEventTag", fSubEventTag);
}

void TRestAnalysisTree::CreateObservableBranches() {
    if (fBranchesCreated) {
        cout << "REST ERROR : Branches have been already created" << endl;
        exit(1);
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
        } else {
            this->Branch(brName, typeName, ref);
        }
    }

    // Branch(fObservableNames[n], fObservableMemory[n]);

    fBranchesCreated = true;
}

//______________________________________________________________________________
TRestAnalysisTree::~TRestAnalysisTree() {}
