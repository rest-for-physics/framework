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
/// 2022-December: First implementation of TRestModel
/// Javier Galan
///
/// \class TRestModel
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestModel.h"

ClassImp(TRestModel);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestModel::TRestModel() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestModel::~TRestModel() {}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestModel::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// combined background.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the distribution.
///
Double_t TRestModel::GetBackground(std::vector<Double_t> point) {
    // we get the rate in seconds at the corresponding bin
    return 0.0;
}

///////////////////////////////////////////////
/// \brief It returns the intensity/rate (in seconds) corresponding to the
/// combined signal.
///
/// The size of the point vector must have the same dimension as the dimensions
/// of the distribution.
///
Double_t TRestModel::GetSignal(std::vector<Double_t> point) {
    // we get the rate in seconds at the corresponding bin
    return 0.0;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestModel::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "----" << RESTendl;
}
