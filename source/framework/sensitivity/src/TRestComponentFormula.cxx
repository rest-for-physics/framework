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
Double_t TRestComponentFormula::GetRate(std::vector<Double_t> point) { return 0.0; }

///////////////////////////////////////////////
/// \brief This method integrates the rate to all the parameter space defined in the density function.
/// The result will be returned in s-1.
///
Double_t TRestComponentFormula::GetTotalRate() {
    Double_t integral = 0;

    return integral;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestComponentFormula::PrintMetadata() {
    TRestComponent::PrintMetadata();

    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestComponentFormula::InitFromConfigFile() { TRestComponent::InitFromConfigFile(); }
