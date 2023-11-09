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
/// 2023-December: First implementation of TRestComponentFormula
/// Javier Galan
///
/// \class TRestComponentFormula
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestComponentFormula.h"

#include <numeric>

#include "TKey.h"

ClassImp(TRestComponentFormula);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestComponentFormula::TRestComponentFormula() { Initialize(); }

TRestComponentFormula::TRestComponentFormula(const char* configFilename) : TRestComponent(configFilename) {
    Initialize();

    LoadConfigFromFile(fConfigFileName);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
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
TRestComponentFormula::TRestComponentFormula(const char* cfgFileName, const std::string& name)
    : TRestComponent(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestComponentFormula::~TRestComponentFormula() {}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestComponentFormula::Initialize() {
    TRestComponent::Initialize();

    SetSectionName(this->ClassName());
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// generated distribution or formula evaluated at the position of the parameter
/// space given by point.
///
/// The density should be normalized to the corresponding parameter space. During
/// the component construction, **the user is responsible** to initialize the component
/// with the appropriate units. For example, if the parameter space is 2 spatial
/// dimensions and 1 energy dimension, the contribution of each cell or event to
/// the component will be expressed in mm-2 keV-1 which are the default units for
/// distance and energy.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the distribution.
///
Double_t TRestComponentFormula::GetRate(std::vector<Double_t> point) {
    Double_t density =
        GetDensityForActiveNode()->GetBinContent(GetDensityForActiveNode()->GetBin(point.data()));

    Double_t norm = 1;

    // Perhaps this value could be stored internally
    for (size_t n = 0; n < fNbins.size(); n++) norm = norm * (fRanges[n].Y() - fRanges[n].X()) / fNbins[n];

    return norm * density;
}

///////////////////////////////////////////////
/// \brief This method integrates the rate to all the parameter space defined in the density function.
/// The result will be returned in s-1.
///
Double_t TRestComponentFormula::GetTotalRate() {
    THnD* dHist = GetDensityForActiveNode();

    Double_t integral = 0;
    if (dHist != nullptr) integral = dHist->ComputeIntegral();

    // Perhaps this value could be stored internally
    for (size_t n = 0; n < fNbins.size(); n++)
        integral = integral * (fRanges[n].Y() - fRanges[n].X()) / fNbins[n];

    return integral;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponentFormula::PrintMetadata() {
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
void TRestComponentFormula::PrintStatistics() {
    if (fNodeStatistics.empty() && IsDataSetLoaded()) fNodeStatistics = ExtractNodeStatistics();

    if (!IsDataSetLoaded()) {
        RESTWarning << "TRestComponentFormula::PrintStatistics. No dataset loaded." << RESTendl;
        RESTWarning << "Invoking TRestComponentFormula::LoadDataSets might solve the problem" << RESTendl;
        return;
    }

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
void TRestComponentFormula::PrintNodes() {
    std::cout << " - Number of nodes : " << fParameterizationNodes.size() << std::endl;
    for (const auto& x : fParameterizationNodes) std::cout << x << " ";
    std::cout << std::endl;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponentFormula::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

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

    ele = GetElement("dataset");
    while (ele != nullptr) {
        fDataSetFileNames.push_back(GetParameter("filename", ele, ""));
        ele = GetNextElement(ele);
    }
}

/////////////////////////////////////////////
/// \brief It will produce a histogram with the distribution defined using the
/// variables and the weights for each of the parameter nodes.
///
void TRestComponentFormula::FillHistograms() {
    fNodeStatistics = ExtractNodeStatistics();

    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponentFormula::FillHistograms. Dataset has not been initialized!" << RESTendl;
        return;
    }

    if (fParameterizationNodes.empty()) {
        RESTWarning << "Nodes have not been defined" << RESTendl;
        RESTWarning << "The full dataset will be used to generate the density distribution" << RESTendl;
        fParameterizationNodes.push_back(-137);
    }

    RESTInfo << "Generating N-dim histograms" << RESTendl;
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
            df = fDataSet.GetDataFrame().Filter(filter);
        }

        Int_t* bins = new Int_t[fNbins.size()];
        Double_t* xmin = new Double_t[fNbins.size()];
        Double_t* xmax = new Double_t[fNbins.size()];

        for (size_t n = 0; n < fNbins.size(); n++) {
            bins[n] = fNbins[n];
            xmin[n] = fRanges[n].X();
            xmax[n] = fRanges[n].Y();
        }

        TString hName = fParameter + "_" + DoubleToString(node);

        std::vector<std::string> varsAndWeight = fVariables;
        if (!fWeights.empty()) {
            std::string weightsStr = "";
            for (size_t n = 0; n < fWeights.size(); n++) {
                if (n > 0) weightsStr += "*";

                weightsStr += fWeights[n];
            }
            df = df.Define("componentWeight", weightsStr);
            varsAndWeight.push_back("componentWeight");
        }

        auto hn = df.HistoND({hName, hName, (int)fNbins.size(), bins, xmin, xmax}, varsAndWeight);
        THnD* hNd = new THnD(*hn);

        fNodeDensity.push_back(hNd);
        fActiveNode = nIndex;
        nIndex++;
    }
}

/////////////////////////////////////////////
/// \brief It returns the position of the fVariable element for
/// the variable name given by argument.
///
Int_t TRestComponentFormula::GetVariableIndex(std::string varName) {
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
Int_t TRestComponentFormula::SetActiveNode(Double_t node) {
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
THnD* TRestComponentFormula::GetDensityForNode(Double_t node) {
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
THnD* TRestComponentFormula::GetDensityForActiveNode() {
    if (fActiveNode >= 0) return fNodeDensity[fActiveNode];

    RESTError << "The active node is invalid" << RESTendl;
    PrintNodes();
    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument
///
TH1D* TRestComponentFormula::GetHistogram(Double_t node, std::string varName) {
    Int_t v1 = GetVariableIndex(varName);

    if (v1 >= 0) return GetDensityForNode(node)->Projection(v1);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH1D* TRestComponentFormula::GetHistogram(std::string varName) {
    Int_t v1 = GetVariableIndex(varName);

    if (v1 >= 0 && GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns the 2-dimensional projected histogram for the variable names
/// provided in the argument
///
TH2D* TRestComponentFormula::GetHistogram(Double_t node, std::string varName1, std::string varName2) {
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
TH2D* TRestComponentFormula::GetHistogram(std::string varName1, std::string varName2) {
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
TH3D* TRestComponentFormula::GetHistogram(Double_t node, std::string varName1, std::string varName2,
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
TH3D* TRestComponentFormula::GetHistogram(std::string varName1, std::string varName2, std::string varName3) {
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
std::vector<Double_t> TRestComponentFormula::ExtractParameterizationNodes() {
    if (!fParameterizationNodes.empty()) return fParameterizationNodes;

    std::vector<double> vs;
    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponentFormula::ExtractParameterizationNodes. Dataset has not been initialized!"
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
std::vector<Int_t> TRestComponentFormula::ExtractNodeStatistics() {
    if (!fNodeStatistics.empty()) return fNodeStatistics;

    std::vector<Int_t> stats;
    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponentFormula::ExtractNodeStatistics. Dataset has not been initialized!"
                  << RESTendl;
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
Bool_t TRestComponentFormula::LoadDataSets() {
    if (fDataSetFileNames.empty()) {
        RESTWarning << "Dataset filename was not defined. You may still use "
                       "TRestComponentFormula::LoadDataSet( filename );"
                    << RESTendl;
        fDataSetLoaded = false;
        return fDataSetLoaded;
    }

    RESTInfo << "Loading datasets" << RESTendl;

    std::vector<std::string> fullFileNames;
    for (const auto& name : fDataSetFileNames) {
        std::string fileName = SearchFile(name);
        if (fileName.empty()) {
            RESTError << "TRestComponentFormula::LoadDataSet. Error loading file : " << name << RESTendl;
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
        FillHistograms();
        return fDataSetLoaded;
    }

    return fDataSetLoaded;
}

/////////////////////////////////////////////
/// \brief It returns true if all variables have been found inside TRestDataSet
///
Bool_t TRestComponentFormula::VariablesOk() {
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
Bool_t TRestComponentFormula::WeightsOk() {
    Bool_t ok = true;
    std::vector cNames = fDataSet.GetDataFrame().GetColumnNames();

    for (const auto var : fWeights)
        if (std::count(cNames.begin(), cNames.end(), var) == 0) {
            RESTError << "Weight ---> " << var << " <--- NOT found on dataset" << RESTendl;
            ok = false;
        }
    return ok;
}
