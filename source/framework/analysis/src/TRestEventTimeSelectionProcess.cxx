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
/// The TRestEventTimeSelectionProcess allows processing of events that are within
/// a certain time range. The time ranges are read from a file that contains
/// the start and end times of the periods. The file should have the following
/// format (it can have more columns separated by the same delimiter, but the first
/// two columns should be the start and end times and all the other columns
/// will be ignored):
/// \code
///     # This is a comment
///     # Start time,End time
///     2021-01-01 00:00:00,2021-01-01 01:00:00
///     2021-01-01 02:00:00,2021-01-01 03:00:00
///     2021-01-01 04:00:00,2021-01-01 05:00:00
/// \endcode
/// The default delimiter is a comma, but it can be changed by setting the
/// `delimiter` parameter (single character only). The time format is
/// given by the accepted formats of the StringToTimeStamp function.
///
/// The time ranges can be active or dead periods of time. If the time ranges
/// are active periods of time, the events that are within the time ranges will
/// be selected. If the time ranges are dead periods of time, the events that
/// are outside the time ranges will be selected. Set the `isActiveTime` parameter
/// to `true` to select the events within the time ranges (they represent active periods
/// of time) or to `false` to select the events outside the time ranges (they represent
/// dead periods of time).
///
/// The number of events selected, rejected and the total time selected are stored in the
/// metadata members. The total time selected is calculated as the sum of the time
/// of all the time ranges, so it represents the total active time in the case of
/// active periods of time, or the total dead time in the case of dead periods of time.
///
/// ### Parameters
/// * **fileWithTimes**: name of the file that contains the time ranges.
/// * **isActiveTime**: if `true` (default) the time ranges represent active periods of time, if `false` the
/// time ranges represent dead periods of time.
/// * **delimiter**: delimiter used in the file with the time ranges (default is `,`).
/// * **offsetTimeInSeconds**: offset time in seconds to be added to the event time (default is 0). This is
/// useful to correct the time of the events if needed. This number of seconds will be added to the event time
/// before checking if it is within the time ranges.
///
/// ### Examples
/// Examples for rml files:
/// \code
/// <addProcess type="TRestEventTimeSelectionProcess" name="timeSel"
/// fileWithTimes="/path/to/file/timeperiods.txt" value="ON" verboseLevel="info"> \endcode <hr>
///
/// \warning ** REST is under continuous development.** This documentation
/// is offered to you by the REST community. Your HELP is needed to keep this code
/// up to date. Your feedback will be worth to support this software, please report
/// any problems/suggestions you may find while using it at [The REST Framework
/// forum](http://ezpc10.unizar.es). You are welcome to contribute fixing typos,
/// updating information or adding/proposing new contributions. See also our
/// <a href="https://github.com/rest-for-physics/framework/blob/master/CONTRIBUTING.md">Contribution
/// Guide</a>.
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
/// 2024-Jun: Use of the processing file itself (no need for external fileWithIDs)
///              Alvaro Ezquerro
///
/// \class      TRestEventTimeSelectionProcess
/// \author     Alvaro Ezquerro
///
/// <hr>
///

#include "TRestEventTimeSelectionProcess.h"

using namespace std;

ClassImp(TRestEventTimeSelectionProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestEventTimeSelectionProcess::TRestEventTimeSelectionProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestEventTimeSelectionProcess::Initialize() {
    SetSectionName(this->ClassName());
    fEvent = nullptr;
    fFileWithTimes = "";
    fIsActiveTime = true;
    fDelimiter = ',';
    fStartEndTimes.clear();
    fTimeOffsetInSeconds = 0;
    fTimeStartMarginInSeconds = 0;
    fTimeEndMarginInSeconds = 0;
    fNEventsRejected = 0;
    fNEventsSelected = 0;
    fTotalTimeInSeconds = 0;
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
///
void TRestEventTimeSelectionProcess::InitProcess() {
    // Read the file with the time ranges
    if (!fFileWithTimes.empty()) {
        fStartEndTimes.clear();
        string line;
        ifstream file(fFileWithTimes);
        if (file.is_open()) {
            while (getline(file, line)) {
                if (line[0] == '#') {  // understand as comment
                    continue;
                }
                std::istringstream lineStream(line);
                std::string startDate, endDate;
                if (std::getline(lineStream, startDate, fDelimiter) &&
                    std::getline(lineStream, endDate, fDelimiter)) {
                    fStartEndTimes.emplace_back(startDate, endDate);
                    RESTDebug << "Start: " << startDate << " End: " << endDate << RESTendl;
                }
            }
            file.close();
        }
    }
    fTotalTimeInSeconds = CalculateTotalTimeInSeconds();
    fNEventsRejected = 0;
    fNEventsSelected = 0;
}

Double_t TRestEventTimeSelectionProcess::CalculateTotalTimeInSeconds() {
    Double_t totalTime = 0;
    for (auto id : fStartEndTimes) {
        TTimeStamp startTime = TTimeStamp(StringToTimeStamp(id.first), 0);
        TTimeStamp endTime = TTimeStamp(StringToTimeStamp(id.second), 0);
        // Reduce the time by the margin in both sides
        startTime.Add(TTimeStamp(fTimeStartMarginInSeconds));
        endTime.Add(TTimeStamp(-fTimeEndMarginInSeconds));
        auto timeDiff = endTime.AsDouble() - startTime.AsDouble();
        if (timeDiff < 0) {
            RESTDebug << "End time is before start time in time range: " << id.first << " to " << id.second
                      << RESTendl;
            continue;
        }
        totalTime += endTime.AsDouble() - startTime.AsDouble();
    }
    return totalTime;
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestEventTimeSelectionProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;

    TTimeStamp eventTime = fEvent->GetTimeStamp();
    eventTime.Add(TTimeStamp(fOffsetTimeInSeconds));
    if (fIsActiveTime) {  // time ranges represent active periods of time
        for (auto id : fStartEndTimes) {
            TTimeStamp startTime = TTimeStamp(StringToTimeStamp(id.first), 0);
            TTimeStamp endTime = TTimeStamp(StringToTimeStamp(id.second), 0);
            // Reduce the time by the margin in both sides
            startTime.Add(TTimeStamp(fTimeStartMarginInSeconds));
            endTime.Add(TTimeStamp(-fTimeEndMarginInSeconds));
            if (eventTime >= startTime && eventTime <= endTime) {
                fNEventsSelected++;
                return fEvent;
            }
        }
    }

    if (!fIsActiveTime) {  // time ranges represent dead periods of time
        Bool_t isInDeadPeriod = false;
        for (auto id : fStartEndTimes) {
            TTimeStamp startTime = TTimeStamp(StringToTimeStamp(id.first), 0);
            TTimeStamp endTime = TTimeStamp(StringToTimeStamp(id.second), 0);
            // Reduce the time by the margin in both sides
            startTime.Add(TTimeStamp(fTimeStartMarginInSeconds));
            endTime.Add(TTimeStamp(-fTimeEndMarginInSeconds));
            if (eventTime >= startTime && eventTime <= endTime) {
                isInDeadPeriod = true;
                break;
            }
        }
        if (!isInDeadPeriod) {
            fNEventsSelected++;
            return fEvent;
        }
    }

    fNEventsRejected++;
    return nullptr;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestEventTimeSelectionProcess::EndProcess() {
    // Write here the jobs to do when all the events are processed
}

///////////////////////////////////////////////
/// \brief Function to get the cut string that reproduce the time selection
/// done by this process (useful for TRestDataSet::MakeCut() for example).
/// \note The cut string can be really long if there are many time ranges and
/// this may cause the following error
/// 'Error in <TTreeFormula::Compile>:  Too many operators !' when trying to
/// use the cut in TTree->Draw(). In such case, use
/// \code
/// ROOT::v5::TFormula::SetMaxima(10000) // or any other big enough number
/// \endcode
/// to increase the maximum number of operators allowed in a formula.
///
std::string TRestEventTimeSelectionProcess::GetTimeStampCut(std::string timeStampObsName, Bool_t useOffset,
                                                            Int_t nTimes) {
    std::string timeCut = "";
    std::string timeStampObsNameWithOffset = timeStampObsName;
    if (useOffset) {
        timeStampObsNameWithOffset += "+" + to_string(fTimeOffsetInSeconds);
    }
    if (nTimes < 0) nTimes = fStartEndTimes.size();
    Int_t c = 0;
    for (auto id : fStartEndTimes) {
        if (c++ >= nTimes) break;
        auto startTime = StringToTimeStamp(id.first);
        auto endTime = StringToTimeStamp(id.second);
        // Reduce the time by the margin in both sides
        startTime += fTimeStartMarginInSeconds;
        endTime -= fTimeEndMarginInSeconds;
        if (!timeCut.empty()) {
            if (fIsActiveTime)
                timeCut += " || ";
            else
                timeCut += " && ";
        }
        if (!fIsActiveTime) timeCut += "!";
        timeCut += "(";
        timeCut += timeStampObsNameWithOffset + ">=" + to_string(startTime);
        timeCut += "&&";
        timeCut += timeStampObsNameWithOffset + "<=" + to_string(endTime);
        timeCut += ")";
    }
    return timeCut;
}
///////////////////////////////////////////////
/// \brief Prints on screen the process data members
///
void TRestEventTimeSelectionProcess::PrintMetadata() {
    BeginPrintProcess();
    std::string typeOfTime = fIsActiveTime ? "Active" : "Dead";

    RESTMetadata << "File with times: " << fFileWithTimes << RESTendl;
    // print periods
    RESTMetadata << "Offset time: " << fTimeOffsetInSeconds << " seconds" << RESTendl;
    RESTMetadata << "Start margin time: " << fTimeStartMarginInSeconds << " seconds" << RESTendl;
    RESTMetadata << "End margin time: " << fTimeEndMarginInSeconds << " seconds" << RESTendl;
    RESTMetadata << typeOfTime << " time periods: " << RESTendl;
    for (auto id : fStartEndTimes) {
        RESTMetadata << id.first << " to " << id.second << RESTendl;
    }

    if ((Int_t)(fTotalTimeInSeconds / 24 / 3600) != 0)  // order of days
        RESTMetadata << "Total " << typeOfTime << " time: " << fTotalTimeInSeconds / 24 / 3600 << " days"
                     << RESTendl;
    else if ((Int_t)(fTotalTimeInSeconds / 3600) != 0)  // order of hours
        RESTMetadata << "Total " << typeOfTime << " time: " << fTotalTimeInSeconds / 3600 << " hours"
                     << RESTendl;
    else if ((Int_t)(fTotalTimeInSeconds / 60) != 0)  // order of minutes
        RESTMetadata << "Total " << typeOfTime << " time: " << fTotalTimeInSeconds / 60 << " minutes"
                     << RESTendl;
    else
        RESTMetadata << "Total " << typeOfTime << " time: " << fTotalTimeInSeconds << RESTendl;

    RESTMetadata << "Number of events rejected: " << fNEventsRejected << " ("
                 << fNEventsRejected * 1. / (fNEventsRejected + fNEventsSelected) * 100 << " %)" << RESTendl;
    RESTMetadata << "Number of events selected: " << fNEventsSelected << " ("
                 << fNEventsSelected * 1. / (fNEventsRejected + fNEventsSelected) * 100 << " %)" << RESTendl;

    EndPrintProcess();
}
