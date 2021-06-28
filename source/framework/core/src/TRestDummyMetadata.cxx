/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
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
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

////////////////////////////////////////////////////////////////////////////
///
/// A TRestMetadata template
///
/// \class TRestDummyMetadata
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-Jun:  First implementation
///			Javier Galan
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestDummyMetadata.h"

ClassImp(TRestDummyMetadata);

TRestDummyMetadata::TRestDummyMetadata() { Initialize(); }

TRestDummyMetadata::~TRestDummyMetadata() {}

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestDummyMetadata::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// Implement this only if necessary
/// void TRestDummyMetadata::InitFromConfigFile() {
/// }

///////////////////////////////////////////////
/// Printing metadata member values on screen
///
void TRestDummyMetadata::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Dummy : " << fDummy << endl;
    metadata << endl;
}

