///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawDAQMetadata inherited from TRestMetadata
///
///             Load metadata of the decoding and return a readout channel for a
///             given DAQ channel.
///
///             now 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________

#include "TRestRawDAQMetadata.h"
using namespace std;

ClassImp(TRestRawDAQMetadata)
    //______________________________________________________________________________
    TRestRawDAQMetadata::TRestRawDAQMetadata() {
    Initialize();
}

TRestRawDAQMetadata::TRestRawDAQMetadata(char* cfgFileName) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName);

    SetScriptsBuffer();
    SetParFromPedBuffer();
}

void TRestRawDAQMetadata::Initialize() { SetSectionName(this->ClassName()); }

//______________________________________________________________________________
TRestRawDAQMetadata::~TRestRawDAQMetadata() { cout << "Deleting TRestRawDAQMetadata" << endl; }

//______________________________________________________________________________
void TRestRawDAQMetadata::InitFromConfigFile() {
    // string daqString;

    fNamePedScript = GetParameter("pedScript");
    if (fNamePedScript == "") {
        cout << "Pedestal script " << endl;
    }

    fNameRunScript = GetParameter("runScript");
    if (fNameRunScript == "") {
        cout << "Run script " << endl;
    }

    fElectronicsType = GetParameter("electronics");
    if (fElectronicsType == "") {
        cout << "electronic type not found " << endl;
    }
}

void TRestRawDAQMetadata::PrintMetadata() {
    cout << endl;
    cout << "====================================" << endl;
    cout << "DAQ : " << GetTitle() << endl;
    cout << "Pedestal script : " << fNamePedScript.Data() << endl;
    cout << "Run script : " << fNameRunScript.Data() << endl;
    cout << "Electronics type : " << fElectronicsType.Data() << endl;
    cout << "Gain : " << GetGain() << endl;
    cout << "Shapping time : " << GetShappingTime() << endl;
    cout << "====================================" << endl;

    cout << endl;
}

void TRestRawDAQMetadata::SetScriptsBuffer() {
    TString folder = REST_PATH;
    folder.Append("data/acquisition/");

    TString fName;

    fName = folder + fNamePedScript;

    ifstream file(fName);
    if (!file) {
        cout << __PRETTY_FUNCTION__ << " ERROR:FILE " << fName << " not found " << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        fPedBuffer.push_back(line);
    }

    file.close();

    fName = folder + fNameRunScript;

    ifstream file2(fName);
    if (!file2) {
        cout << __PRETTY_FUNCTION__ << " ERROR:FILE " << fName << " not found " << endl;
        return;
    }

    while (getline(file2, line)) {
        fRunBuffer.push_back(line);
    }

    file2.close();
}

void TRestRawDAQMetadata::PrintRunScript() {
    for (unsigned int i = 0; i < fRunBuffer.size(); i++) cout << fRunBuffer[i].Data() << endl;
}

void TRestRawDAQMetadata::PrintPedScript() {
    for (unsigned int i = 0; i < fPedBuffer.size(); i++) cout << fPedBuffer[i].Data() << endl;
}

void TRestRawDAQMetadata::SetParFromPedBuffer() {
    for (unsigned int i = 0; i < fPedBuffer.size(); i++) {
        if (fPedBuffer[i].Contains("aget * gain * "))
            fGain = GetValFromString("aget * gain * ", fPedBuffer[i]);

        if (fPedBuffer[i].Contains("aget * time "))
            fShappingTime = GetValFromString("aget * time ", fPedBuffer[i]);

        if (fPedBuffer[i].Contains("after * gain * "))
            fGain = GetValFromString("after * gain * ", fPedBuffer[i]);

        if (fPedBuffer[i].Contains("after * time "))
            fShappingTime = GetValFromString("after * time ", fPedBuffer[i]);
    }
}

UInt_t TRestRawDAQMetadata::GetValFromString(TString var, TString line) {
    unsigned int val;

    unsigned int varSize = var.Sizeof();
    unsigned int lineSize = line.Sizeof();

    // cout<<varSize<<"  "<<lineSize<<endl;

    TString diff(line(varSize - 1, lineSize - 1));

    cout << diff.Data() << endl;

    sscanf(diff.Data(), "0x%x", &val);

    return val;
}
