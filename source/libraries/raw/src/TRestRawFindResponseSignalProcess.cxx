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
/// The TRestRawFindResponseSignalProcess seems to get the dominant signal,
/// renormalize, and place it at the output event. The process is far from
/// being a generic process, since it receives many hard-coded parameters.
/// At first sight it looks as if the output file will contain a
/// representative signal per event (extracted after some conditions) and
/// renormalized to 1000 amplitude units.
///
/// \warning This process might be obsolete today. It may need additional
/// revision, validation, and documentation. Use it under your own risk. If you
/// find this process useful for your work feel free to use it, improve it,
/// validate and/or document this process. If all those points are addressed
/// these lines can be removed.
///
/// **⚠ WARNING: REST is under continous development.** This documentation
/// is offered to you by the REST community. Your HELP is needed to keep this file
/// up to date. You are very welcome to contribute fixing typos, updating
/// information or adding new contributions. See also our
/// <a href="https://lfna.unizar.es/rest-development/REST_v2/-/blob/master/CONTRIBUTING.md">
/// Contribution guide </a>.
/// Any concern, issue or problem using REST might also be posted to the
/// [REST Framework Forum](http://ezpc10.unizar.es/).
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-Feb: First implementation
///           Javier Galan
///
/// \class      TRestRawFindResponseSignalProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestRawFindResponseSignalProcess.h"
using namespace std;

#include <TFile.h>

ClassImp(TRestRawFindResponseSignalProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRawFindResponseSignalProcess::TRestRawFindResponseSignalProcess() { Initialize(); }

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
TRestRawFindResponseSignalProcess::TRestRawFindResponseSignalProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    PrintMetadata();
    // TRestRawFindResponseSignalProcess default constructor
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawFindResponseSignalProcess::~TRestRawFindResponseSignalProcess() { delete fOutputSignalEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawFindResponseSignalProcess::LoadDefaultConfig() {
    SetName("findResponseSignal-Default");
    SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawFindResponseSignalProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputSignalEvent = NULL;
    fOutputSignalEvent = new TRestRawSignalEvent();
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
void TRestRawFindResponseSignalProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization. Function to be executed once at the
/// beginning of process (before starting the event processing)
///
void TRestRawFindResponseSignalProcess::InitProcess() {}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawFindResponseSignalProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestRawSignalEvent*)evInput;

    // We accept signals that are inside a given condition.
    // TODO: Now it is also possible to use ApplyCut and <cut definitions?
    if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;
    if (fInputSignalEvent->GetNumberOfSignals() > 8) return NULL;

    Int_t dominantSignal = -1;
    Double_t maxPeak = 0;
    //   Double_t maxTime = 0;
    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        if (fInputSignalEvent->GetSignal(n)->GetMaxPeakValue() > maxPeak) {
            maxPeak = fInputSignalEvent->GetSignal(n)->GetMaxPeakValue();
            // maxTime = fInputSignalEvent->GetSignal(n)->GetMaxPeakBin();
            dominantSignal = n;
        }
    }

    if (maxPeak < 400 || maxPeak > 600) return NULL;

    TRestRawSignal* sgnl = fInputSignalEvent->GetSignal(dominantSignal);
    sgnl->Scale(1000. / maxPeak);

    fOutputSignalEvent->AddSignal(*sgnl);

    return fOutputSignalEvent;
}

///////////////////////////////////////////////
/// \brief End of procesing. Function to be executed once at the
/// end of the process. It gets access to all the analysis tree
/// entries. See for example: TRestDataSummary.
///
void TRestRawFindResponseSignalProcess::EndProcess() {}
