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
/// This class ...
///
/// \code
///    <TRestComponentDataSet name="agSignal_vacuum">
///    <!-- We add here all the axion-photon coupling components -->
///        <dataset filename="${DUST}/Dataset_BabyIAXO_XMM_mm_Vacuum.root" weight="1" />
///
///        <variable name="final_posX" range="(-10,10)mm" bins="10" />
///        <variable name="final_posY" range="(-10,10)mm" bins="10" />
///        <variable name="final_energy" range="(0,10)keV" bins="20" />
///
///        <parameter name="weights" value="{NGamma}"/>
///        <parameter name="parameter" value="final_mass" />
///        <parameter name="parameterizationNodes" value="" />
///    </TRestComponentDataSet>
/// \endcode
///
/// \code
///     restRoot
///     [0] TRestComponentDataSet comp("components.rml", "agSignal_vacuum");
///     [1] comp.LoadDataSets()
///     [2] TFile *f = TFile::Open("vacuumComponent.root", "RECREATE");
///     [3] comp.Write("agSignal_vacuum");
/// \endcode
///
/// \code
///     restRoot vacuumComponents.root
///     [0] TCanvas *c = agVacuum->DrawComponent( { "final_posX", "final_posY"}, {"final_energy"}, 2);
///	    [1] c->Print("component_hitmaps.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="component_hitmap.png"]{width:750px;}</style> \endhtmlonly
/// ![A 2-dimensional histogram scan versus the `final_energy` observable, generated by the DrawComponent
/// method](component_hitmap.png)
///
/// \code
///     restRoot vacuumComponents.root
///     [0] TCanvas *c = agVacuum->DrawComponent( { "final_energy"}, {"final_posX", "final_posY"}, 2);
///	    [1] c->Print("component_hitmaps.png");
/// \endcode
///
/// In both cases each plot will regroup 2 bins.
///
/// \htmlonly <style>div.image img[src="component_spectra.png"]{width:750px;}</style> \endhtmlonly
/// ![A 1-dimensional histogram scan versus the `final_posX` and `final_posY` observables, generated by the
/// DrawComponent method](component_spectra.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-December: First implementation of TRestComponentDataSet
/// Javier Galan
///
/// \class TRestComponentDataSet
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestComponentDataSet.h"

#include <TKey.h>
#include <TLatex.h>

#include <numeric>

ClassImp(TRestComponentDataSet);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestComponentDataSet::TRestComponentDataSet() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestComponentDataSet::~TRestComponentDataSet() {}

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
TRestComponentDataSet::TRestComponentDataSet(const char* cfgFileName, const std::string& name)
    : TRestComponent(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestComponentDataSet::Initialize() {
    TRestComponent::Initialize();

    SetSectionName(this->ClassName());
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// generated distribution or formula evaluated at the position of the parameter
/// space given by point.
///
/// The rate will be normalized to the corresponding parameter space. Thus, if
/// the parameter consists of 2-spatial dimensions and 1-energy dimension, the
/// returned rate will be expressed in standard REST units as, s-1 mm-2 keV-1.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the distribution.
///
/// If interpolation is enabled (which is the default) the rate will be evaluated
/// using interpolation with neighbor histogram cells.
///
/// Interpolation technique extracted from:
/// https://math.stackexchange.com/questions/1342364/formula-for-n-dimensional-linear-interpolation
///
/// 𝑓(𝑥0,𝑥1,𝑥2)=𝐴000(1−𝑥0)(1−𝑥1)(1−𝑥2)+𝐴001𝑥0(1−𝑥1)(1−𝑥2)+𝐴010(1−𝑥0)𝑥1(1−𝑥2)⋯+𝐴111𝑥0𝑥1𝑥
///
Double_t TRestComponentDataSet::GetRate(std::vector<Double_t> point) {
    if (point.size() != GetDimensions()) {
        RESTError << "The size of the point given is : " << point.size() << RESTendl;
        RESTError << "The density distribution dimensions are : " << GetDimensions() << RESTendl;
        RESTError << "Point must have the same dimensions as the distribution" << RESTendl;
        return 0;
    }

    if (!HasNodes()) {
        RESTError << "TRestComponentDataSet::GetRate. The component has no nodes!" << RESTendl;
        RESTError << "Try calling TRestComponentDataSet::LoadDataSets" << RESTendl;

        RESTInfo << "Trying to load datasets" << RESTendl;
        LoadDataSets();
        if (IsDataSetLoaded())
            RESTInfo << "Sucess!" << RESTendl;
        else
            return 0;
    }

    if (HasNodes() && fActiveNode == -1) {
        RESTError << "TRestComponentDataSet::GetRate. Active node has not been defined" << RESTendl;
        return 0;
    }

    Int_t centerBin[GetDimensions()];
    Double_t centralDensity = GetDensity()->GetBinContent(GetDensity()->GetBin(point.data()), centerBin);
    if (!Interpolation()) return centralDensity;

    std::vector<Int_t> direction;
    std::vector<Double_t> nDist;
    for (size_t dim = 0; dim < GetDimensions(); dim++) {
        Double_t x1 = GetBinCenter(dim, centerBin[dim] - 1);
        Double_t x2 = GetBinCenter(dim, centerBin[dim] + 1);

        if (centerBin[dim] == 1 || centerBin[dim] == fNbins[dim]) {
            direction.push_back(0);
            nDist.push_back(0);
        } else if (x2 - point[dim] > point[dim] - x1) {
            // we chose left bin (x1) since it is closer than right bin
            direction.push_back(-1);
            nDist.push_back(1 - 2 * (point[dim] - x1) / (x2 - x1));
        } else {
            direction.push_back(1);
            nDist.push_back(1 - 2 * (x2 - point[dim]) / (x2 - x1));
        }
    }

    // In 3-dimensions we got 8 points to interpolate
    // In 4-dimensions we would get 16 points to interpolate
    // ...
    Int_t nPoints = (Int_t)TMath::Power(2, (Int_t)GetDimensions());

    Double_t sum = 0;
    for (int n = 0; n < nPoints; n++) {
        std::vector<int> cell = REST_StringHelper::IntegerToBinary(n, GetDimensions());

        Double_t weightDistance = 1;
        int cont = 0;
        for (const auto& c : cell) {
            if (c == 0)
                weightDistance *= (1 - nDist[cont]);
            else
                weightDistance *= nDist[cont];
            cont++;
        }

        for (size_t k = 0; k < cell.size(); k++) cell[k] = cell[k] * direction[k] + centerBin[k];

        Double_t density = GetDensity()->GetBinContent(cell.data());
        sum += density * weightDistance;
    }

    return sum;
}

///////////////////////////////////////////////
/// \brief This method integrates the rate to all the parameter space defined in the density function.
/// The result will be returned in s-1.
///
Double_t TRestComponentDataSet::GetTotalRate() {
    THnD* dHist = GetDensityForActiveNode();

    Double_t integral = 0;
    if (dHist != nullptr) integral = dHist->ComputeIntegral();

    // Perhaps this value could be stored internally
    for (size_t n = 0; n < fNbins.size(); n++)
        integral = integral * (fRanges[n].Y() - fRanges[n].X()) / fNbins[n];

    return integral;
}

///////////////////////////////////////////////
/// \brief It returns the bin center of the given component dimension.
///
/// It required implementation since I did not find a method inside THnD. Surprising.
///
Double_t TRestComponentDataSet::GetBinCenter(Int_t nDim, const Int_t bin) {
    return fRanges[nDim].X() + (fRanges[nDim].Y() - fRanges[nDim].X()) * ((double)bin - 0.5) / fNbins[nDim];
}

///////////////////////////////////////////////
/// \brief A method allowing to draw a series of plots representing the density distributions.
///
/// The method will produce 1- or 2-dimensional histograms of the `drawVariables` given in the
/// argument. A third scan variable must be provided in order to show the distribution slices
/// along the scan variable.
///
/// The binScanSize argument can be used to define the binSize of the scanning variables.
///
TCanvas* TRestComponentDataSet::DrawComponent(std::vector<std::string> drawVariables,
                                              std::vector<std::string> scanVariables, Int_t binScanSize,
                                              TString drawOption) {
    if (drawVariables.size() > 2 || drawVariables.size() == 0) {
        RESTError << "TRestComponentDataSet::DrawComponent. The number of variables to be drawn must "
                     "be 1 or 2!" << RESTendl;
        return fCanvas;
    }

    if (scanVariables.size() > 2 || scanVariables.size() == 0) {
        RESTError << "TRestComponentDataSet::DrawComponent. The number of variables to be scanned must "
                     "be 1 or 2!" << RESTendl;
        return fCanvas;
    }

    //// Checking the number of plots to be generated
    std::vector<int> scanIndexes;
    for (const auto& x : scanVariables) scanIndexes.push_back(GetVariableIndex(x));

    Int_t nPlots = 1;
    size_t n = 0;
    for (const auto& x : scanIndexes) {
        if (fNbins[x] % binScanSize != 0) {
            RESTWarning << "The variable " << scanVariables[n] << " contains " << fNbins[x]
                        << " bins and it doesnt match with a bin size " << binScanSize << RESTendl;
            RESTWarning << "The bin size must be a multiple of the number of bins." << RESTendl;
            RESTWarning << "Redefining bin size to 1." << RESTendl;
            binScanSize = 1;
        }
        nPlots *= fNbins[x] / binScanSize;
        n++;
    }

    /// Finding canvas division scheme
    Int_t nPlotsX = 0;
    Int_t nPlotsY = 0;

    if (scanIndexes.size() == 2) {
        nPlotsX = fNbins[scanIndexes[0]] / binScanSize;
        nPlotsY = fNbins[scanIndexes[1]] / binScanSize;
    } else {
        nPlotsX = TRestTools::CanvasDivisions(nPlots)[1];
        nPlotsY = TRestTools::CanvasDivisions(nPlots)[0];
    }

    RESTInfo << "Number of plots to be generated: " << nPlots << RESTendl;
    RESTInfo << "Canvas size : " << nPlotsX << " x " << nPlotsY << RESTendl;

    //// Setting up the canvas with the appropriate number of divisions
    if (fCanvas != nullptr) {
        delete fCanvas;
        fCanvas = nullptr;
    }

    fCanvas = new TCanvas(this->GetName(), this->GetName(), 0, 0, nPlotsX * 640, nPlotsY * 480);
    fCanvas->Divide(nPlotsX, nPlotsY, 0.01, 0.01);

    std::vector<int> variableIndexes;
    for (const auto& x : drawVariables) variableIndexes.push_back(GetVariableIndex(x));

    for (int n = 0; n < nPlotsX; n++)
        for (int m = 0; m < nPlotsY; m++) {
            TPad* pad = (TPad*)fCanvas->cd(n * nPlotsY + m + 1);
            pad->SetFixedAspectRatio(true);

            THnD* hnd = GetDensity();

            int binXo = binScanSize * n + 1;
            int binXf = binScanSize * n + binScanSize;
            int binYo = binScanSize * m + 1;
            int binYf = binScanSize * m + binScanSize;

            if (scanVariables.size() == 2) {
                hnd->GetAxis(scanIndexes[0])->SetRange(binXo, binXf);
                hnd->GetAxis(scanIndexes[1])->SetRange(binYo, binYf);
            } else if (scanVariables.size() == 1) {
                binXo = binScanSize * nPlotsY * n + binScanSize * m + 1;
                binXf = binScanSize * nPlotsY * n + binScanSize * m + binScanSize;
                hnd->GetAxis(scanIndexes[0])->SetRange(binXo, binXf);
            }

            if (variableIndexes.size() == 1) {
                TH1D* h1 = hnd->Projection(variableIndexes[0]);
                std::string hName;

                if (scanIndexes.size() == 2)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") " + scanVariables[1] + "(" + IntegerToString(binYo) + ", " +
                            IntegerToString(binYf) + ") ";

                if (scanIndexes.size() == 1)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") ";

                TH1D* newh = (TH1D*)h1->Clone(hName.c_str());
                newh->SetTitle(hName.c_str());
                newh->SetStats(false);
                newh->GetXaxis()->SetTitle((TString)drawVariables[0]);
                newh->SetMarkerStyle(kFullCircle);
                newh->Draw("PLC PMC");

                TString entriesStr = "Entries: " + IntegerToString(newh->GetEntries());
                TLatex* textLatex = new TLatex(0.62, 0.825, entriesStr);
                textLatex->SetNDC();
                textLatex->SetTextColor(1);
                textLatex->SetTextSize(0.05);
                textLatex->Draw("same");
                delete h1;
            }

            if (variableIndexes.size() == 2) {
                TH2D* h2 = hnd->Projection(variableIndexes[0], variableIndexes[1]);

                std::string hName;
                if (scanIndexes.size() == 2)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") " + scanVariables[1] + "(" + IntegerToString(binYo) + ", " +
                            IntegerToString(binYf) + ") ";

                if (scanIndexes.size() == 1)
                    hName = scanVariables[0] + "(" + IntegerToString(binXo) + ", " + IntegerToString(binXf) +
                            ") ";

                TH2D* newh = (TH2D*)h2->Clone(hName.c_str());
                newh->SetStats(false);
                newh->GetXaxis()->SetTitle((TString)drawVariables[0]);
                newh->GetYaxis()->SetTitle((TString)drawVariables[1]);
                newh->SetTitle(hName.c_str());
                newh->Draw(drawOption);

                TString entriesStr = "Entries: " + IntegerToString(newh->GetEntries());
                TLatex* textLatex = new TLatex(0.62, 0.825, entriesStr);
                textLatex->SetNDC();
                textLatex->SetTextColor(1);
                textLatex->SetTextSize(0.05);
                textLatex->Draw("same");
                delete h2;
            }
        }

    return fCanvas;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponentDataSet::PrintMetadata() {
    TRestComponent::PrintMetadata();

    if (!fDataSetFileNames.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " == Dataset filenames ==" << RESTendl;

        for (const auto& x : fDataSetFileNames) RESTMetadata << "- " << x << RESTendl;

        RESTMetadata << " " << RESTendl;
    }

    if (!fParameter.empty() && fParameterizationNodes.empty()) {
        RESTMetadata << "This component has no nodes!" << RESTendl;
        RESTMetadata << " Use: LoadDataSets() to initialize the nodes" << RESTendl;
    }

    if (!fWeights.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " == Weights ==" << RESTendl;

        for (const auto& x : fWeights) RESTMetadata << "- " << x << RESTendl;

        RESTMetadata << " " << RESTendl;
    }

    RESTMetadata << " Use : PrintStatistics() to check node statistics" << RESTendl;
    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It prints out the statistics available for each parametric node
///
void TRestComponentDataSet::PrintStatistics() {
    if (fNSimPerNode.empty() && IsDataSetLoaded()) fNSimPerNode = ExtractNodeStatistics();

    if (!HasNodes() && !IsDataSetLoaded()) {
        RESTWarning << "TRestComponentDataSet::PrintStatistics. Empty nodes and no dataset loaded!"
                    << RESTendl;
        RESTWarning << "Invoking TRestComponentDataSet::LoadDataSets might solve the problem" << RESTendl;
        return;
    }

    auto result = std::accumulate(fNSimPerNode.begin(), fNSimPerNode.end(), 0);
    std::cout << "Total counts : " << result << std::endl;
    std::cout << std::endl;

    std::cout << " Parameter node statistics (" << fParameter << ")" << std::endl;
    int n = 0;
    for (const auto& p : fParameterizationNodes) {
        std::cout << " - Value : " << p << " Counts: " << fNSimPerNode[n] << std::endl;
        n++;
    }
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponentDataSet::InitFromConfigFile() {
    TRestComponent::InitFromConfigFile();

    auto ele = GetElement("dataset");
    while (ele != nullptr) {
        fDataSetFileNames.push_back(GetParameter("filename", ele, ""));
        ele = GetNextElement(ele);
    }
}

/////////////////////////////////////////////
/// \brief It will produce a histogram with the distribution defined using the
/// variables and the weights for each of the parameter nodes.
///
void TRestComponentDataSet::FillHistograms() {
    fNSimPerNode = ExtractNodeStatistics();

    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponentDataSet::FillHistograms. Dataset has not been initialized!" << RESTendl;
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
        //// Yet not tested in the case when we want to define a unique node without filters
        //// Needs to be improved
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
        if (fParameterizationNodes.empty()) hName = "full";

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
        hNd->Scale(1. / fNSimPerNode[nIndex]);

        fNodeDensity.push_back(hNd);
        fActiveNode = nIndex;
        nIndex++;
    }
}

/////////////////////////////////////////////
/// \brief
///
THnD* TRestComponentDataSet::GetDensityForNode(Double_t node) {
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
THnD* TRestComponentDataSet::GetDensityForActiveNode() {
    if (fActiveNode >= 0) return fNodeDensity[fActiveNode];

    RESTError << "The active node is invalid" << RESTendl;
    PrintNodes();
    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument
///
TH1D* TRestComponentDataSet::GetHistogram(Double_t node, std::string varName) {
    SetActiveNode(node);
    return GetHistogram(varName);
}

/////////////////////////////////////////////
/// \brief It returns a 1-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH1D* TRestComponentDataSet::GetHistogram(std::string varName) {
    if (fActiveNode < 0) return nullptr;

    Int_t v1 = GetVariableIndex(varName);

    if (v1 >= 0 && GetDensityForActiveNode()) return GetDensityForActiveNode()->Projection(v1);

    return nullptr;
}

/////////////////////////////////////////////
/// \brief It returns the 2-dimensional projected histogram for the variable names
/// provided in the argument
///
TH2D* TRestComponentDataSet::GetHistogram(Double_t node, std::string varName1, std::string varName2) {
    SetActiveNode(node);
    return GetHistogram(varName1, varName2);
}

/////////////////////////////////////////////
/// \brief It returns a 2-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH2D* TRestComponentDataSet::GetHistogram(std::string varName1, std::string varName2) {
    if (fActiveNode < 0) return nullptr;

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
TH3D* TRestComponentDataSet::GetHistogram(Double_t node, std::string varName1, std::string varName2,
                                          std::string varName3) {
    SetActiveNode(node);
    return GetHistogram(varName1, varName2, varName3);
}

/////////////////////////////////////////////
/// \brief It returns a 3-dimensional projected histogram for the variable names
/// provided in the argument. It will recover the histogram corresponding to
/// the active node.
///
TH3D* TRestComponentDataSet::GetHistogram(std::string varName1, std::string varName2, std::string varName3) {
    if (fActiveNode < 0) return nullptr;

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
std::vector<Double_t> TRestComponentDataSet::ExtractParameterizationNodes() {
    if (!fParameterizationNodes.empty()) return fParameterizationNodes;

    RESTInfo << "Extracting parameterization nodes" << RESTendl;

    std::vector<double> vs;
    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponentDataSet::ExtractParameterizationNodes. Dataset has not been initialized!"
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
/// If fNSimPerNode has already been initialized it will
/// directly return its value.
///
std::vector<Int_t> TRestComponentDataSet::ExtractNodeStatistics() {
    if (!fNSimPerNode.empty()) return fNSimPerNode;

    std::vector<Int_t> stats;
    if (!IsDataSetLoaded()) {
        RESTError << "TRestComponentDataSet::ExtractNodeStatistics. Dataset has not been initialized!"
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
Bool_t TRestComponentDataSet::LoadDataSets() {
    if (fDataSetFileNames.empty()) {
        RESTWarning << "Dataset filename was not defined. You may still use "
                       "TRestComponentDataSet::LoadDataSet( filename );" << RESTendl;
        fDataSetLoaded = false;
        return fDataSetLoaded;
    }

    RESTInfo << "Loading datasets" << RESTendl;

    std::vector<std::string> fullFileNames;
    for (const auto& name : fDataSetFileNames) {
        // TODO we get here a list of files. However, we will need to weight each dataset with a factor
        // to consider the contribution of each background component.
        // Of course, we could previously take a factor into account already in the dataset, through the
        // definition of a new column. But being this way would allow us to play around with the
        // background model without having to regenerate the dataset.
        std::string fileName = SearchFile(name);
        if (fileName.empty()) {
            RESTError << "TRestComponentDataSet::LoadDataSet. Error loading file : " << name << RESTendl;
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
Bool_t TRestComponentDataSet::VariablesOk() {
    Bool_t ok = true;
    std::vector cNames = fDataSet.GetDataFrame().GetColumnNames();

    for (const auto& var : fVariables)
        if (std::count(cNames.begin(), cNames.end(), var) == 0) {
            RESTError << "Variable ---> " << var << " <--- NOT found on dataset" << RESTendl;
            ok = false;
        }
    return ok;
}

/////////////////////////////////////////////
/// \brief It returns true if all weights have been found inside TRestDataSet
///
Bool_t TRestComponentDataSet::WeightsOk() {
    Bool_t ok = true;
    std::vector cNames = fDataSet.GetDataFrame().GetColumnNames();

    for (const auto& var : fWeights)
        if (std::count(cNames.begin(), cNames.end(), var) == 0) {
            RESTError << "Weight ---> " << var << " <--- NOT found on dataset" << RESTendl;
            ok = false;
        }
    return ok;
}

/////////////////////////////////////////////
/// \brief Takes care of initializing datasets if have not been initialized.
/// On sucess it returns true.
///
Bool_t TRestComponentDataSet::ValidDataSet() {
    if (!IsDataSetLoaded()) {
        RESTWarning << "TRestComponentDataSet::ValidDataSet. Dataset has not been loaded" << RESTendl;
        RESTWarning << "Try calling TRestComponentDataSet::LoadDataSets" << RESTendl;

        RESTInfo << "Trying to load datasets" << RESTendl;
        LoadDataSets();
        if (IsDataSetLoaded()) {
            RESTInfo << "Sucess!" << RESTendl;
        } else {
            RESTError << "Failed loading datasets" << RESTendl;
            return false;
        }
    }

    if (HasNodes() && fActiveNode == -1) {
        RESTError << "TRestComponentDataSet::ValidDataSet. Active node has not been defined" << RESTendl;
        return false;
    }
    return true;
}