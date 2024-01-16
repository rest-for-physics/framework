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
/// 2022-December: First implementation of TRestExperiment
/// Javier Galan
///
/// \class TRestExperiment
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestExperiment.h"

ClassImp(TRestExperiment);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestExperiment::TRestExperiment() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestExperiment::~TRestExperiment() {}

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
TRestExperiment::TRestExperiment(const char* cfgFileName, const std::string& name)
    : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestExperiment::Initialize() { SetSectionName(this->ClassName()); }

void TRestExperiment::GenerateMockDataSet() {
    if (!fBackground) {
        RESTError << "TRestExperiment::GenerateMockData. Background component was not initialized!"
                  << RESTendl;
        return;
    }

    ROOT::RDF::RNode df = fBackground->GetMonteCarloDataFrame(10);

    df.Display({"final_posX"})->Print();
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestExperiment::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    int cont = 0;
    TRestComponent* comp = (TRestComponent*)this->InstantiateChildMetadata(cont, "Component");
    while (comp != nullptr) {
        if (ToLower(comp->GetNature()) == "background")
            fBackground = comp;
        else if (ToLower(comp->GetNature()) == "signal")
            fSignal = comp;
        else
            RESTWarning << "TRestExperiment::InitFromConfigFile. Unknown component!" << RESTendl;

        cont++;
        comp = (TRestComponent*)this->InstantiateChildMetadata(cont, "Component");
    }

    auto ele = GetElement("addComponent");
    if (ele != nullptr) {
        std::string filename = GetParameter("filename", ele, "");
        std::string component = GetParameter("component", ele, "");

        if (filename.empty())
            RESTWarning << "TRestExperiment. There is a problem with `filename` definition inside <addSignal."
                        << RESTendl;
        if (component.empty())
            RESTWarning
                << "TRestExperiment. There is a problem with `component` definition inside <addSignal."
                << RESTendl;

        if (TRestTools::fileExists(filename) && TRestTools::isRootFile(filename)) {
            TFile* file = TFile::Open(filename.c_str(), "READ");
            if (file != nullptr) {
                TRestComponent* comp = file->Get<TRestComponent>(component.c_str());
                if (comp) {
                    if (ToLower(comp->GetNature()) == "signal")
                        fSignal = comp;
                    else if (ToLower(comp->GetNature()) == "background")
                        fBackground = comp;
                    else
                        RESTError << "TRestExperiment::InitFromConfigFile. Component : " << component
                                  << ". Nature unknown!" << RESTendl;
                } else
                    RESTError << "TRestExperiment::InitFromConfigFile. Component : " << component
                              << " not found! File : " << filename << RESTendl;
            }
        }
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestExperiment::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    if (fSignal) RESTMetadata << "Signal component : " << fSignal->GetName() << RESTendl;

    if (fBackground) RESTMetadata << "Background component : " << fBackground->GetName() << RESTendl;

    RESTMetadata << "----" << RESTendl;
}
