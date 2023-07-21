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

    if (!fParameter.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " === Parameterization === " << RESTendl;
        RESTMetadata << "- Parameter : " << fParameter << RESTendl;

        RESTMetadata << " - Parametric nodes : " << fParameterizationNodes.size() << RESTendl;
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " Use : this->PrintStatistics() for additional info" << RESTendl;
    }

    RESTMetadata << "----" << RESTendl;
}

void TRestComponent::PrintStatistics() {
    auto nEv = fDataSet.GetDataFrame().Count();
    std::cout << "Total counts : " << *nEv << std::endl;
    std::cout << std::endl;

    fNodeStatistics = ExtractNodeStatistics();
    std::cout << " Parameter node statistics (" << fParameter << ")" << std::endl;
    int n = 0;
    for (const auto& p : fParameterizationNodes) {
        std::cout << " - Value : " << p << " Counts: " << fNodeStatistics[n] << std::endl;
        n++;
    }
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponent::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    auto ele = GetElement("variable");
    while (ele != nullptr) {
        std::string name = GetParameter("name", ele, "");
        TVector2 v = Get2DVectorParameterWithUnits("range", ele, TVector2(-1, -1));
        Int_t bins = StringToInteger(GetParameter("bins", ele, "0"));

        if (name.empty() || (v.X() == -1 && v.Y() == -1) || bins == 0) {
            RESTWarning << "TRestComponent::fVariable. Problem with definition." << RESTendl;
            RESTWarning << "Name: " << name << " range: (" << v.X() << ", " << v.Y() << ") bins: " << bins
                        << RESTendl;
        } else {
            fVariables.push_back(name);
            fRanges.push_back(v);
            fNbins.push_back(bins);
        }

        ele = GetNextElement(ele);
    }

    ele = GetElement("dataset");
    while (ele != nullptr) {
        fDataSetFileNames.push_back(GetParameter("filename", ele, ""));
        ele = GetNextElement(ele);
    }

    if (!fDataSetFileNames.empty()) {
        RESTInfo << "Loading datasets" << RESTendl;
        fDataSetLoaded = LoadDataSets();

        fParameterizationNodes = ExtractParameterizationNodes();
    } else {
        RESTWarning
            << "Dataset filename was not defined. You may still use TRestComponent::LoadDataSet( filename );"
            << RESTendl;
    }
}

std::vector<Double_t> TRestComponent::ExtractParameterizationNodes() {
    auto parValues = fDataSet.GetDataFrame().Take<double>(fParameter);
    std::vector<double> vs;
    for (const auto v : parValues) vs.push_back(v);

    std::vector<double>::iterator ip;
    ip = std::unique(vs.begin(), vs.begin() + vs.size());
    vs.resize(std::distance(vs.begin(), ip));
    std::sort(vs.begin(), vs.end());
    ip = std::unique(vs.begin(), vs.end());
    vs.resize(std::distance(vs.begin(), ip));

    return vs;
}

std::vector<Int_t> TRestComponent::ExtractNodeStatistics() {
    if (!fNodeStatistics.empty()) return fNodeStatistics;

    std::cout << "Counting statistics for each node ..." << std::endl;
    std::vector<Int_t> stats;
    for (const auto& p : fParameterizationNodes) {
        std::string filter = fParameter + " == " + DoubleToString(p);
        auto nEv = fDataSet.GetDataFrame().Filter(filter).Count();
        stats.push_back(*nEv);
    }
    return stats;
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
