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
///
/// \class TRestMapGroup
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-Jun:  First implementation
///		Konrad Altenmüller
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestMapGroup.h"

ClassImp(TRestMapGroup);

TRestMapGroup::TRestMapGroup() { Initialize(); }

TRestMapGroup::~TRestMapGroup() {}

/// \brief Construct map
void TRestMapGroup::ConstructMap() {
    for (unsigned int i = 0; i < fGroupKeys.size(); ++i) {
        fMapGroup[fGroupKeys[i]] = fGroupValues[i];
    }
}
///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestMapGroup::Initialize() { SetSectionName(this->ClassName()); }
/// \brief Read Map Group from config file
///////////////////////////////////////////////
void TRestMapGroup::InitFromConfigFile() {
    TiXmlElement* groupDefinition = GetElement("MapGroup");

    while (groupDefinition != NULL) {
        fGroupKeys.push_back(GetFieldValue("key", groupDefinition));
        fGroupValues.push_back(GetFieldValue("value", groupDefinition));
        groupDefinition = GetNextElement(groupDefinition);
    }
}

///////////////////////////////////////////////
/// Printing metadata member values on screen
///
void TRestMapGroup::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Group Map (key => value): " << endl;
    for (auto i = fMapGroup.begin(); i != fMapGroup.end(); ++i) {
        metadata << i->first << " => " << i->second << endl;
    }

    metadata << endl;
}

