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

#include <numeric>

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
    Initialize();
    LoadConfigFromFile(fConfigFileName, name);
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
    return GetDensityForActiveNode()->GetBinContent(GetDensityForActiveNode()->GetBin(point.data()));
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

        RESTMetadata << " - Number of parametric nodes : " << fParameterizationNodes.size() << RESTendl;
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " Use : PrintStatistics() or PrintNodes() for additional info" << RESTendl;
    }

    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It prints out the statistics available for each parametric node
///
void TRestComponent::PrintStatistics() {
    if (fNodeStatistics.empty() && IsDataSetLoaded()) fNodeStatistics = ExtractNodeStatistics();

    auto result = std::accumulate(fNodeStatistics.begin(), fNodeStatistics.end(), 0);
    std::cout << "Total counts : " << result << std::endl;
    std::cout << std::endl;

    std::cout << " Parameter node statistics (" << fParameter << ")" << std::endl;
    int n = 0;
    for (const auto& p : fParameterizationNodes) {
        std::cout << " - Value : " << p << " Counts: " << fNodeStatistics[n] << std::endl;
        n++;
    }
}

/////////////////////////////////////////////
/// \brief It prints out on screen the values of the parametric node
///
void TRestComponent::PrintNodes() {
    std::cout << " - Number of nodes : " << fParameterizationNodes.size() << std::endl;
    for (const auto& x : fParameterizationNodes) std::cout << x << " ";
    std::cout << std::endl;
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
}

/////////////////////////////////////////////
/// \brief
///
void TRestComponent::GenerateSparseHistograms() {
    fNodeStatistics = ExtractNodeStatistics();

    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponent::GenerateSparseHistograms. Dataset has not been initialized!"
                  << RESTendl;
        return;
    }

    if (fParameterizationNodes.empty()) {
        RESTWarning << "Nodes have not been defined" << RESTendl;
        RESTWarning << "The full dataset will be used to generate the density distribution" << RESTendl;
        fParameterizationNodes.push_back(-137);
    }

    RESTInfo << "Generating Sparse histograms" << RESTendl;
    int nIndex = 0;
    for (const auto& node : fParameterizationNodes) {
        ROOT::RDF::RNode df = ROOT::RDataFrame(0);
        if (fParameterizationNodes.size() == 1 && node == -137) {
            RESTInfo << "Creating component with no parameters (full dataset used)" << RESTendl;
            df = fDataSet.GetDataFrame();
            fParameterizationNodes.clear();
        } else {
            RESTInfo << "Creating THnD for parameter " << fParameter << ": " << DoubleToString(node)
                     << RESTendl;
            std::string filter = fParameter + " == " + DoubleToString(node);
            df = fDataSet.GetDataFrame();
            // df = fDataSet.GetDataFrame().Filter(filter);
        }

        std::string weightsStr = "";
        for (size_t n = 0; n < fWeights.size(); n++) {
            if (n > 0) weightsStr += "*";

            weightsStr += fWeights[n];
        }
        auto wValues = df.Define("componentWeight", weightsStr).Take<double>("componentWeight");
        std::vector<double> weightValues = *wValues;

        Int_t* bins = new Int_t[fNbins.size()];
        Double_t* xmin = new Double_t[fNbins.size()];
        Double_t* xmax = new Double_t[fNbins.size()];

        for (size_t n = 0; n < fNbins.size(); n++) {
            bins[n] = fNbins[n];
            xmin[n] = fRanges[n].X();
            xmax[n] = fRanges[n].Y();
        }

        TString hName = fParameter + "_" + DoubleToString(node);

        /// It is named sparse because I tried to use first THnSparse.
        /// See root-forum post: https://root-forum.cern.ch/t/problems-using-thnsparse-projection/55829/3
        THnD* sparse = new THnD(hName, hName, fNbins.size(), bins, xmin, xmax);

        std::vector<std::vector<double> > data;
        for (const auto& v : fVariables) {
            auto parValues = df.Take<double>(v);
            data.push_back(*parValues);
        }

        Double_t* values = new Double_t[fVariables.size()];
        std::cout << "N-values filled : " << data[0].size() << std::endl;
        if (!data.empty())
            for (size_t m = 0; m < data[0].size(); m++) {
                for (size_t v = 0; v < fVariables.size(); v++) {
                    values[v] = data[v][m];
                }
                sparse->Fill(values);  // weightValues[m]/fNodeStatistics[nIndex]);
            }
        delete[] values;

        fNodeDensity.push_back(sparse);
        fActiveNode = nIndex;
        nIndex++;
    }
}

/////////////////////////////////////////////
/// \brief It returns the position of the fVariable element for
/// the variable name given by argument.
///
Int_t TRestComponent::GetVariableIndex(std::string varName) {
    int n = 0;
    for (const auto& v : fVariables) {
        if (v == varName) return n;
        n++;
    }

    return -1;
}

/////////////////////////////////////////////
/// \brief It returns the position of the fParameterizationNodes
/// element for the variable name given by argument.
///
Int_t TRestComponent::SetActiveNode(Double_t node) {
    int n = 0;
    for (const auto& v : fParameterizationNodes) {
        if (v == node) {
            fActiveNode = n;
            return fActiveNode;
        }
        n++;
    }

    RESTError << "Parametric node : " << node << " was not found in component" << RESTendl;
    RESTError << "Keeping previous node as active : " << fParameterizationNodes[fActiveNode] << RESTendl;
    PrintNodes();

    return fActiveNode;
}

/////////////////////////////////////////////
/// \brief
///
THnD* TRestComponent::GetDensityForNode(Double_t node) {
    int n = 0;
    for (const auto& x : fParameterizationNodes) {
        if (x == node) {
            return fNodeDensity[n];
        }
        n++;
    }

    RESTError << "Parametric node : " << node << " was not found in component" << RESTendl;
    PrintNodes();
    return nullptr;
}

/////////////////////////////////////////////
/// \brief
///
THnD* TRestComponent::GetDensityForActiveNode() {
    if (fActiveNode >= 0) return fNodeDensity[fActiveNode];

    RESTError << "The active node is invalid" << RESTendl;
    PrintNodes();
    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument
///
TH1D* TRestComponent::GetHistogram(Double_t node, std::string varName) {
    Int_t v1 = GetVariableIndex(varName);

    if (v1 >= 0) return GetDensityForNode(node)->Projection(v1);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH1D* TRestComponent::GetHistogram(std::string varName) {
    Int_t v1 = GetVariableIndex(varName);

    if (v1 >= 0 && GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns the 2-dimensional projected histogram for the variable names
/// provided in the argument
///
TH2D* TRestComponent::GetHistogram(Double_t node, std::string varName1, std::string varName2) {
    Int_t v1 = GetVariableIndex(varName1);
    Int_t v2 = GetVariableIndex(varName2);

    if (v1 >= 0 && v2 >= 0) return GetDensityForNode(node)->Projection(v1, v2);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 2-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH2D* TRestComponent::GetHistogram(std::string varName1, std::string varName2) {
    Int_t v1 = GetVariableIndex(varName1);
    Int_t v2 = GetVariableIndex(varName2);

    if (v1 >= 0 && v2 >= 0)
        if (GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1, v2);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns the 3-dimensional projected histogram for the variable names
/// provided in the argument
///
TH3D* TRestComponent::GetHistogram(Double_t node, std::string varName1, std::string varName2,
                                   std::string varName3) {
    Int_t v1 = GetVariableIndex(varName1);
    Int_t v2 = GetVariableIndex(varName2);
    Int_t v3 = GetVariableIndex(varName3);

    if (v1 >= 0 && v2 >= 0 && v3 >= 0) return GetDensityForNode(node)->Projection(v1, v2, v3);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 3-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH3D* TRestComponent::GetHistogram(std::string varName1, std::string varName2, std::string varName3) {
    Int_t v1 = GetVariableIndex(varName1);
    Int_t v2 = GetVariableIndex(varName2);
    Int_t v3 = GetVariableIndex(varName3);

    if (v1 >= 0 && v2 >= 0 && v3 >= 0)
        if (GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1, v2, v3);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a vector with all the different values found on
/// the dataset column for the user given parameterization variable.
///
/// If fParameterizationNodes has already been initialized it will
/// directly return its value.
///
std::vector<Double_t> TRestComponent::ExtractParameterizationNodes() {
    if (!fParameterizationNodes.empty()) return fParameterizationNodes;

    std::vector<double> vs;
    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponent::ExtractParameterizationNodes. Dataset has not been initialized!"
                  << RESTendl;
        return vs;
    }

    auto parValues = fDataSet.GetDataFrame().Take<double>(fParameter);
    for (const auto v : parValues) vs.push_back(v);

    std::vector<double>::iterator ip;
    ip = std::unique(vs.begin(), vs.begin() + vs.size());
    vs.resize(std::distance(vs.begin(), ip));
    std::sort(vs.begin(), vs.end());
    ip = std::unique(vs.begin(), vs.end());
    vs.resize(std::distance(vs.begin(), ip));

    return vs;
}

/////////////////////////////////////////////
/// \brief It returns a vector with the number of entries found for each
/// parameterization node.
///
/// If fNodeStatistics has already been initialized it will
/// directly return its value.
///
std::vector<Int_t> TRestComponent::ExtractNodeStatistics() {
    if (!fNodeStatistics.empty()) return fNodeStatistics;

    std::vector<Int_t> stats;
    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponent::ExtractNodeStatistics. Dataset has not been initialized!" << RESTendl;
        return stats;
    }

    RESTInfo << "Counting statistics for each node ..." << RESTendl;
    RESTInfo << "Number of nodes : " << fParameterizationNodes.size() << RESTendl;
    for (const auto& p : fParameterizationNodes) {
        std::string filter = fParameter + " == " + DoubleToString(p);
        auto nEv = fDataSet.GetDataFrame().Filter(filter).Count();
        stats.push_back(*nEv);
    }
    return stats;
}

/////////////////////////////////////////////
/// \brief A method responsible to import a list of TRestDataSet into fDataSet
/// and check that the variables and weights defined by the user can be found
/// inside the dataset.
///
Bool_t TRestComponent::LoadDataSets() {
    if (fDataSetFileNames.empty()) {
        RESTWarning
            << "Dataset filename was not defined. You may still use TRestComponent::LoadDataSet( filename );"
            << RESTendl;
        fDataSetLoaded = false;
        return fDataSetLoaded;
    }

    RESTInfo << "Loading datasets" << RESTendl;

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
    fDataSetLoaded = true;

    if (fDataSet.GetTree() == nullptr) {
        RESTError << "Problem loading dataset from file list :" << RESTendl;
        for (const auto& f : fDataSetFileNames) RESTError << " - " << f << RESTendl;
        return false;
    }

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) fDataSet.PrintMetadata();

    if (VariablesOk() && WeightsOk()) {
        fParameterizationNodes = ExtractParameterizationNodes();
        GenerateSparseHistograms();
        return fDataSetLoaded;
    }

    return fDataSetLoaded;
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
