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
///    <TRestComponentFormula name="DummyFormulaComponent">
///            <variable name="final_posX" range="(-10,10)mm" bins="100" />
///            <variable name="final_posY" range="(-1,1)cm" bins="100" />
///            <variable name="final_energy" range="(0,10)keV" bins="20" />
///
///            <parameter name="units" value="cm^-2*keV^-1" />
///
///			   // A spatial gaussian component contribution (energy normalized)
///            <formula name="gaus" expression="1E-${REST_BCK_LEVEL} *
/// TMath::Exp(-[final_posX]*[final_posX]-[final_posY]*[final_posY])/(1+[final_energy])" />
///			   // A flat contribution
///            <formula name="flat" expression="1E-7" />
///     </TRestComponentFormula>
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

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
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

    FillHistograms();
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// formula evaluated at the position of the parameter space given by point
/// and integrated to the parameter space cell volume.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the variables of the distribution.
///
Double_t TRestComponentFormula::GetFormulaRate(std::vector<Double_t> point) {
    if (fVariables.size() != point.size()) {
        RESTError << "Point should have same dimensions as number of variables!" << RESTendl;
        return 0;
    }

    Double_t result = 0;
    for (auto& formula : fFormulas) {
        for (size_t n = 0; n < fVariables.size(); n++) formula.SetParameter(fVariables[n].c_str(), point[n]);

        result += formula.EvalPar(nullptr);
    }

    Double_t normFactor = 1;
    for (size_t n = 0; n < GetDimensions(); n++) {
        normFactor *= (fRanges[n].Y() - fRanges[n].X()) / fNbins[n];
    }

    return normFactor * result / units(fUnits);
}

/////////////////////////////////////////////
/// \brief It will produce a histogram with the distribution using the formula
/// contributions.
///
/// For the moment this method will just fill one node (without fParameter). But
/// if the component expression depends on the node parameter it might require
/// further development.
///
/// TODO: The histogram is filled just by evaluating the formula, but it would
/// be more realistic that we fill the histograms with a number N of entries
/// that mimic a MC generation scheme similar to TRestComponentDataSet.
///
void TRestComponentFormula::FillHistograms() {
    if (fFormulas.empty()) return;

    RESTInfo << "Generating N-dim histogram for " << GetName() << RESTendl;

    TString hName = "formula";

    Int_t* bins = new Int_t[fNbins.size()];
    Double_t* xlow = new Double_t[fNbins.size()];
    Double_t* xhigh = new Double_t[fNbins.size()];

    for (size_t n = 0; n < fNbins.size(); n++) {
        bins[n] = fNbins[n];
        xlow[n] = fRanges[n].X();
        xhigh[n] = fRanges[n].Y();
    }

    THnD* hNd = new THnD(hName, hName, fNbins.size(), bins, xlow, xhigh);

    // Calculate the bin width in each dimension
    std::vector<double> binWidths;
    for (size_t i = 0; i < fNbins.size(); ++i) {
        double width = static_cast<double>(xhigh[i] - xlow[i]) / bins[i];
        binWidths.push_back(width);
    }

    // Nested loop to iterate over each bin and print its center
    std::vector<int> binIndices(fNbins.size(), 0);  // Initialize bin indices to 0 in each dimension

    bool carry = false;
    while (!carry) {
        // Calculate the center of the current bin in each dimension
        std::vector<double> binCenter;
        for (size_t i = 0; i < fNbins.size(); ++i)
            binCenter.push_back(xlow[i] + (binIndices[i] + 0.5) * binWidths[i]);

        hNd->Fill(binCenter.data(), GetFormulaRate(binCenter));

        // Update bin indices for the next iteration
        carry = true;
        for (size_t i = 0; i < fNbins.size(); ++i) {
            binIndices[i]++;
            if (binIndices[i] < bins[i]) {
                carry = false;
                break;
            }
            binIndices[i] = 0;
        }
    }

    fNodeDensity.clear();
    fNodeDensity.push_back(hNd);
    fActiveNode = 0;  // For the moment only 1-node!
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponentFormula::PrintMetadata() {
    TRestComponent::PrintMetadata();

    RESTMetadata << " " << RESTendl;
    RESTMetadata << "Formula units: " << fUnits << RESTendl;

    if (!fFormulas.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " == Contributions implemented inside the component ==" << RESTendl;

        for (const auto& x : fFormulas)
            RESTMetadata << "- " << x.GetName() << " = " << x.GetExpFormula() << RESTendl;

        RESTMetadata << " " << RESTendl;
    }

    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponentFormula::InitFromConfigFile() {
    TRestComponent::InitFromConfigFile();

    if (!fFormulas.empty()) return;

    auto ele = GetElement("formula");
    while (ele != nullptr) {
        std::string name = GetParameter("name", ele, "");
        std::string expression = GetParameter("expression", ele, "");

        if (expression.empty()) {
            RESTWarning << "TRestComponentFormula::InitFromConfigFile. Invalid formula" << RESTendl;
        } else {
            TFormula formula(name.c_str(), expression.c_str());

            for (Int_t n = 0; n < formula.GetNpar(); n++) {
                if (std::find(fVariables.begin(), fVariables.end(), formula.GetParName(n)) ==
                    fVariables.end()) {
                    RESTError << "Variable : " << formula.GetParName(n) << " not found in component! "
                              << RESTendl;
                    RESTError << "TRestComponentFormula evaluation will lead to wrong results!" << RESTendl;
                }
            }

            for (const auto& varName : fVariables) formula.SetParameter(varName.c_str(), 0.0);

            fFormulas.push_back(formula);
        }

        ele = GetNextElement(ele);
    }
}
