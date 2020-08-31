///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorImpl.cxx
///
///             G4 class description
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________
#include "TRestDetectorImpl.h"

#include "TClass.h"
#include "TInterpreter.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TRestDriftVolume.h"
#include "TRestGainMap.h"
#include "TRestReadout.h"
using namespace std;

//______________________________________________________________________________
TRestDetectorImpl::TRestDetectorImpl() {
    fDetectorName = "REST default detector";

    fDetectorMedium = NULL;
    fReadout = NULL;
    fGain = NULL;
    // TRestDetectorImpl default constructor
}

//______________________________________________________________________________
TRestDetectorImpl::~TRestDetectorImpl() {
    // TRestDetectorImpl destructor
}

string TRestDetectorImpl::GetMediumName() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetName();
    }
    return "";
}
Double_t TRestDetectorImpl::GetPressure() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetPressure();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetTemperature() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetTemperature();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetWvalue() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetWvalue();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetDriftVelocity() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetDriftVelocity();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetElectronLifeTime() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetElectronLifeTime();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetLongitudinalDiffusion() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetLongitudinalDiffusion();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetTransversalDiffusion() {
    if (fDetectorMedium != NULL) {
        return fDetectorMedium->GetTransversalDiffusion();
    }
    return 0;
}

Double_t TRestDetectorImpl::GetTPCBottomZ() {
    if (fReadout != NULL) {
        if (fReadout->GetNumberOfReadoutPlanes() > 1) {
            double minz = 1e9;
            for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
                TRestReadoutPlane* plane = &(*fReadout)[p];
                double zz = plane->GetPosition().Z();
                if (zz < minz) minz = zz;
            }
            return minz;
        } else if (fReadout->GetNumberOfReadoutPlanes() == 1) {
            TRestReadoutPlane* plane = &(*fReadout)[0];
            if (plane->GetPlaneVector().Z() < 0) {
                return plane->GetPosition().Z() - plane->GetTotalDriftDistance();
            } else {
                return plane->GetPosition().Z();
            }
        }

        return 0;
    }
    return 0;
}
Double_t TRestDetectorImpl::GetTPCTopZ() {
    if (fReadout != NULL) {
        if (fReadout->GetNumberOfReadoutPlanes() > 1) {
            double maxz = -1e9;
            for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
                TRestReadoutPlane* plane = &(*fReadout)[p];
                double zz = plane->GetPosition().Z();
                if (zz > maxz) maxz = zz;
            }
            return maxz;
        } else if (fReadout->GetNumberOfReadoutPlanes() == 1) {
            TRestReadoutPlane* plane = &(*fReadout)[0];
            if (plane->GetPlaneVector().Z() < 0) {
                return plane->GetPosition().Z();
            } else {
                return plane->GetPosition().Z() + plane->GetTotalDriftDistance();
            }
        }

        return 0;
    }
    return 0;
}
Double_t TRestDetectorImpl::GetDriftDistance(TVector3 pos) {
    if (fReadout != NULL) {
        for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
            TRestReadoutPlane* plane = &(*fReadout)[p];
            if (plane->isZInsideDriftVolume(pos)) {
                return plane->GetDistanceTo(pos);
            }
        }
    }
    return 0;
}
Double_t TRestDetectorImpl::GetAmplificationDistance(TVector3 pos) { return fAmplificationDistance; }

string TRestDetectorImpl::GetReadoutName() {
    if (fReadout != NULL) {
        return fReadout->GetName();
    }
    return "";
}
Int_t TRestDetectorImpl::GetNReadoutModules() {
    if (fReadout != NULL) {
        return fReadout->GetNumberOfModules();
    }
    return 0;
}
Int_t TRestDetectorImpl::GetNReadoutChannels() {
    if (fReadout != NULL) {
        return fReadout->GetNumberOfChannels();
    }
    return 0;
}
Double_t TRestDetectorImpl::GetReadoutVoltage(int id) { return fAmplificationVoltage; }
Double_t TRestDetectorImpl::GetReadoutGain(int id) {
    if (fGain != NULL && fGain->fChannelGain.count(id) > 0) {
        return fGain->fChannelGain[id];
    }
    return 0;
}
TVector2 TRestDetectorImpl::GetReadoutPosition(int id) {
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
Int_t TRestDetectorImpl::GetReadoutType(int id) {
    if (fReadout != NULL) {
        double x = fReadout->GetX(id);
        double y = fReadout->GetY(id);

        return (!TMath::IsNaN(x) ? X : none) * (!TMath::IsNaN(y) ? Y : none);
    }
    return unknown;
}

void TRestDetectorImpl::RegisterMetadata(TObject* ptr) {
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

void TRestDetectorImpl::RegisterString(string str) {
    if (str.find(".aqs") != -1) {
        ReadFileNameFEMINOS(str);
    }
}

void TRestDetectorImpl::Print() {}

void TRestDetectorImpl::ReadFileNameFEMINOS(string fName) {
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

MakeGlobal(TRestDetectorImpl, gDetector, 2);