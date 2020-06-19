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
/// TRestSummaryProcess might be added at any stage of the data processing chain.
///
/// This process will register as metadata the following members:
///
/// 1. The *mean rate* and its *mean rate sigma* as deduced from the final number
/// of entries registered at the current TRestAnalysisTree, using the start and end
/// times defined at the current TRestRun object.
///
/// 2. The *average* of any observable available at the TRestAnalysisTree object accessible
/// to the processing chain. It is defined through the keyword `<average` where we must
/// specify the name of the observable we are willing to calculate its average using the
/// field `obsName`. Optionally we are allowed to introduce the range where events
/// will be considered for the average calculation.
///
/// 3. The *RMS* of any observable available at the TRestAnalysisTree object accessible
/// to the processing chain. It is defined through the keyword `<rml` where we must
/// specify the name of the observable we are willing to calculate its RMS using the
/// field `obsName`. Optionally we are allowed to introduce the range where events
/// will be considered for the RMS calculation.
///
/// The following code shows an example of implementation inside a
/// TRestProcessRunner RML section.
///
/// ```
///     <addProcess type="TRestSummaryProcess" name="summary" value="ON" verboseLevel="info" >
///        <average obsName="rawAna_ThresholdIntegral" range="(0,500000)" />
///        <average obsName="rawAna_BaseLineMean" range="(0,500)" />
///        <average obsName="rawAna_BaseLineSigmaMean" range="(0,50)" />
///        <rms obsName="rawAna_ThresholdIntegral" range="(0,500000)" />
///        <rms obsName="rawAna_BaseLineMean" range="(0,500)" />
///    </addProcess>
/// ```
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-May:  First implementation and concept
///             Javier Galan
///
/// \class      TRestSummaryProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestSummaryProcess.h"
using namespace std;

ClassImp(TRestSummaryProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestSummaryProcess::TRestSummaryProcess() { Initialize(); }

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
TRestSummaryProcess::TRestSummaryProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestSummaryProcess::~TRestSummaryProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestSummaryProcess::LoadDefaultConfig() {
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
/// correspondig TRestSummaryProcess section inside the RML.
///
void TRestSummaryProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestSummaryProcess::InitProcess() {
    // For example, try to initialize a pointer to existing metadata
    // accessible from TRestRun
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestSummaryProcess::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestSummaryProcess::ProcessEvent(TRestEvent* evInput) {
    fEvent = evInput;

    // This process does nothing at the event-by-event level

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Function to use when all events have been processed
///
void TRestSummaryProcess::EndProcess() {
    Int_t nEntries = fRunInfo->GetEntries();
    Double_t startTime = fRunInfo->GetStartTimestamp();
    Double_t endTime = fRunInfo->GetEndTimestamp();

    fMeanRate = nEntries / (endTime - startTime);
    fMeanRateSigma = TMath::Sqrt(nEntries) / (endTime - startTime);

    for (auto const& x : fAverageObservable) {
        TVector2 range = fAverageRange[x.first];
        fAverageObservable[x.first] =
            this->GetFullAnalysisTree()->GetObservableAverage(x.first, range.X(), range.Y());
    }

    for (auto const& x : fRMSObservable) {
        TVector2 range = fRMSRange[x.first];
        fRMSObservable[x.first] =
            this->GetFullAnalysisTree()->GetObservableRMS(x.first, range.X(), range.Y());
    }

    if (GetVerboseLevel() >= REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestSummaryProcess section
///
void TRestSummaryProcess::InitFromConfigFile() {
    string definition;
    size_t pos = 0;
    while ((definition = GetKEYDefinition("average", pos)) != "") {
        TString obsName = GetFieldValue("obsName", definition);

        fAverageObservable[obsName] = 0;
        fAverageRange[obsName] = StringTo2DVector(GetFieldValue("range", definition));
    }

    pos = 0;
    while ((definition = GetKEYDefinition("rms", pos)) != "") {
        TString obsName = GetFieldValue("obsName", definition);

        fRMSObservable[obsName] = 0;
        fRMSRange[obsName] = StringTo2DVector(GetFieldValue("range", definition));
    }
}

///////////////////////////////////////////////
/// \brief It prints out the process parameters stored in the
/// metadata structure
///
void TRestSummaryProcess::PrintMetadata() {
    BeginPrintProcess();

    metadata << " - Mean rate : " << fMeanRate << " Hz" << endl;
    metadata << " - Mean rate sigma : " << fMeanRateSigma << " Hz" << endl;
    for (auto const& x : fAverageObservable) {
        metadata << " " << endl;
        metadata << x.first << " average:" << x.second << endl;
        TVector2 a = fAverageRange[x.first];
        if (a.X() != -1 && a.Y() != -1) metadata << "    range : (" << a.X() << ", " << a.Y() << ")" << endl;
    }
    for (auto const& x : fRMSObservable) {
        metadata << " " << endl;
        metadata << x.first << " RMS:" << x.second << endl;
        TVector2 a = fRMSRange[x.first];
        if (a.X() != -1 && a.Y() != -1) metadata << "    range : (" << a.X() << ", " << a.Y() << ")" << endl;
    }
    EndPrintProcess();
}
