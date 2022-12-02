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
/// TRestDataQualityProcess ... TODO documentation here ...
///
///
/// ```
/// A code example here
///
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
///				Oscar Perez
///
/// \class      TRestDataQualityProcess
/// \author     Javier Galan
/// \author     Oscar Perez
///
/// <hr>
///
#include "TRestDataQualityProcess.h"
using namespace std;

ClassImp(TRestDataQualityProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataQualityProcess::TRestDataQualityProcess() { Initialize(); }

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
/// \param configFilename A const char* giving the path to an RML file.
///
TRestDataQualityProcess::TRestDataQualityProcess(const char* configFilename) {
    Initialize();

    LoadConfig(configFilename);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataQualityProcess::~TRestDataQualityProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDataQualityProcess::LoadDefaultConfig() {
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
/// \param configFilename A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestDataQualityProcess section inside the RML.
///
void TRestDataQualityProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestDataQualityProcess::InitProcess() {
    // For example, try to initialize a pointer to existing metadata
    // accessible from TRestRun
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestDataQualityProcess::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDataQualityProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;

    // This process does nothing at the event-by-event level

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Function to use when all events have been processed
///
void TRestDataQualityProcess::EndProcess() {
    /// We loop to each quality number definition
    for (int n = 0; n < fQualityNumber.size(); n++) {
        /// We loop to each rule from the quality definition
        for (int r = 0; r < fRules[n].GetNumberOfRules(); r++) {
            // We implement metadata-based quality numbers
            if (fRules[n].GetType(r) == "metadata") {
                if (EvaluateMetadataRule(fRules[n].GetValue(r), fRules[n].GetRange(r)))
                    EnableBit(fQualityNumber[n], fRules[n].GetBit(r));
                else
                    DisableBit(fQualityNumber[n], fRules[n].GetBit(r));
            }

            if (fRules[n].GetType(r) == "obsAverage") {
                // TODO implementation
            }

            if (fRules[n].GetType(r) == "obsMax") {
                // TODO implementation
            }
        }
    }

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();

    ///// Just some examples accessing analysis tree
    ///// We can get now any entry.
    // this->GetFullAnalysisTree()->GetEntry(3);
    // this->GetFullAnalysisTree()->PrintObservables();
    // cout << "Nobs : " << fRunInfo->GetAnalysisTree()->GetNumberOfObservables() << endl;
    // cout << "Entries : " << fRunInfo->GetAnalysisTree()->GetEntries() << endl;
    // cout << "Nobs : " << this->GetFullAnalysisTree()->GetNumberOfObservables() << endl;
    // cout << "Entries : " << this->GetFullAnalysisTree()->GetEntries() << endl;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestDataQualityProcess section
///
void TRestDataQualityProcess::InitFromConfigFile() {
    size_t position = 0;
    string QNumberString;

    // We get each qualityNumber definition
    while ((QNumberString = GetKEYStructure("qualityNumber", position)) != "NotFound") {
        string qualityDefinition = GetKEYDefinition("qualityNumber", QNumberString);

        TString name = GetFieldValue("name", qualityDefinition);
        fQualityTag.push_back(name);

        fQualityNumber.push_back(0);

        TRestDataQualityRules rules;

        RESTDebug << "Quality number tag : " << name << RESTendl;
        RESTDebug << "------------------" << RESTendl;

        size_t position_2 = 0;
        string ruleDefinition;
        while ((ruleDefinition = GetKEYDefinition("rule", position_2, QNumberString)) != "") {
            TVector2 range = StringTo2DVector(GetFieldValue("range", ruleDefinition));
            TString type = GetFieldValue("type", ruleDefinition);
            TString value = GetFieldValue("value", ruleDefinition);
            Int_t bit = StringToInteger(GetFieldValue("bit", ruleDefinition));

            // TODO check that the bit is not already defined inswide rules.bits
            // If already defined output a warning << and do not push_back the values into rules

            // TODO if types = metadata check that the class and the data member exist in fRunInfo
            // If don't we output a warning << and do not push_back the values into rules

            // TODO if types = obsAverage check that the observable name exists in the analysisTree
            // If don't we output a warning << and do not push_back the values into rules

            // TODO if types = obsMax check that the observable name exists in the analysisTree
            // If don't we output a warning << and do not push_back the values into rules

            // If everything in TODO is ok we push the rule!
            rules.AddRule(type, value, range, bit);

            RESTDebug << "Rule " << rules.GetNumberOfRules() << RESTendl;
            RESTDebug << "+++++++++++" << RESTendl;
            RESTDebug << "Type : " << type << RESTendl;
            RESTDebug << "Value : " << value << RESTendl;
            RESTDebug << "Bit : " << bit << RESTendl;
            RESTDebug << "Range : (" << range.X() << ", " << range.Y() << ")" << RESTendl;
        }
        fRules.push_back(rules);
    }

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Extreme) GetChar();
}

///////////////////////////////////////////////
/// \brief It prints out the process parameters stored in the
/// metadata structure
///
void TRestDataQualityProcess::PrintMetadata() {
    BeginPrintProcess();

    for (int n = 0; n < fQualityNumber.size(); n++) {
        RESTMetadata << "    " << RESTendl;
        RESTMetadata << "xxxxxxxxxxxxxxxxxxxxxx" << RESTendl;
        RESTMetadata << " tag : " << fQualityTag[n] << ". Quality number : " << fQualityNumber[n] << RESTendl;
        RESTMetadata << "xxxxxxxxxxxxxxxxxxxxxx" << RESTendl;
        RESTMetadata << "  " << RESTendl;
        RESTMetadata << "Rules that have been found in range:" << RESTendl;
        RESTMetadata << "  -----------------  " << RESTendl;
        Int_t rulesInRange = 0;
        for (int r = 0; r < fRules[n].GetNumberOfRules(); r++)
            if (isBitEnabled(fQualityNumber[n], fRules[n].GetBit(r))) {
                RESTMetadata << fRules[n].GetValue(r) << " is in range (" << fRules[n].GetRange(r).X() << ", "
                             << fRules[n].GetRange(r).Y() << ")" << RESTendl;
                rulesInRange++;
            }
        if (!rulesInRange) RESTMetadata << "No rules found in range!" << RESTendl;
        RESTMetadata << " " << RESTendl;

        RESTMetadata << "Rules that have NOT been found in range:" << RESTendl;
        RESTMetadata << "  -----------------  " << RESTendl;
        Int_t rulesOutRange = 0;
        for (int r = 0; r < fRules[n].GetNumberOfRules(); r++)
            if (!isBitEnabled(fQualityNumber[n], fRules[n].GetBit(r))) {
                RESTMetadata << fRules[n].GetValue(r) << " is NOT in range (" << fRules[n].GetRange(r).X()
                             << ", " << fRules[n].GetRange(r).Y() << ")" << RESTendl;
                rulesOutRange++;
            }
        if (!rulesOutRange) RESTMetadata << "No rules found outside range!" << RESTendl;
    }

    EndPrintProcess();
}

Bool_t TRestDataQualityProcess::EvaluateMetadataRule(TString value, TVector2 range) {
    vector<string> results = REST_StringHelper::Split((string)value, "::", false, true);

    if (results.size() == 2) {
        if (fRunInfo->GetMetadataClass(results[0])) {
            string val = fRunInfo->GetMetadataClass(results[0])->GetDataMemberValue(results[1]);
            Double_t dblVal = StringToDouble(val);

            // If the metadata value is in range we return true
            if (dblVal >= range.X() && dblVal <= range.Y()) return true;
        } else if (GetFriend(results[0])) {
            string val = GetFriend(results[0])->GetDataMemberValue(results[1]);
            Double_t dblVal = StringToDouble(val);

            // If the metadata value is in range we return true
            if (dblVal >= range.X() && dblVal <= range.Y()) return true;
        } else {
            RESTError << "TRestDataQualityProcess::EvaluateMetadataRule." << RESTendl;
            RESTError << "Metadata class " << results[0] << " is not available inside TRestRun" << RESTendl;
        }
    } else
        RESTError << "TRestDataQualityProcess::EvaluateMetadataRule. Wrong number of elements found"
                  << RESTendl;
    return false;
}
