/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2020 GIFNA/TREX (University of Zaragoza)                *
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
/// TODO: fill documentation
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-September: Implementation.
///
/// \class      TRestGeant4VetoAnalysisProcess
/// \author     Luis Obis
///
/// <hr>
///

#include "TRestGeant4VetoAnalysisProcess.h"
using namespace std;

ClassImp(TRestGeant4VetoAnalysisProcess);

TRestGeant4VetoAnalysisProcess::TRestGeant4VetoAnalysisProcess() { Initialize(); }

TRestGeant4VetoAnalysisProcess::TRestGeant4VetoAnalysisProcess(char* cfgFileName) {
    Initialize();
    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGeant4VetoAnalysisProcess::~TRestGeant4VetoAnalysisProcess() { delete fOutputG4Event; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestGeant4VetoAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestGeant4VetoAnalysisProcess::Initialize() {
    fG4Metadata = NULL;
    SetSectionName(this->ClassName());

    fInputG4Event = NULL;
    fOutputG4Event = new TRestGeant4Event();
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
/// correspondig TRestGeant4VetoAnalysisProcess section inside the RML.
///
void TRestGeant4VetoAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestGeant4VetoAnalysisProcess::InitProcess() {
    fG4Metadata = GetMetadata<TRestGeant4Metadata>();

    // CAREFUL THIS METHOD IS CALLED TWICE!
    // we need to reset these variables to zero
    fVetoVolumeIds.clear();
    fVetoGroupVolumeNames.clear();

    // get "veto" volumes
    for (unsigned int i = 0; i < fG4Metadata->GetNumberOfActiveVolumes(); i++) {
        string volume_name = (string)fG4Metadata->GetActiveVolumeName(i);
        volume_name = clean_string(volume_name);
        if (volume_name.find(clean_string(fVetoKeyword)) != string::npos) {
            fVetoVolumeIds.push_back(i);
        }
    }

    // veto groups (fill fVetoGroupVolumeNames)
    for (unsigned int i = 0; i < fVetoGroupKeywords.size(); i++) {
        string veto_group_keyword = clean_string(fVetoGroupKeywords[i]);
        fVetoGroupVolumeNames[veto_group_keyword] = std::vector<string>{};
        for (int& id : fVetoVolumeIds) {
            string volume_name = (string)fG4Metadata->GetActiveVolumeName(id);
            volume_name = clean_string(volume_name);
            if (volume_name.find(veto_group_keyword) != string::npos) {
                fVetoGroupVolumeNames[veto_group_keyword].push_back(
                    (string)fG4Metadata->GetActiveVolumeName(id));
            }
        }
    }

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4VetoAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fInputG4Event = (TRestGeant4Event*)evInput;
    *fOutputG4Event = *((TRestGeant4Event*)evInput);

    std::map<string, Double_t> volume_energy_map;

    for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
        int id = fVetoVolumeIds[i];
        string volume_name = (string)fG4Metadata->GetActiveVolumeName(id);

        Double_t energy = fOutputG4Event->GetEnergyDepositedInVolume(id);
        volume_energy_map[volume_name] = energy;
    }

    Double_t energy_veto_max = 0;
    for (const auto& pair : volume_energy_map) {
        Double_t veto_energy = pair.second;
        SetObservableValue(pair.first + "VolumeEDep", veto_energy);
        if (veto_energy > energy_veto_max) {
            energy_veto_max = veto_energy;
        };
    }
    SetObservableValue("vetoAllEVetoMax", energy_veto_max);

    // veto groups
    for (const auto& pair : fVetoGroupVolumeNames) {
        Double_t energy_veto_max_group = 0;
        for (unsigned int i = 0; i < pair.second.size(); i++) {
            string volume_name = pair.second[i];
            Double_t veto_energy = volume_energy_map[volume_name];
            if (veto_energy > energy_veto_max_group) {
                energy_veto_max_group = veto_energy;
            };
        }
        // convert to Upper + lower case (VetoGroupTopEVetoMax, ...)
        string group_name;
        for (auto it = pair.first.cbegin(); it != pair.first.cend(); ++it) {
            if (it == pair.first.cbegin()) {
                group_name += std::toupper(*it);
            } else {
                group_name += std::tolower(*it);
            }
        }
        SetObservableValue("vetoGroup" + group_name + "EVetoMax", energy_veto_max_group);
    }

    //

    for (const auto& quenching_factor : fQuenchingFactors) {
        string quenching_factor_string = std::to_string(quenching_factor);
        // replace "." in string by "_" because its gives very strange problems
        quenching_factor_string =
            quenching_factor_string.replace(quenching_factor_string.find("."), sizeof(".") - 1, "_");
        volume_energy_map.clear();
        for (int i = 0; i < fOutputG4Event->GetNumberOfTracks(); i++) {
            auto track = fOutputG4Event->GetTrack(i);
            string particle_name = (string)track->GetParticleName();
            for (const auto& id : fVetoVolumeIds) {
                string volume_name = (string)fG4Metadata->GetActiveVolumeName(id);

                if (particle_name == "e-" || particle_name == "e+" || particle_name == "gamma") {
                    // no quenching factor
                    volume_energy_map[volume_name] += track->GetEnergyInVolume(id);
                } else {
                    // apply quenching factor
                    volume_energy_map[volume_name] += quenching_factor * track->GetEnergyInVolume(id);
                }
            }
        }

        Double_t energy_veto_max = 0;
        for (const auto& pair : volume_energy_map) {
            Double_t veto_energy = pair.second;
            SetObservableValue(pair.first + "VolumeEDep" + "Qf" + quenching_factor_string, veto_energy);
            if (veto_energy > energy_veto_max) {
                energy_veto_max = veto_energy;
            };
        }
        SetObservableValue(string("vetoAllEVetoMax") + "Qf" + quenching_factor_string, energy_veto_max);

        // veto groups
        for (const auto& pair : fVetoGroupVolumeNames) {
            Double_t energy_veto_max_group = 0;
            for (unsigned int i = 0; i < pair.second.size(); i++) {
                string volume_name = pair.second[i];
                Double_t veto_energy = volume_energy_map[volume_name];
                if (veto_energy > energy_veto_max_group) {
                    energy_veto_max_group = veto_energy;
                };
            }
            // convert to Upper + lower case (VetoGroupTopEVetoMax, ...)
            string group_name;
            for (auto it = pair.first.cbegin(); it != pair.first.cend(); ++it) {
                if (it == pair.first.cbegin()) {
                    group_name += std::toupper(*it);
                } else {
                    group_name += std::tolower(*it);
                }
            }
            SetObservableValue("vetoGroup" + group_name + "EVetoMax" + "Qf" + quenching_factor_string,
                               energy_veto_max_group);
        }
    }

    return fOutputG4Event;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestGeant4VetoAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestGeant4VetoAnalysisProcess metadata section
///
void TRestGeant4VetoAnalysisProcess::InitFromConfigFile() {
    // word to identify active volume as veto (default = "veto" e.g. "vetoTop")
    string veto_keyword = GetParameter("vetoKeyword", "veto");
    fVetoKeyword = clean_string(veto_keyword);
    // comma separated tags: "top, bottom, ..."
    string veto_group_keywords = GetParameter("vetoGroupKeywords", "");
    stringstream ss(veto_group_keywords);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        fVetoGroupKeywords.push_back(clean_string(substr));
    }

    // comma separated quenching factors: "0.15, 1.00, ..."
    string quenching_factors = GetParameter("vetoQuenchingFactors", "-1");
    stringstream ss_qf(quenching_factors);
    while (ss_qf.good()) {
        string substr;
        getline(ss_qf, substr, ',');
        substr = clean_string(substr);
        Float_t quenching_factor = (Float_t)std::atof(substr.c_str());
        if (quenching_factor > 1 || quenching_factor < 0) {
            cout << "ERROR: quenching factor must be between 0 and 1" << endl;
            continue;
        }
        fQuenchingFactors.push_back(quenching_factor);
    }
}
