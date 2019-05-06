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
/// The TRestRawSignalRemoveChannelsProcess allows to remove a selection
/// daq channels ids from a TRestRawSignalEvent. The channels should be
/// provided through the corresponding section at the RML configuration file.
///
/// An application of this process is to evaluate the impact on the detector
/// response due to the absence of signal on some channels, or dead channels.
///
/// The following example will remove the channels with signal ids 17,19,27
/// and 67 from the TRestRawSignalEvent output.
///
/// \code
/// <TRestRawSignalRemoveChannelsProcess name="rmChannels" title="Removing few
/// channels" verboseLevel="debug" >
///     <removeChannel id="17" />
///     <removeChannel id="19" />
///     <removeChannel id="27" />
///     <removeChannel id="67" />
/// </TRestRawSignalRemoveChannelsProcess>
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-November: First implementation of TRestRawSignalRemoveChannelsProcess.
///             Javier Galan
///
/// \class      TRestRawSignalRemoveChannelsProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestRawSignalRemoveChannelsProcess.h"
using namespace std;

ClassImp(TRestRawSignalRemoveChannelsProcess)

    ///////////////////////////////////////////////
    /// \brief Default constructor
    ///
    TRestRawSignalRemoveChannelsProcess::TRestRawSignalRemoveChannelsProcess() {
  Initialize();
}

///////////////////////////////////////////////
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
///
TRestRawSignalRemoveChannelsProcess::TRestRawSignalRemoveChannelsProcess(
    char* cfgFileName) {
  Initialize();

  if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

  PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalRemoveChannelsProcess::~TRestRawSignalRemoveChannelsProcess() {
  delete fOutputSignalEvent;
  delete fInputSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalRemoveChannelsProcess::LoadDefaultConfig() {
  SetName("removeChannels-Default");
  SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalRemoveChannelsProcess::Initialize() {
  SetSectionName(this->ClassName());

  fInputSignalEvent = new TRestRawSignalEvent();
  fOutputSignalEvent = new TRestRawSignalEvent();

  fInputEvent = fInputSignalEvent;
  fOutputEvent = fOutputSignalEvent;
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
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestRawSignalRemoveChannelsProcess::LoadConfig(string cfgFilename,
                                                     string name) {
  if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function including required initialization before each event starts
/// to process.
///
void TRestRawSignalRemoveChannelsProcess::BeginOfEventProcess() {
  fOutputSignalEvent->Initialize();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalRemoveChannelsProcess::ProcessEvent(
    TRestEvent* evInput) {
  fInputSignalEvent = (TRestRawSignalEvent*)evInput;

  for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
    TRestRawSignal* sgnl = fInputSignalEvent->GetSignal(n);

    Bool_t removeChannel = false;
    for (unsigned int x = 0; x < fChannelIds.size() && !removeChannel; x++)
      if (sgnl->GetID() == fChannelIds[x]) removeChannel = true;

    if (!removeChannel) fOutputSignalEvent->AddSignal(*sgnl);

    if (GetVerboseLevel() >= REST_Extreme)
      cout << "Channel ID : " << sgnl->GetID() << endl;

    if (GetVerboseLevel() >= REST_Debug && removeChannel)
      cout << "Removing channel id : " << sgnl->GetID() << endl;
  }

  if (GetVerboseLevel() >= REST_Extreme) GetChar();

  return fOutputSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestSignalToRawSignalProcess metadata section
///
void TRestRawSignalRemoveChannelsProcess::InitFromConfigFile() {
  size_t pos = 0;

  string removeChannelDefinition;
  while ((removeChannelDefinition = GetKEYDefinition("removeChannel", pos)) !=
         "") {
    Int_t id = StringToInteger(GetFieldValue("id", removeChannelDefinition));
    fChannelIds.push_back(id);
  }
}
