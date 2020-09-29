///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorTPC.cxx
///
///             G4 class description
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________
#include "TRestDetectorTPC.h"

#include "TClass.h"
#include "TInterpreter.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TRestDetectorReadout.h"
#include "TRestDriftVolume.h"
#include "TRestGainMap.h"
using namespace std;

//______________________________________________________________________________
TRestDetectorTPC::TRestDetectorTPC() {
    fDetectorName = "REST default detector";

    fDetectorMedium = NULL;
    fReadout = NULL;
    fGain = NULL;
    // TRestDetectorTPC default constructor
}

//______________________________________________________________________________
TRestDetectorTPC::~TRestDetectorTPC() {
    // TRestDetectorTPC destructor
}

string TRestDetectorTPC::GetMediumName() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetName();
    }
    return "";
}
Double_t TRestDetectorTPC::GetPressure() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetPressure();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetTemperature() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetTemperature();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetWvalue() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetWvalue();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetDriftVelocity() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetDriftVelocity();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetElectronLifeTime() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetElectronLifeTime();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetLongitudinalDiffusion() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetLongitudinalDiffusion();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetTransversalDiffusion() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetTransversalDiffusion();
    }
    return NAN;
}

Double_t TRestDetectorTPC::GetTPCBottomZ() {
    if (fReadout != NULL) {
        if (fReadout->GetNumberOfReadoutPlanes() > 1) {
            double minz = 1e9;
            for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
                TRestDetectorReadoutPlane* plane = &(*fReadout)[p];
                double zz = plane->GetPosition().Z();
                if (zz < minz) minz = zz;
            }
            return minz;
        } else if (fReadout->GetNumberOfReadoutPlanes() == 1) {
            TRestDetectorReadoutPlane* plane = &(*fReadout)[0];
            if (plane->GetPlaneVector().Z() < 0) {
                return plane->GetPosition().Z() - plane->GetTotalDriftDistance();
            } else {
                return plane->GetPosition().Z();
            }
        }

        return NAN;
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetTPCTopZ() {
    if (fReadout != NULL) {
        if (fReadout->GetNumberOfReadoutPlanes() > 1) {
            double maxz = -1e9;
            for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
                TRestDetectorReadoutPlane* plane = &(*fReadout)[p];
                double zz = plane->GetPosition().Z();
                if (zz > maxz) maxz = zz;
            }
            return maxz;
        } else if (fReadout->GetNumberOfReadoutPlanes() == 1) {
            TRestDetectorReadoutPlane* plane = &(*fReadout)[0];
            if (plane->GetPlaneVector().Z() < 0) {
                return plane->GetPosition().Z();
            } else {
                return plane->GetPosition().Z() + plane->GetTotalDriftDistance();
            }
        }

        return NAN;
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetDriftDistance(TVector3 pos) {
    if (fReadout != NULL) {
        for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
            TRestDetectorReadoutPlane* plane = &(*fReadout)[p];
            if (plane->isZInsideDriftVolume(pos)) {
                return plane->GetDistanceTo(pos);
            }
        }
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetAmplificationDistance(TVector3 pos) { return fAmplificationDistance; }

string TRestDetectorTPC::GetReadoutName() {
    if (fReadout != NULL) {
        return fReadout->GetName();
    }
    return "";
}
Int_t TRestDetectorTPC::GetNReadoutModules() {
    if (fReadout != NULL) {
        return fReadout->GetNumberOfModules();
    }
    return NAN;
}
Int_t TRestDetectorTPC::GetNReadoutChannels() {
    if (fReadout != NULL) {
        return fReadout->GetNumberOfChannels();
    }
    return NAN;
}
Double_t TRestDetectorTPC::GetReadoutVoltage(int id) { return fAmplificationVoltage; }
Double_t TRestDetectorTPC::GetReadoutGain(int id) {
    if (fGain != NULL && fGain->fChannelGain.count(id) > 0) {
        return fGain->fChannelGain[id];
    }
    return NAN;
}
TVector3 TRestDetectorTPC::GetReadoutPosition(int id) {
    if (fReadout != NULL) {
        double x = fReadout->GetX(id);
        double y = fReadout->GetY(id);
        double z = 0;

        if (TMath::IsNaN(x) || TMath::IsNaN(y)) {
            Int_t planeID, readoutChannel = -1, readoutModule;
            fReadout->GetPlaneModuleChannel(id, planeID, readoutModule, readoutChannel);

            if (readoutChannel != -1) {
                TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlaneWithID(planeID);
                TRestDetectorReadoutModule* mod = plane->GetModuleByID(readoutModule);

                z = plane->GetPosition().Z();
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
        return TVector3(x, y, z);
    }
    return TVector3(NAN, NAN, NAN);
}
TVector3 TRestDetectorTPC::GetReadoutDirection(int id) {
    if (fReadout != NULL) {
        Int_t planeID, readoutChannel = -1, readoutModule;
        fReadout->GetPlaneModuleChannel(id, planeID, readoutModule, readoutChannel);

        if (readoutChannel != -1) {
            TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlaneWithID(planeID);
            return plane->GetPlaneVector();
        }
    }
    return TVector3(NAN, NAN, NAN);
}
enum REST_ReadoutType { unknown = -1, none = 1, X = 2, Y = 3, Z = 5, U = 7, V = 11, W = 13 };
Int_t TRestDetectorTPC::GetReadoutType(int id) {
    if (fReadout != NULL) {
        double x = fReadout->GetX(id);
        double y = fReadout->GetY(id);

        return (!TMath::IsNaN(x) ? X : none) * (!TMath::IsNaN(y) ? Y : none);
    }
    return unknown;
}

void TRestDetectorTPC::SetDriftMedium(string mediumname) {}
void TRestDetectorTPC::SetDriftField(Double_t df) {
    if (fDetectorMedium != NULL) {
        fDetectorMedium->SetElectricField(df);
    }
}
void TRestDetectorTPC::SetPressure(Double_t p) {
    if (fDetectorMedium != NULL) {
        fDetectorMedium->SetPressure(p);
    }
}
void TRestDetectorTPC::SetDAQSamplingTime(Double_t st) { fDAQSamplingTime = st; }
void TRestDetectorTPC::SetElectronLifeTime(Double_t elt) {
    if (fDetectorMedium != NULL) {
        fDetectorMedium->SetElectronLifeTime(elt);
    }
}

void TRestDetectorTPC::RegisterMetadata(TObject* ptr) {
    if (ptr != NULL) {
        if (ptr->InheritsFrom("TRestDriftVolume")) {
            fDetectorMedium = (TRestDriftVolume*)ptr;
        } else if (ptr->InheritsFrom("TRestDetectorReadout")) {
            fReadout = (TRestDetectorReadout*)ptr;
        } else if (ptr->InheritsFrom("TRestGainMap")) {
            fGain = (TRestGainMap*)ptr;
        }
    }
}

void TRestDetectorTPC::RegisterString(string str) {
    if (str.find(".aqs") != -1) {
        ReadFileNameFEMINOS(str);
    }
}

void TRestDetectorTPC::Print() {}

void TRestDetectorTPC::ReadFileNameFEMINOS(string fName) {
    string fullName = fName;

    unsigned int startPos = fullName.find_last_of("/") + 1;
    unsigned int length = fullName.length();
    string name = fullName.substr(startPos, length - startPos);

    fRunNumber = StringToInteger(name.substr(1, 5));

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

string TRestDetectorTPC::GetParameter(string paraname) {
    string result = PARAMETER_NOT_FOUND_STR;

    switch (ToHash(paraname.c_str())) {
        // basic types
        case ToHash("driftVelocity"): {
            result = ToString(any(GetDriftVelocity()));
            break;
        }
        case ToHash("pressure"): {
            result = ToString(any(GetPressure()));
            break;
        }
        default: { break; }
    }

    if (result == "nan" || result == "(nan,nan)" || result == "(nan,nan,nan)") {
        return PARAMETER_NOT_FOUND_STR;
    }
    return result;
}

void TRestDetectorTPC::SetParameter(string paraname, string paraval) {}

MakeGlobal(TRestDetectorTPC, gDetector, 2);
