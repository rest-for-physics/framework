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
///
///
/// TODO Write a proper documentation here
///
/// \warning REST is under continous development. This README is offered
/// to you by the REST community. Your HELP is needed to keep this file up
/// to date. Feel free to contribute fixing typos, updating information or
/// adding new contributions. See also our Contribution Guide.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-Nov:  First implementation
///            AUTHOR NAME
///
/// \class      TRestDummyAnalysisProcess
/// \author     AUTHOR NAME
///
/// <hr>
///
#include "TRestDummyAnalysisProcess.h"
using namespace std;

ClassImp(TRestDummyAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDummyAnalysisProcess::TRestDummyAnalysisProcess() { Initialize(); }

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
TRestDummyAnalysisProcess::TRestDummyAnalysisProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDummyAnalysisProcess::~TRestDummyAnalysisProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDummyAnalysisProcess::LoadDefaultConfig() {
    SetName(this->ClassName());
    SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestDummyAnalysisProcess section inside the RML.
///
void TRestDummyAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestDummyAnalysisProcess::InitProcess() {
    fStartTimestamp = fRunInfo->GetStartTimestamp();
    fEndTimestamp = fRunInfo->GetEndTimestamp();

    if (fSQLVariables.size() > 0)
        FillDBArrays();
    else {
        warning << "TRestDummyAnalysisProcess::InitProcess. No data base field entries have been specified!"
                << endl;
        warning << "This process will do nothing!" << endl;
    }
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestDummyAnalysisProcess::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDummyAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fEvent = evInput;

    // An analysis process does not specialize the event to Hits, Geant4, Signal, ...
    // Therefore, we should not acess the event data at a pure analysis process
    // Just basic access will be availble such as eventId, timestamp, etc.
    //
    // A pure analysis process will usually perform some calculations and then
    // add the calculated value to the analysis tree.
    //
    // SetObservableValue( "myNewObsName", calculatedValue );

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestDummyAnalysisProcess section
///
void TRestDummyAnalysisProcess::InitFromConfigFile() {
    //
    // Implement any special members to be read from the
    // corresponding RML section
    // Basic <parameter name="member" should be automatically identified with member fMember
    //
}

///////////////////////////////////////////////
/// \brief It prints out basic information of the SQL database used to generate the analysis tree observables.
/// And the SQL fields extracted together with its corresponding analysis tree name, and minimum and maximum
/// values during the duration of the run.
///
void TRestDummyAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    //
    // Print out any metadata member here using metadata output directive
    // metadata << "My output : " << fMember << endl;
    //

    EndPrintProcess();
}

// Implement any auxiliar methods as needed
