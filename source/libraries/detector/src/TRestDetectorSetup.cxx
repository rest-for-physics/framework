///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSetup.cxx
///
///             G4 class description
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorSetup.h"
#include "TRestManager.h"
#include "TRestRun.h"
using namespace std;

ClassImp(TRestDetectorSetup)
    //______________________________________________________________________________
    TRestDetectorSetup::TRestDetectorSetup()
    : TRestMetadata() {
    // TRestDetectorSetup default constructor
    Initialize();
}

//______________________________________________________________________________
TRestDetectorSetup::TRestDetectorSetup(char* cfgFileName, string name) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    PrintMetadata();
}

//______________________________________________________________________________
TRestDetectorSetup::~TRestDetectorSetup() {
    // TRestDetectorSetup destructor
}

void TRestDetectorSetup::Initialize() {
    SetSectionName(this->ClassName());

    fRunNumber = 0;
    fSubRunNumber = 0;
    fRunTag = "";

    fMeshVoltage = 0;
    fDriftField = 0;
    fDetectorPressure = 0;

    fElectronicsGain = "";
    fSamplingTime = "";
    fShapingTime = "";

    fSamplingInMicroSec = 0;
}

//______________________________________________________________________________
void TRestDetectorSetup::InitFromConfigFile() {
    this->Initialize();

    // Initialize the metadata members from a configfile
    fRunNumber = StringToInteger(GetParameter("runNumber"));
    fSubRunNumber = StringToInteger(GetParameter("subRunNumber"));
    if (fHostmgr != NULL && fHostmgr->GetRunInfo() != NULL) {
        TRestRun* r = fHostmgr->GetRunInfo();
        if (r->GetInputFileNumber() > 0) InitFromFileName(r->GetInputFileName(0));
    }
    // TOBE implemented if needed
}

void TRestDetectorSetup::InitFromFileName(TString fName) {
    string fullName = (string)fName;

    unsigned int startPos = fullName.find_last_of("/") + 1;
    unsigned int length = fullName.length();
    string name = fullName.substr(startPos, length - startPos);

    fRunNumber = StringToInteger(name.substr(1, 5));

    unsigned int pos = name.find("_") + 1;
    unsigned int len = name.find("_Vm") - pos;
    fRunTag = (TString)name.substr(pos, len);

    pos = name.find("Vm_") + 3;
    len = name.find("_Vd") - pos;
    fMeshVoltage = StringToDouble(name.substr(pos, len));

    pos = name.find("Vd_") + 3;
    len = name.find("_Pr") - pos;
    fDriftField = StringToDouble(name.substr(pos, len));

    pos = name.find("Pr_") + 3;
    len = name.find("_Gain") - pos;
    fDetectorPressure = StringToDouble(name.substr(pos, len));

    pos = name.find("Gain_") + 5;
    len = name.find("_Shape") - pos;
    fElectronicsGain = name.substr(pos, len);

    pos = name.find("Shape_") + 6;
    len = name.find("_Clock") - pos;
    fShapingTime = name.substr(pos, len);

    pos = name.find("Clock_") + 6;
    len = name.find("-") - pos;
    fSamplingTime = name.substr(pos, len);

    TString samplingReduced = fSamplingTime(2, fSamplingTime.Length());
    fSamplingInMicroSec = (Double_t)strtol(samplingReduced.Data(), NULL, 16) / 100.;  // This is only for AGET

    pos = name.find("-") + 1;
    len = name.find(".aqs") - pos;
    fSubRunNumber = StringToInteger(name.substr(pos, len));
}

void TRestDetectorSetup::PrintMetadata() {
    cout << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestDetectorSetup content" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << " Run number : " << fRunNumber << endl;
    cout << " Run sub number : " << fSubRunNumber << endl;
    cout << " Run tag : " << fRunTag << endl;
    cout << " --------------------------------------------" << endl;
    cout << " Mesh voltage : " << fMeshVoltage << " V" << endl;
    cout << " Drift field : " << fDriftField << " V/cm/bar" << endl;
    cout << " Detector pressure : " << fDetectorPressure << " bar" << endl;
    cout << " --------------------------------------------" << endl;
    cout << " Electronics gain register : " << fElectronicsGain << endl;
    cout << " Shaping time register : " << fShapingTime << endl;
    cout << " Sampling rate register : " << fSamplingTime << endl;
    cout << " Sampling rate : " << fSamplingInMicroSec << " us " << endl;
    cout << " --------------------------------------------" << endl;
}
