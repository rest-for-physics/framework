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
/// This class allows to ...
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-December: First implementation of TRestComponent
/// Javier Galan
///
/// \class TRestComponent
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestComponent.h"

#include "TKey.h"

ClassImp(TRestComponent);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestComponent::TRestComponent() { Initialize(); }

TRestComponent::TRestComponent(const char* configFilename) : TRestMetadata(configFilename) {
    Initialize();

    LoadConfigFromFile(fConfigFileName);
}

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
TRestComponent::TRestComponent(const char* cfgFileName, const std::string& name)
    : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestComponent::~TRestComponent() {}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestComponent::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// generated distribution or formula evaluated at the position of the parameter
/// space given by point.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the distribution.
///
Double_t TRestComponent::GetRate(std::vector<Double_t> point) {
    // we get the rate in seconds at the corresponding bin
    return 0.0;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponent::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    if (fVariables.size() != fRanges.size())
        RESTWarning << "The number of variables does not match with the number of defined ranges!"
                    << RESTendl;

    else if (fVariables.size() != fNbins.size())
        RESTWarning << "The number of variables does not match with the number of defined bins!" << RESTendl;
    else {
        int n = 0;
        RESTMetadata << " === Variables === " << RESTendl;
        for (const auto& varName : fVariables) {
            RESTMetadata << " - Name: " << varName << " Range: (" << fRanges[n].X() << ", " << fRanges[n].Y()
                         << ") bins: " << fNbins[n] << RESTendl;
            n++;
        }
    }

    RESTMetadata << " " << RESTendl;
    RESTMetadata << " === Weights === " << RESTendl;
    for (const auto& wName : fWeights) RESTMetadata << "- " << wName << RESTendl;

    if (!fParametricVariable.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " === Parametrization === " << RESTendl;
        RESTMetadata << "- Parametric variable : " << fParametricVariable << RESTendl;

        RESTMetadata << " - Parametric nodes : ";
        for (const auto& node : fParametrizationNodes) {
            RESTMetadata << node << " ";
        }
        RESTMetadata << RESTendl;
    }

    RESTMetadata << " - Parametrization binning : " << fParametrizationBinning << RESTendl;

    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponent::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    auto ele = GetElement("variable");
    while (ele != nullptr) {
        std::string name = GetParameter("name", ele, "");
        TVector2 v = Get2DVectorParameterWithUnits("range", ele);
        Int_t bins = StringToInteger(GetParameter("bins", ele, "100"));

        fVariables.push_back(name);
        fRanges.push_back(v);
        fNbins.push_back(bins);

        ele = GetNextElement(ele);
    }

    ele = GetElement("parametricVariable");
    while (ele != nullptr) {
        fParametricVariable = GetParameter("name", ele, "");
        fParametrizationNodes = StringToElements(GetParameter("nodes", ele, "-1"), ",");
        fParametrizationBinning = StringToInteger(GetParameter("bins", ele, "100"));

        ele = GetNextElement(ele);
    }

    ele = GetElement("dataset");
    while (ele != nullptr) {
        fDataSetFileNames.push_back(GetParameter("file", ele, ""));
        ele = GetNextElement(ele);
    }

    if (!fDataSetFileNames.empty()) {
        fDataSetLoaded = LoadDataSets();
    } else {
        RESTWarning
            << "Dataset filename was not defined. You may still use TRestComponent::LoadDataSet( filename );"
            << RESTendl;
    }
}

/////////////////////////////////////////////
/// \brief A method responsible to import a list of TRestDataSet into fDataSet
///
Bool_t TRestComponent::LoadDataSets() {
    std::vector<std::string> fullFileNames;
    for (const auto& name : fDataSetFileNames) {
        std::string fileName = SearchFile(name);
        if (fileName.empty()) {
            RESTError << "TRestComponent::LoadDataSet. Error loading file : " << name << RESTendl;
            RESTError << "Does the file exist?" << RESTendl;
            RESTError << "You may use `<globals> <searchPath ...` to indicate the path location" << RESTendl;
            return false;
        }
        fullFileNames.push_back(fileName);
    }

    fDataSet.Import(fullFileNames);

    if (fDataSet.GetTree() == nullptr) {
        RESTError << "Problem loading dataset from file list :" << RESTendl;
        for (const auto& f : fDataSetFileNames) RESTError << " - " << f << RESTendl;
        return false;
    }

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) fDataSet.PrintMetadata();

    return (VariablesOk() && WeightsOk());
}

/////////////////////////////////////////////
/// \brief It returns true if all variables have been found inside TRestDataSet
///
Bool_t TRestComponent::VariablesOk() {
    Bool_t ok = true;
    std::vector cNames = fDataSet.GetDataFrame().GetColumnNames();

    for (const auto var : fVariables)
        if (std::count(cNames.begin(), cNames.end(), var) == 0) {
            RESTError << "Variable ---> " << var << " <--- NOT found on dataset" << RESTendl;
            ok = false;
        }
    return ok;
}

/////////////////////////////////////////////
/// \brief It returns true if all weights have been found inside TRestDataSet
///
Bool_t TRestComponent::WeightsOk() {
    Bool_t ok = true;
    std::vector cNames = fDataSet.GetDataFrame().GetColumnNames();

    for (const auto var : fWeights)
        if (std::count(cNames.begin(), cNames.end(), var) == 0) {
            RESTError << "Weight ---> " << var << " <--- NOT found on dataset" << RESTendl;
            ok = false;
        }
    return ok;
}
