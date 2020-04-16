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
/// The TRestRawSignalToSignalProcess transforms a TRestRawSignalEvent into
/// a TRestSignalEvent. It applies a direct transform between both data
/// types. The data points inside the raw signal are transformed to time
/// information using the input sampling time and time start provided
/// through the process RML section.
///
/// All the data points will be transferred to the output signal event.
///
/// The following list describes the different parameters that can be
/// used in this process.
/// * **sampling**: It is the sampling time of input raw signal data.
/// Time units must be specified (ns, us, ms).
/// * **triggerStarts**: It defines the absolute time value for the first
/// raw data bin.
/// * **gain**: Each data point from the resulting output signal will be
/// multiplied by this factor.
///
/// The following lines of code show how the process metadata should be
/// defined.
///
/// \code
///
/// // A raw signal with 200ns binning will be translated to a
/// // TRestSignalEvent. The new signal will start at time=20us, and its
/// // amplitude will be reduced a factor 50.
///
/// <TRestRawSignalToSignalProcess name="rsTos" title"Raw signal to signal">
///     <parameter name="sampling" value="0.2" units="us" />
///     <parameter name="triggerStarts" value="20" units="us" />
///     <parameter name="gain" value="1/50." />
/// </TRestRawSignalToSignalProcess>
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-February: First implementation of rawsignal to signal conversion.
///             Javier Gracia
///
/// 2017-November: Class documented and re-furbished
///             Javier Galan
///
/// \class      TRestRawSignalToSignalProcess
/// \author     Javier Gracia
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestRawSignalToSignalProcess.h"
using namespace std;

ClassImp(TRestRawSignalToSignalProcess)

    ///////////////////////////////////////////////
    /// \brief Default constructor
    ///
    TRestRawSignalToSignalProcess::TRestRawSignalToSignalProcess() {
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
TRestRawSignalToSignalProcess::TRestRawSignalToSignalProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalToSignalProcess::~TRestRawSignalToSignalProcess() {
    delete fOutputSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalToSignalProcess::LoadDefaultConfig() {
    SetName("rawSignalToSignal-Default");
    SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalToSignalProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputSignalEvent = NULL;
    fOutputSignalEvent = new TRestSignalEvent();
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
void TRestRawSignalToSignalProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalToSignalProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestRawSignalEvent*)evInput;

    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        TRestSignal sgnl;
        sgnl.Initialize();
        TRestRawSignal* rawSgnl = fInputSignalEvent->GetSignal(n);
        sgnl.SetID(rawSgnl->GetID());
        for (int p = 0; p < rawSgnl->GetNumberOfPoints(); p++)
            sgnl.NewPoint(fTriggerStarts + fSampling * p, fGain * rawSgnl->GetData(p));

        fOutputSignalEvent->AddSignal(sgnl);
    }

    return fOutputSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestRawSignalToSignalProcess metadata section
///
void TRestRawSignalToSignalProcess::InitFromConfigFile() {
    fSampling = GetDblParameterWithUnits("sampling");
    fTriggerStarts = GetDblParameterWithUnits("triggerStarts");
    fGain = StringToDouble(GetParameter("gain", "1"));
}
