#include "TRestDetector.h"

#include "TClass.h"
#include "TMap.h"
#include "TObjString.h"
#include "TRestStringHelper.h"
// This is an interface class

void TRestDetector::Print() {
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestDetector content" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << " Detector name : " << fDetectorName << endl;
    cout << " Run number : " << fRunNumber << endl;
    cout << " Number of parameters stored: " << fParameterMap.size() << endl;
    cout << " --------------------------------------------" << endl;
}

void TRestDetector::PrintParameterMap() {
    for (auto iter : fParameterMap) {
        cout << iter.first << " : " << iter.second << endl;
    }
    cout << " --------------------------------------------" << endl;
}

void TRestDetector::WriteFile(TFile* f) {
    TObjArray* arr = new TObjArray();

    vector<TNamed*> items;
    for (auto iter : fParameterMap) {
        TNamed* item = new TNamed(iter.first.c_str(), iter.second.c_str());
        arr->Add(item);
        items.push_back(item);
    }

    f->WriteObject(arr, "DetectorParameters");

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

void TRestDetector::SetParameter(string paraname, string paraval) { 
    if (paraname == "fRunNumber") {
        fRunNumber = StringToInteger(paraval);
    }
    else if (paraname == "fDetectorName") {
        fDetectorName = paraval;
    }
    
    fParameterMap[paraname] = paraval; 

}