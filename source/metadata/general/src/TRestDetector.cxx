///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetector.cxx
///
///             G4 class description
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________
#include "TClass.h"
#include "TInterpreter.h"
#include "TMethod.h"
#include "TMethodCall.h"

#include "TRestDetector.h"
#include "TRestDriftVolume.h"
#include "TRestReadout.h"
#include "TRestGainMap.h"
using namespace std;

//______________________________________________________________________________
TRestDetector::TRestDetector() {
    // TRestDetector default constructor
}

//______________________________________________________________________________
TRestDetector::~TRestDetector() {
    // TRestDetector destructor
}

TRestDetector* TRestDetector::instantiate(string name) {
    TRestDetector* detector = NULL;
    if (name != "") {
        TClass* c = TClass::GetClass(("TRestDetector" + name).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            detector = (TRestDetector*)c->New();
        } else {
            warning << "unrecognized TRestDataBase implementation: \"" << name << "\"" << endl;
            detector = new TRestDetector();
        }
    } else {
        detector = new TRestDetector();
    }
    if (gDetector != NULL) delete gDetector;
    gDetector = detector;
    return detector;
}

string TRestDetector::GetMediumName() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetName();
    }
    return "";
}
Double_t TRestDetector::GetPressure() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetPressure();
    }
    return 0;
}
Double_t TRestDetector::GetTemperature() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetTemperature();
    }
    return 0;
}
Double_t TRestDetector::GetWvalue() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetWvalue();
    }
    return 0;
}
Double_t TRestDetector::GetDriftVelocity() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetDriftVelocity();
    }
    return 0;
}
Double_t TRestDetector::GetElectronLifeTime() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetElectronLifeTime();
    }
    return 0;
}
Double_t TRestDetector::GetLongitudinalDiffusion() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetLongitudinalDiffusion();
    }
    return 0;
}
Double_t TRestDetector::GetTransversalDiffusion() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetTransversalDiffusion();
    }
    return 0;
}

string TRestDetector::GetReadoutName() {
    if (fReadout != NULL) {
        return fReadout->GetName();
    }
    return "";
}
Int_t TRestDetector::GetNReadoutModules() {
    if (fReadout != NULL) {
        return fReadout->GetNumberOfModules();
    }
    return 0;
}
Int_t TRestDetector::GetNReadoutChannels() {
    if (fReadout != NULL) {
        return fReadout->GetNumberOfChannels();
    }
    return 0;
}
Double_t TRestDetector::GetReadoutVoltage(int id) { return fAmplificationVoltage; }
Double_t TRestDetector::GetReadoutGain(int id) {
    if (fGain != NULL && fGain->fChannelGain.count(id) > 0) {
        return fGain->fChannelGain[id];
    }
    return 0;
}
TVector2 TRestDetector::GetReadoutPosition(int id) {
    if (fReadout != NULL) {
        double x = fReadout->GetX(id);
        double y = fReadout->GetY(id);

        if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
            Int_t planeID, readoutChannel = -1, readoutModule;
            fReadout->GetPlaneModuleChannel(id, planeID, readoutModule, readoutChannel);

            if (readoutChannel != -1) {
                TRestReadoutPlane* plane = fReadout->GetReadoutPlaneWithID(planeID);
                TRestReadoutModule* mod = plane->GetModuleByID(readoutModule);
                if (TMath::IsNaN(x)) {
                    x = mod->GetPhysicalCoordinates(
                               TVector2(mod->GetModuleSizeX() / 2, mod->GetModuleSizeY() / 2))
                            .X();
                } else if (TMath::IsNaN(y)) {
                    y = mod->GetPhysicalCoordinates(
                               TVector2(mod->GetModuleSizeX() / 2, mod->GetModuleSizeY() / 2))
                            .Y();
                }
            }
        }
        return TVector2(fReadout->GetX(id), fReadout->GetY(id));
    }
    return TVector2();
}
enum REST_ReadoutType { unknown = -1, none = 1, X = 2, Y = 3, Z = 5, U = 7, V = 11, W = 13 };
Int_t TRestDetector::GetReadoutType(int id) {
    if (fReadout != NULL) {
        double x = fReadout->GetX(id);
        double y = fReadout->GetY(id);

        return (!TMath::IsNaN(x) ? X : none) * (!TMath::IsNaN(y) ? Y : none);
    }
    return unknown;
}

void TRestDetector::RegisterMetadata(TRestMetadata* ptr) {
    if (ptr != NULL) {
        if (ptr->InheritsFrom("TRestDriftVolume")) {
            fDetectorMedium = (TRestDriftVolume*)ptr;
        } else if (ptr->InheritsFrom("TRestReadout")) {
            fReadout = (TRestReadout*)ptr;
        } else if (ptr->InheritsFrom("TRestGainMap")) {
            fGain = (TRestGainMap*)ptr;
        }
    }
}

void TRestDetector::RegisterString(string str) {
    if (str.find(".aqs") != -1) {
        ReadFileNameFEMINOS(str);
    }
}

void TRestDetector::Print() {}

void TRestDetector::ReadFileNameFEMINOS(string fName) {
    string fullName = fName;

    unsigned int startPos = fullName.find_last_of("/") + 1;
    unsigned int length = fullName.length();
    string name = fullName.substr(startPos, length - startPos);

    // fRunNumber = StringToInteger(name.substr(1, 5));

    unsigned int pos = name.find("_") + 1;
    unsigned int len = name.find("_Vm") - pos;
    // fRunTag = (TString)name.substr(pos, len);

    pos = name.find("Vm_") + 3;
    len = name.find("_Vd") - pos;
    fAmplificationVoltage = StringToDouble(name.substr(pos, len));

    pos = name.find("Vd_") + 3;
    len = name.find("_Pr") - pos;
    fDriftVoltage = StringToDouble(name.substr(pos, len));

    pos = name.find("Pr_") + 3;
    len = name.find("_Gain") - pos;
    if (fDetectorMedium != NULL) fDetectorMedium->SetPressure(StringToDouble(name.substr(pos, len)));

    pos = name.find("Gain_") + 5;
    len = name.find("_Shape") - pos;
    fDAQDynamicRange = atof(name.substr(pos, len).c_str());

    pos = name.find("Shape_") + 6;
    len = name.find("_Clock") - pos;
    fDAQShapingTime = atof(name.substr(pos, len).c_str());

    pos = name.find("Clock_") + 6;
    len = name.find("-") - pos;
    TString _sampling = name.substr(pos, len).c_str();

    TString samplingReduced = _sampling(2, _sampling.Length());
    fDAQSamplingTime = (Double_t)strtol(samplingReduced.Data(), NULL, 16) / 100.;  // This is only for AGET

    pos = name.find("-") + 1;
    len = name.find(".aqs") - pos;
    // fSubRunNumber = StringToInteger(name.substr(pos, len));
}

TRestDetector* gDetector = NULL;
MakeGlobal(TRestDetector, gDetector);