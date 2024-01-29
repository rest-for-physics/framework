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

    if (!fExperimentsFile.empty() && fExperiments.empty()) {
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
                         "(Exposure/Background/Signal)" << RESTendl;
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
                         "the number of table columns" << RESTendl;
            RESTError << "Number of table columns : " << nTableColumns << RESTendl;
            RESTError << "Number of expected columns : " << nExpectedColumns << RESTendl;
            RESTError << "Expected columns : " << expectedColumns << RESTendl;
            return;
        }

        fComponentFiles = TRestTools::GetFilesMatchingPattern(fComponentPattern);

        Bool_t generateMockData = false;
        for (const auto& experimentRow : fExperimentsTable) {
            TRestExperiment* experiment = new TRestExperiment();

            std::string rowStr = "";
            for (const auto& el : experimentRow) {
                rowStr += el + " ";
            }

            RESTInfo << "Loading experiment: " << rowStr << RESTendl;

            int column = 0;
            if (fExposureTime == 0) {
                if (REST_StringHelper::isANumber(experimentRow[column])) {
                    experiment->SetExposureInSeconds(
                        REST_StringHelper::StringToDouble(experimentRow[column]));
                    // We will generate mock data once we load the background component
                    generateMockData = true;
                } else if (TRestTools::isRootFile(experimentRow[column])) {
                    // We load the file with the dataset into the experimental data
                    std::string fname = SearchFile(experimentRow[column]);
                    experiment->SetExperimentalDataSetFile(fname);
                    RESTWarning << "Loading experimental data havent been tested yet!" << RESTendl;
                    RESTWarning
                        << "It might require further development. Remove these lines once it works smooth!"
                        << RESTendl;
                } else {
                    RESTError << experimentRow[column] << " is not a exposure time or an experimental dataset"
                              << RESTendl;
                }
                column++;
            } else {
                experiment->SetExposureInSeconds(fExposureTime * units("s"));
                // We will generate mock data once we load the background component
                generateMockData = true;
            }

            if (!fSignal) {
                TRestComponent* sgnl = (TRestComponent*)GetComponent(experimentRow[column])->Clone();
                experiment->SetSignal(sgnl);
                column++;
            } else {
                experiment->SetSignal(fSignal);
            }

            if (!fBackground) {
                TRestComponent* bck = (TRestComponent*)GetComponent(experimentRow[column])->Clone();
                experiment->SetBackground(bck);
            } else {
                experiment->SetBackground(fBackground);
            }

            if (generateMockData) {
                RESTInfo << "Generating mock dataset" << RESTendl;
                experiment->GenerateMockDataSet();
            }

            fExperiments.push_back(experiment);
        }
    }
}

TRestComponent* TRestExperimentList::GetComponent(std::string compName) {
    TRestComponent* component = nullptr;
    for (const auto& c : fComponentFiles) {
        TFile* f = TFile::Open(c.c_str(), "READ");
        TObject* obj = f->Get((TString)compName);

        if (!obj) continue;

        if (obj->InheritsFrom("TRestComponent")) {
            return (TRestComponent*)obj;
        } else {
            RESTError << "An object named : " << compName
                      << " exists inside the file, but it does not inherit from TRestComponent" << RESTendl;
        }
    }

    return component;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestExperimentList::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Number of experiments loaded: " << fExperiments.size() << RESTendl;

    RESTMetadata << "----" << RESTendl;
}
