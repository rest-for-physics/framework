///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4toHitsProcess.cxx
///
///
///             Simple process to convert a TRestG4Event class into a
///    		    TRestHitsEvent, that is, we just "extract" the hits
///    information
///             Date : oct/2016
///             Author : I. G. Irastorza
///
///_______________________________________________________________________________

#include "TRestG4toHitsProcess.h"
using namespace std;

ClassImp(TRestG4toHitsProcess)
    //______________________________________________________________________________
    TRestG4toHitsProcess::TRestG4toHitsProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestG4toHitsProcess::TRestG4toHitsProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestG4toHitsProcess::~TRestG4toHitsProcess() {
    delete fG4Event;
    delete fHitsEvent;
}

//______________________________________________________________________________
void TRestG4toHitsProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    cout << "G4 to hits metadata not found. Loading default values" << endl;
}

//______________________________________________________________________________
void TRestG4toHitsProcess::Initialize() {
    SetSectionName(this->ClassName());

    fG4Event = new TRestG4Event();
    fHitsEvent = new TRestHitsEvent();

    fOutputEvent = fHitsEvent;
    fInputEvent = fG4Event;
}

void TRestG4toHitsProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestG4toHitsProcess::InitProcess() {
    //    TRestEventProcess::ReadObservables();

    fG4Metadata = (TRestG4Metadata*)GetGeant4Metadata();

    for (unsigned int n = 0; n < fVolumeSelection.size(); n++) {
        if (fG4Metadata->GetActiveVolumeID(fVolumeSelection[n]) >= 0)
            fVolumeId.push_back(fG4Metadata->GetActiveVolumeID(fVolumeSelection[n]));
        else if (GetVerboseLevel() >= REST_Warning)
            cout << "TRestG4ToHitsProcess. volume name : " << fVolumeSelection[n]
                 << " not found and will not be added." << endl;
    }

    ////// Debug output //////
    debug << "Active volumes available in TRestG4Metadata" << endl;
    debug << "-------------------------------------------" << endl;
    for (int n = 0; n < fG4Metadata->GetNumberOfActiveVolumes(); n++)
        debug << "Volume id : " << n << " name : " << fG4Metadata->GetActiveVolumeName(n) << endl;
    debug << endl;

    debug << "TRestG4HitsProcess volumes enabled in RML : ";
    debug << "-------------------------------------------" << endl;
    if (fVolumeSelection.size() == 0)
        debug << "all" << endl;
    else {
        for (int n = 0; n < fVolumeSelection.size(); n++) {
            debug << "" << endl;
            debug << " - " << fVolumeSelection[n] << endl;
        }
        debug << " " << endl;
    }

    if (fVolumeSelection.size() > 0 && fVolumeSelection.size() != fVolumeId.size())
        warning << "TRestG4toHitsProcess. Not all volumes were properly identified!" << endl;

    if (fVolumeId.size() > 0) {
        debug << "TRestG4HitsProcess volumes identified : ";
        debug << "---------------------------------------" << endl;
        if (fVolumeSelection.size() == 0)
            debug << "all" << endl;
        else
            for (int n = 0; n < fVolumeSelection.size(); n++) {
                debug << "" << endl;
                debug << " - " << fVolumeSelection[n] << endl;
            }
        debug << " " << endl;
    }
}

//______________________________________________________________________________
void TRestG4toHitsProcess::BeginOfEventProcess() { fHitsEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestG4toHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fG4Event = (TRestG4Event*)evInput;

    fHitsEvent->SetRunOrigin(fG4Event->GetRunOrigin());
    fHitsEvent->SetSubRunOrigin(fG4Event->GetSubRunOrigin());
    fHitsEvent->SetID(fG4Event->GetID());
    fHitsEvent->SetSubID(fG4Event->GetSubID());
    fHitsEvent->SetSubEventTag(fG4Event->GetSubEventTag());
    fHitsEvent->SetTimeStamp(fG4Event->GetTimeStamp());
    fHitsEvent->SetState(fG4Event->isOk());

    Int_t i, j;
    Double_t x, y, z, E;

    for (i = 0; i < fG4Event->GetNumberOfTracks(); i++) {
        for (j = 0; j < fG4Event->GetTrack(i)->GetNumberOfHits(); j++) {
            // read x,y,z and E of every hit in the G4 event
            x = fG4Event->GetTrack(i)->GetHits()->fX[j];
            y = fG4Event->GetTrack(i)->GetHits()->fY[j];
            z = fG4Event->GetTrack(i)->GetHits()->fZ[j];
            E = fG4Event->GetTrack(i)->GetHits()->fEnergy[j];

            Bool_t addHit = true;
            if (fVolumeId.size() > 0) {
                addHit = false;
                for (unsigned int n = 0; n < fVolumeId.size(); n++)
                    if (fG4Event->GetTrack(i)->GetHits()->GetVolumeId(j) == fVolumeId[n]) addHit = true;
            }

            // and write them in the output hits event:
            if (addHit && E > 0) fHitsEvent->AddHit(x, y, z, E);
        }
    }

    if (this->GetVerboseLevel() >= REST_Debug) {
        cout << "TRestG4toHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestG4toHitsProcess. Hits total energy : " << fHitsEvent->GetEnergy() << endl;
    }

    return fHitsEvent;
}

//______________________________________________________________________________
void TRestG4toHitsProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestG4toHitsProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestG4toHitsProcess::InitFromConfigFile() {
    size_t position = 0;
    string addVolumeDefinition;

    while ((addVolumeDefinition = GetKEYDefinition("addVolume", position)) != "")
        fVolumeSelection.push_back(GetFieldValue("name", addVolumeDefinition));
}
