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
/// Describe any parameters this process receives:
/// * **parameter1**: This parameter ...
/// * **parameter2**: This parameter is ...
///
///
/// ### Examples
/// Give examples of usage and RML descriptions that can be tested.
/// \code
///     <WRITE A CODE EXAMPLE HERE>
/// \endcode
///
/// ### Running pipeline example
/// Add the examples to a pipeline to guarantee the code will be running
/// on future framework upgrades.
///
///
/// Please, add any figure that may help to illustrate the process
///
/// \htmlonly <style>div.image img[src="trigger.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the trigger definition](trigger.png)
///
/// The png image should be uploaded to the ./images/ directory
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// YEAR-Month: First implementation of TRestDataSet
/// WRITE YOUR FULL NAME
///
/// \class TRestDataSet
/// \author: TODO. Write full name and e-mail:        victor
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

std::vector<std::string> TRestDataSet::FileSelect() {
    // Introduce data for time zone GMT +01:00
    std::tm ts{};
    std::tm te{};
    std::istringstream ss(fStartTime);
    std::istringstream se(fEndTime);

    ss >> std::get_time(&ts, "%Y-%m-%dT%H:%M:%S");
    se >> std::get_time(&te, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error{"failed to parse time string -- StartTime"};
    }
    if (se.fail()) {
        throw std::runtime_error{"failed to parse time string -- EndTime"};
    }
    std::time_t time_stamp_start = mktime(&ts);
    std::time_t time_stamp_end = mktime(&te);

    // Selecting files

    //   TRestRun::SetInputFileName(fFilePattern);

    std::cout.precision(10);
    std::vector<std::string> fileNames = TRestTools::GetFilesMatchingPattern(fFilePattern);

    std::vector<std::string> fileSelected;

    for (const auto& file : fileNames) {
        TRestRun* run = new TRestRun(file);
        double runStart = run->GetStartTimestamp();
        double runEnd = run->GetEndTimestamp();

        if (runStart >= time_stamp_start && runEnd <= time_stamp_end) {
            fileSelected.push_back(file);
        }

        // std::cout << "File : " << file << std::endl;
        //  std::cout << "RS : " << runStart << std::endl;
        // std::cout << "RE : " << runEnd << std::endl;
        delete run;
    }
    //   for (const auto& i : fileSelected) {
    //     std::cout << "File Selected : " << i << std::endl;
    //}
    return fileSelected;
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
