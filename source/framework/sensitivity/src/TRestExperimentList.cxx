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
/// 2022-December: First implementation of TRestExperimentList
/// Javier Galan
///
/// \class TRestExperimentList
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestExperimentList.h"

ClassImp(TRestExperimentList);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestExperimentList::TRestExperimentList() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestExperimentList::~TRestExperimentList() {}

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
/// \param name The name of the specific metadata.
///
TRestExperimentList::TRestExperimentList(const char* cfgFileName, const std::string& name)
    : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestExperimentList::Initialize() { SetSectionName(this->ClassName()); }

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestExperimentList::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    if (!fExperimentsFile.empty()) {
        TRestTools::ReadASCIITable(fExperimentsFile, fExperimentsTable);

        for (auto& row : fExperimentsTable)
            for (auto& el : row) el = REST_StringHelper::ReplaceMathematicalExpressions(el);

        if (fExperimentsTable.empty()) {
            RESTError << "TRestExperimentList::InitFromConfigFile. The experiments table is empty!"
                      << RESTendl;
            return;
        }

        Int_t nTableColumns = fExperimentsTable[0].size();

        int cont = 0;
        TRestComponent* comp = (TRestComponent*)this->InstantiateChildMetadata(cont, "Component");
        while (comp != nullptr) {
            if (ToLower(comp->GetNature()) == "background")
                fBackground = comp;
            else if (ToLower(comp->GetNature()) == "signal")
                fSignal = comp;
            else
                RESTWarning << "TRestExperimentList::InitFromConfigFile. Unknown component!" << RESTendl;

            cont++;
            comp = (TRestComponent*)this->InstantiateChildMetadata(cont, "Component");
        }

        Int_t nExpectedColumns = 3;
        if (fSignal) nExpectedColumns--;
        if (fBackground) nExpectedColumns--;
        if (fExposureTime > 0) nExpectedColumns--;

        if (nExpectedColumns == 0) {
            RESTError << "TRestExperimentList::InitFromConfigFile. At least one free parameter required! "
                         "(Exposure/Background/Signal)"
                      << RESTendl;
            return;
        }

        if (nExpectedColumns != nTableColumns) {
            std::string expectedColumns = "";
            if (fExposureTime == 0) expectedColumns += "exposure";
            if (!fSignal) {
                if (fExposureTime == 0) expectedColumns += "/";
                expectedColumns += "signal";
            }
            if (!fBackground) {
                if (fExposureTime == 0 || !fSignal) expectedColumns += "/";
                expectedColumns += "background";
            }

            RESTError << "TRestExperimentList::InitFromConfigFile. Number of expected columns does not match "
                         "the number of table columns"
                      << RESTendl;
            RESTError << "Number of table columns : " << nTableColumns << RESTendl;
            RESTError << "Number of expected columns : " << nExpectedColumns << RESTendl;
            RESTError << "Expected columns : " << expectedColumns << RESTendl;
            return;
        }
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestExperimentList::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Number of experiments loaded: " << fExperiments.size() << RESTendl;

    RESTMetadata << "----" << RESTendl;
}
