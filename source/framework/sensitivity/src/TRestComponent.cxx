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
/// This class allows to make a selection of ROOT data files that fulfill
/// certain metadata conditions allowing to create a group of files that
/// define a particular dataset. The files will be searched in a relative
/// or absolute path that is given together the `filePattern` parameter.
///
/// ### Basic file selection
///
/// We will be able to define the dates range where files will be
/// accepted, using `startTime` and `endTime` parameters. The run start
/// time and end time stored inside TRestRun will be evaluated to decide
/// if the file should be considered.
///
/// A summary of the basic parameters follows:
///
/// * **filePattern**: A full path glob pattern to the files that will
/// be considered. It is a first filter considering the path and the
/// filename. Usual wild cards such as * or ? will be allowed to target
/// a given range of files.
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-December: First implementation of TRestComponent
/// Javier Galan
///
/// \class TRestComponent
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include "TRestComponent.h"

ClassImp(TRestComponent);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestComponent::TRestComponent() { Initialize(); }

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

    RESTMetadata << "----" << RESTendl;
}
