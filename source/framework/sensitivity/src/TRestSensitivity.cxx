/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
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
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

/////////////////////////////////////////////////////////////////////////
/// Documentation TOBE written
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-December: First implementation of TRestSensitivity
/// Javier Galan
///
/// \class TRestSensitivity
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include <TRestExperimentList.h>
#include <TRestSensitivity.h>

ClassImp(TRestSensitivity);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestSensitivity::TRestSensitivity() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestSensitivity::~TRestSensitivity() {}

/////////////////////////////////////////////
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
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestAxionMagneticField section inside the RML.
///
TRestSensitivity::TRestSensitivity(const char* cfgFileName, const std::string& name)
    : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestSensitivity::Initialize() { SetSectionName(this->ClassName()); }

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestSensitivity::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    int cont = 0;
    TRestMetadata* metadata = (TRestMetadata*)this->InstantiateChildMetadata(cont, "Experiment");
    while (metadata != nullptr) {
        cont++;
        if (metadata->InheritsFrom("TRestExperimentList")) {
            TRestExperimentList* experimentsList = (TRestExperimentList*)metadata;
            std::vector<TRestExperiment*> exList = experimentsList->GetExperiments();
            fExperiments.insert(fExperiments.end(), exList.begin(), exList.end());
        } else if (metadata->InheritsFrom("TRestExperiment")) {
            fExperiments.push_back((TRestExperiment*)metadata);
        }

        metadata = (TRestMetadata*)this->InstantiateChildMetadata(cont, "Experiment");
    }

    Initialize();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestSensitivity::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "----" << RESTendl;
}
