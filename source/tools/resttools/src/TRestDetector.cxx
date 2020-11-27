#include "TRestDetector.h"

#include "TClass.h"
#include "TMap.h"
#include "TObjString.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"

Int_t TRestDetector::GetRunNumber() const { return fRunNumber; }

void TRestDetector::SetRunNumber(const Int_t& run) {
    fRunNumber = run;
    SetParameter("runNumber", ToString(run));
}

string TRestDetector::GetDetectorName() const { return fDetectorName; }

void TRestDetector::SetDetectorName(const string& name) {
    fDetectorName = name;
    SetParameter("detectorName", name);
}

string TRestDetector::GetParameter(const string& paraname) const {
    if (this->count(paraname) > 0) return this->at(paraname);
    return PARAMETER_NOT_FOUND_STR;
}

void TRestDetector::Print() const {
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestDetector content" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << " Detector name : " << fDetectorName << endl;
    cout << " Detector class : " << REST_ARGS["gDetector"] << endl;
    cout << " Run number : " << fRunNumber << endl;
    cout << " Number of parameters stored: " << (*this).size() << endl;
    cout << " --------------------------------------------" << endl;
    for (auto iter : *this) {
        cout << iter.first << " : " << iter.second << endl;
    }
    cout << " --------------------------------------------" << endl;
}

void TRestDetector::WriteFile(TFile* f) const {
    TObjArray* arr = new TObjArray();

    vector<TNamed*> items;
    for (auto iter : *this) {
        TNamed* item = new TNamed(iter.first.c_str(), iter.second.c_str());
        arr->Add(item);
        items.push_back(item);
    }

    f->WriteTObject(arr, "DetectorParameters", "Overwrite");

    for (auto item : items) {
        delete item;
    }
    delete arr;
}

void TRestDetector::ReadFile(TFile* f) {
    TObjArray* arr = (TObjArray*)f->Get("DetectorParameters");
    if (arr != NULL) {
        int i = 0;
        TNamed* item = NULL;
        while ((item = (TNamed*)arr->At(i)) != NULL) {
            SetParameter(item->GetName(), item->GetTitle());
            i++;
        }
    } else {
        warning << "TRestDetector::ReadFile: no detector parameters stored in file" << endl;
    }
}

void TRestDetector::SetParameter(const string& paraname, const string& paraval) {
    any member = any(this, REST_ARGS["gDetector"]).GetDataMember(ParameterNameToDataMemberName(paraname));
    if (!member.IsZombie()) {
        member.ParseString(paraval);
    }

    (*this)[paraname] = paraval;
}