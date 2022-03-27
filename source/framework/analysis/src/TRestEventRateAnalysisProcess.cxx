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
/// This process adds observables related to the event time and measured rate
/// to the analysis tree.
///
/// Inside a TRestProcessRunner chain this process might be added
///  as follows:
///
/// \code
/// <addProcess type="TRestEventRateAnalysisProcess" name="rate" observables="all" />
/// \endcode
///
/// or
///
/// \code
/// <addProcess type="TRestEventRateAnalysisProcess" name="rate">
/// 	<observable name="MeanRate_InHz" />
/// </addProcess>
/// \endcode
///
///
/// ### Observables
///
///
/// * **SecondsFromStart**: It takes the time of the event and subtracts the
/// time of
/// the first event.
/// * **HoursFromStart**: SecondsFromStart divided by 3600.
/// * **EventTimeDelay**: It counts the time from the previous event to the
/// present one.
/// * **MeanRate_InHz**: It records the mean rate using the last 10 events. It
/// divides 10 by the time in seconds between the first and the last entry.
///
///
/// You may add filters to any observable inside the analysis tree. To add a cut,
/// write "cut" sections in your rml file:
///
/// \code
/// <TRestEventRateAnalysisProcess name=""  ... >
///     <cut name="MeanRate_InHz" value="(0,10)" />
/// </TRestEventRateAnalysisProcess>
/// \endcode
///
/// <hr>
///
/// \warning **⚠ REST is under continous development.** This documentation
/// is offered to you by the REST community. Your HELP is needed to keep this
/// code up to date. Your feedback will be worth to support this software, please
/// report any problems/suggestions you may find will using it at [The REST Framework
/// forum](http://ezpc10.unizar.es). You are welcome to contribute fixing typos,
/// updating information or adding/proposing new contributions. See also our
/// <a href="https://github.com/rest-for-physics/framework/blob/master/CONTRIBUTING.md">Contribution
/// Guide</a>.
///
///
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2022-February: Transfering rate observables from TRestRawSignalAnalysisProcess
///
/// \class      TRestEventRateAnalysisProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestEventRateAnalysisProcess.h"

#include "TRestDataBase.h"
using namespace std;

ClassImp(TRestEventRateAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestEventRateAnalysisProcess::TRestEventRateAnalysisProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestEventRateAnalysisProcess::~TRestEventRateAnalysisProcess() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestEventRateAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fEvent = NULL;

    fSingleThreadOnly = true;

    fFirstEventTime = -1;
    fPreviousEventTime.clear();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestEventRateAnalysisProcess::InitProcess() {
    if (fRunInfo->GetStartTimestamp() != 0) {
        fFirstEventTime = fRunInfo->GetStartTimestamp();
    } else {
        fFirstEventTime = -1;
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestEventRateAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fEvent = evInput;

    if (fFirstEventTime == -1) fFirstEventTime = fEvent->GetTime();

    Double_t secondsFromStart = fEvent->GetTime() - fFirstEventTime;
    SetObservableValue("SecondsFromStart", secondsFromStart);
    SetObservableValue("HoursFromStart", secondsFromStart / 3600.);

    Double_t evTimeDelay = 0;
    if (fPreviousEventTime.size() > 0) evTimeDelay = fEvent->GetTime() - fPreviousEventTime.back();
    SetObservableValue("EventTimeDelay", evTimeDelay);

    Double_t meanRate = 0;
    if (fPreviousEventTime.size() == 10) meanRate = 10. / (fEvent->GetTime() - fPreviousEventTime.front());
    SetObservableValue("MeanRate_InHz", meanRate);

    if (GetVerboseLevel() >= REST_Debug) {
        for (auto i : fObservablesDefined) {
            fAnalysisTree->PrintObservable(i.second);
        }
    }

    fPreviousEventTime.push_back(fEvent->GetTimeStamp());
    if (fPreviousEventTime.size() > 10) fPreviousEventTime.erase(fPreviousEventTime.begin());

    // If cut condition matches the event will be not registered.
    if (ApplyCut()) return NULL;

    return fEvent;
}
