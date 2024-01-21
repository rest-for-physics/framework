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

    /*
auto ele = GetElement("variable");
while (ele != nullptr) {
    std::string name = GetParameter("name", ele, "");
    TVector2 v = Get2DVectorParameterWithUnits("range", ele, TVector2(-1, -1));
    Int_t bins = StringToInteger(GetParameter("bins", ele, "0"));

    if (name.empty() || (v.X() == -1 && v.Y() == -1) || bins == 0) {
        RESTWarning << "TRestComponentFormula::fVariable. Problem with definition." << RESTendl;
        RESTWarning << "Name: " << name << " range: (" << v.X() << ", " << v.Y() << ") bins: " << bins
                    << RESTendl;
    } else {
        fVariables.push_back(name);
        fRanges.push_back(v);
        fNbins.push_back(bins);
    }

    ele = GetNextElement(ele);
}
    */

    if (!fExperimentsFile.empty()) {
        TRestTools::ReadASCIITable(fExperimentsFile, fExperimentsTable);

        TRestTools::PrintTable(fExperimentsTable, 0, 3);
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestExperimentList::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "----" << RESTendl;
}
