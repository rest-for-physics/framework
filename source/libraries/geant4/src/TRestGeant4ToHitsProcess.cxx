/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// This process allows to select the GDML geometry volumes (defined in
/// TRestGeant4Metadata) that will be transferred to the TRestDetectorHitsEvent by
/// using the `<addVolume` key inside the process definition.
///
/// The following example shows how to include the process into
/// `TRestProcessRunner` RML definition. In this particular example we
/// extract hits from `gas` and `vessel` volumes defined in the geometry.
/// Any other hits will be ignored.
///
/// \code
///
/// <addProcess type="TRestGeant4ToHitsProcess" name="g4ToHits" value="ON">
///     <addVolume name="gas" />
///     <addVolume name="vessel" />
/// </addProcess>
/// \endcode
///
/// If no volumes are defined using the `<addVolume` key, **all volumes will
/// be active**, and all hits will be transferred to the TRestDetectorHitsEvent output.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-October First implementation of TRestGeant4Event to TRestDetectorHitsEvent
///              Igor Irastorza
///
/// 2017-October: Added the possibility to extract hits only from selected geometrical volumes
///               Javier Galan
///
/// \class      TRestGeant4ToHitsProcess
/// \author     Igor Irastorza
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestGeant4ToHitsProcess.h"
using namespace std;

ClassImp(TRestGeant4ToHitsProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestGeant4ToHitsProcess::TRestGeant4ToHitsProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestGeant4ToHitsProcess::TRestGeant4ToHitsProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGeant4ToHitsProcess::~TRestGeant4ToHitsProcess() { delete fHitsEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestGeant4ToHitsProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    cout << "G4 to hits metadata not found. Loading default values" << endl;
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestGeant4ToHitsProcess::Initialize() {
    SetSectionName(this->ClassName());

    fG4Event = NULL;
    fHitsEvent = new TRestDetectorHitsEvent();
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestGeant4ToHitsProcess section inside the RML.
///
void TRestGeant4ToHitsProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization. This process accesses the information inside
/// TRestGeant4Metadata to identify the geometry volume ids associated to the hits.
///
void TRestGeant4ToHitsProcess::InitProcess() {
    fG4Metadata = GetMetadata<TRestGeant4Metadata>();

    for (unsigned int n = 0; n < fVolumeSelection.size(); n++) {
        if (fG4Metadata->GetActiveVolumeID(fVolumeSelection[n]) >= 0)
            fVolumeId.push_back(fG4Metadata->GetActiveVolumeID(fVolumeSelection[n]));
        else if (GetVerboseLevel() >= REST_Warning)
            cout << "TRestGeant4ToHitsProcess. volume name : " << fVolumeSelection[n]
                 << " not found and will not be added." << endl;
    }

    /* {{{ Debug output */
    debug << "Active volumes available in TRestGeant4Metadata" << endl;
    debug << "-------------------------------------------" << endl;
    for (int n = 0; n < fG4Metadata->GetNumberOfActiveVolumes(); n++)
        debug << "Volume id : " << n << " name : " << fG4Metadata->GetActiveVolumeName(n) << endl;
    debug << endl;

    debug << "TRestGeant4HitsProcess volumes enabled in RML : ";
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
        warning << "TRestGeant4ToHitsProcess. Not all volumes were properly identified!" << endl;

    if (fVolumeId.size() > 0) {
        debug << "TRestGeant4HitsProcess volumes identified : ";
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
    /* }}} */
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4ToHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fG4Event = (TRestGeant4Event*)evInput;

    if (this->GetVerboseLevel() >= REST_Extreme) {
        cout << "------ TRestGeant4ToHitsProcess --- Printing Input Event --- START ----" << endl;
        fG4Event->PrintEvent();
        cout << "------ TRestGeant4ToHitsProcess --- Printing Input Event ---- END ----" << endl;
        GetChar();
    }

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
        cout << "TRestGeant4ToHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestGeant4ToHitsProcess. Hits total energy : " << fHitsEvent->GetEnergy() << endl;
    }

    return fHitsEvent;
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestGeant4ToHitsProcess metadata section
///
void TRestGeant4ToHitsProcess::InitFromConfigFile() {
    size_t position = 0;
    string addVolumeDefinition;

    while ((addVolumeDefinition = GetKEYDefinition("addVolume", position)) != "")
        fVolumeSelection.push_back(GetFieldValue("name", addVolumeDefinition));
}

///////////////////////////////////////////////
/// \brief It prints on screen relevant data members from this class
///
void TRestGeant4ToHitsProcess::PrintMetadata() {
    BeginPrintProcess();

    for (unsigned int n = 0; n < fVolumeSelection.size(); n++)
        metadata << "Volume added : " << fVolumeSelection[n] << endl;

    EndPrintProcess();
}
