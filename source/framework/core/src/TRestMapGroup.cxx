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
/// A class to create a \codestd::map<string, string>\endcode from a RML config File.
/// In the RML file you can create a section like for example
/// \code
/// <TRestMapGroup name="vetoCalibration" title="vetoCalibration" value="ON" verboseLevel="info">
///    <MapGroup key="4612"  value="1"/>
///    <MapGroup key="4616"  value="1"/>
///    <MapGroup key="4620"  value="1"/>
/// </TRestMapGroup>
/// \endcode
/// Calling the \codeConstructMap()\endcode method will return a map
/// \code
///  (std::map<std::string, std::string>) { "4612" => "1", "4616" => "1", "4620" => "1"}
/// \endcode
///
///
/// <hr>
///
/// \warning **⚠ REST is under continous development.** This documentation
/// is offered to you by the REST community. Your HELP is needed to keep this code
/// up to date. Your feedback will be worth to support this software, please report
/// any problems/suggestions you may find while using it at [The REST Framework
/// forum](http://ezpc10.unizar.es). You are welcome to contribute fixing typos,
/// updating
/// information or adding/proposing new contributions. See also our
/// <a href="https://github.com/rest-for-physics/framework/blob/master/CONTRIBUTING.md">Contribution
/// Guide</a>.
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

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestMapGroup::TRestMapGroup() { Initialize(); }

////////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// The path to the config file can be specified using full path,
/// absolute or relative.
///
/// If the file is not found then REST will try to find the file on
/// the default paths defined in REST Framework, usually at the
/// REST_PATH installation directory. Additional search paths may be
/// defined using the parameter `searchPath` in globals section. See
/// TRestMetadata description.
///
///////////////////////////////////////////////
/// \brief cfgFileName: a string giving the path to a RML file; name: name of the TRestMapGroup XML-section
///
TRestMapGroup::TRestMapGroup(string cfgFileName, string name) {
    Initialize();
    // LoadConfig(cfgFileName);
    LoadConfigFromFile(cfgFileName, name);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestMapGroup::~TRestMapGroup() {}

///////////////////////////////////////////////
/// \brief Construct map from RML input
///
std::map<string, string> TRestMapGroup::ConstructMap() {
    for (unsigned int i = 0; i < fGroupKeys.size(); ++i) {
        fMapGroup[fGroupKeys[i]] = fGroupValues[i];
    }
    return fMapGroup;
}

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestMapGroup::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief Read Map Group from config file
///
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
    metadata << "Input values from RML file: " << endl;
    for (unsigned int i = 0; i < fGroupKeys.size(); i++) {
        metadata << "key = " << fGroupKeys[i] << "; value = " << fGroupValues[i] << endl;
    }

    metadata << endl;
}

