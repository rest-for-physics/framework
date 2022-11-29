/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
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
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

/////////////////////////////////////////////////////////////////////////
/// Write the class description Here
///
/// ### Parameters
/// * **startTime**: Only files with start run after `startTime` will be considered.
/// * **endTime**: Only files with end run before `endTime` will be considered.
///
/// ### Examples
/// Give examples of usage and RML descriptions that can be tested.
/// \code
///     <WRITE A CODE EXAMPLE HERE>
/// \endcode
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-November: First implementation of TRestDataSet
/// Javier Galan
///
/// \class TRestDataSet
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///

#include "TRestDataSet.h"
#include "TRestRun.h"
#include "TRestTools.h"

ClassImp(TRestDataSet);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataSet::TRestDataSet() { Initialize(); }

/////////////////////////////////////////////
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
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestAxionMagneticField section inside the RML.
///
TRestDataSet::TRestDataSet(const char* cfgFileName, std::string name) : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSet::~TRestDataSet() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestDataSet::Initialize() {
    SetSectionName(this->ClassName());

    // REMOVE COMMENT. Initialize here any special data members if needed
}

///////////////////////////////////////////////
/// \brief Function to determine the filenames that satisfy the dataset conditions
///
std::vector<std::string> TRestDataSet::FileSelection() {
    fFileSelection.clear();

    std::time_t time_stamp_start = REST_StringHelper::StringToTimeStamp(fStartTime);
    std::time_t time_stamp_end = REST_StringHelper::StringToTimeStamp(fEndTime);

    if (!time_stamp_end || !time_stamp_start) {
        RESTError << "TRestDataSet::FileSelect. Start or end dates not properly formed. Please, check "
                     "REST_StringHelper::StringToTimeStamp documentation for valid formats"
                  << RESTendl;
        return fFileSelection;
    }

    std::vector<std::string> fileNames = TRestTools::GetFilesMatchingPattern(fFilePattern);

    for (const auto& file : fileNames) {
        TRestRun* run = new TRestRun(file);
        double runStart = run->GetStartTimestamp();
        double runEnd = run->GetEndTimestamp();

        if (runStart >= time_stamp_start && runEnd <= time_stamp_end) {
            // TODO Add metadata conditions for file selection
            fFileSelection.push_back(file);
        }

        delete run;
    }

    return fFileSelection;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestDataSet::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " - StartTime : " << fStartTime << RESTendl;
    RESTMetadata << " - EndTime : " << fEndTime << RESTendl;
    RESTMetadata << " - File pattern : " << fFilePattern << RESTendl;
    RESTMetadata << "----" << RESTendl;
}