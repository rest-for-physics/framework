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
    RESTDebug << "Entering TRestComponent constructor( cfgFileName, name )" << RESTendl;
    RESTDebug << "File: " << cfgFileName << " Name: " << name << RESTendl;
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

///////////////////////////////////////////
/// \brief It returns the position of the fVariable element for the variable
/// name given by argument.
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
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponent::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    if (fVariables.size() != fRanges.size())
        RESTWarning << "The number of variables does not match with the number of defined ranges!"
                    << RESTendl;

    else if (!fVariables.empty()) {
        int n = 0;
        RESTMetadata << " === Variables === " << RESTendl;
        for (const auto& varName : fVariables) {
            RESTMetadata << " - Name: " << varName << " Range: (" << fRanges[n].X() << ", " << fRanges[n].Y()
                         << ")" << RESTendl;
            n++;
        }
    }

    if (!fParameter.empty()) {
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " === Parameterization === " << RESTendl;
        RESTMetadata << "- Parameter : " << fParameter << RESTendl;

        RESTMetadata << " - Number of parametric nodes : " << fParameterizationNodes.size() << RESTendl;
        RESTMetadata << " " << RESTendl;
        RESTMetadata << " Use : PrintNodes() for additional info" << RESTendl;
        RESTMetadata << " Use : PrintStatistics() if this is a TRestComponentDataSet" << RESTendl;
    }

    RESTMetadata << "----" << RESTendl;
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
