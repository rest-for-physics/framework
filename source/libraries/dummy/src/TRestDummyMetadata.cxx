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

//////////////////////////////////////////////////////////////////////////
/// TRestDummyMetadata is a dummy metadata class used to define any
/// metadata information that we want to be stored and managed by REST.
///
/// Create a detailed descripition documentation here. Including use and
/// examples.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2019-May: First concept and implementation of TRestDummyEvent class.
///             Javier Galan
///
/// \class      TRestDummyMetadata
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestDummyMetadata.h"
using namespace std;

ClassImp(TRestDummyMetadata);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDummyMetadata::TRestDummyMetadata() : TRestMetadata() { Initialize(); }

///////////////////////////////////////////////
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
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name A string specifying the name of TRestDummyMetadata
/// section to load
///
TRestDummyMetadata::TRestDummyMetadata(const char* cfgFileName, string name) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDummyMetadata::~TRestDummyMetadata() {
    // TRestDummyMetadata destructor
}

///////////////////////////////////////////////
/// \brief Function to initialize any metadata members requiring
/// initialization and define the library version
///
void TRestDummyMetadata::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestDummyMetadata section
///
void TRestDummyMetadata::InitFromConfigFile() {
    this->Initialize();

    // Initialize the metadata members from a configfile
    fDummy = StringToInteger(GetParameter("dummy"));
}

///////////////////////////////////////////////
/// \brief It prints out the process parameters stored in the metadata structure
///
void TRestDummyMetadata::PrintMetadata() {
    // Print output metadata using, metadata << endl;
}
