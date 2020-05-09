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
/// TRestDummyToDetectorHitsProcess an empty TRestDummyEvent to
/// TRestDetectorHits process to serve as a copy/paste template to add
/// future processes exporting data to detector hits type.
///
/// This implementation is a specific example that connects to the detector
/// library. That means the input is a TRestDummyEvent, and the output will
/// be a TRestDetectorHitsEvent. Using similar way we could connect/export
/// data to other REST libraries.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2019-May:  First implementation
///            Javier Galan
///
/// \class      TRestDummyToDetectorHitsProcess
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestDummyToDetectorHitsProcess.h"
using namespace std;

ClassImp(TRestDummyToDetectorHitsProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDummyToDetectorHitsProcess::TRestDummyToDetectorHitsProcess() { Initialize(); }

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
///
TRestDummyToDetectorHitsProcess::TRestDummyToDetectorHitsProcess(char* cfgFileName) {
    Initialize();
    LoadConfigFromFile(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDummyToDetectorHitsProcess::~TRestDummyToDetectorHitsProcess() {
    delete fHitsEvent;
    delete fDummyEvent;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDummyToDetectorHitsProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    cout << "Dummy to hits metadata not found. Loading default values" << endl;
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestDummyToDetectorHitsProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fDummyEvent = new TRestDummyEvent();
    fHitsEvent = new TRestHitsEvent();
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestDummyToDetectorHitsProcess::InitProcess() {
    // For example, try to initialize a pointer to existing metadata
    // accessible from TRestRun
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDummyToDetectorHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fDummyEvent = (TRestDummyEvent*)evInput;

    // fHits event should be filled here

    return fHitsEvent;
}

void TRestDummyToDetectorHitsProcess::EndProcess() {}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestDummyMetadata section
///
void TRestDummyToDetectorHitsProcess::InitFromConfigFile() {
    // Class members can be initialized here
}
