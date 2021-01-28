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
/// The TRestEventSelectionProcess ...
///
/// **TODO**: Needs to be documented and added to validation pipeline.
///
/// <hr>
///
/// \warning **⚠ REST is under continous development.** This documentation
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
///
/// 2021-Jan: Created empty template
///             Javier Galan
///
/// \class      TRestEventSelectionProcess
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestEventSelectionProcess.h"

ClassImp(TRestEventSelectionProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestEventSelectionProcess::TRestEventSelectionProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
/// In this case we re-use the initialization of TRestRawToSignalProcess
/// interface class.
///
void TRestEventSelectionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fEvent = NULL;
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
/// TODO It should fill the fEventIDs vector.
///
void TRestEventSelectionProcess::InitProcess() {
    /// TOBE implemented
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestEventSelectionProcess::ProcessEvent(TRestEvent* eventInput) {
    fEvent = eventInput;

    // TOBE implemented. If the event ID fEvent->GetID() is not in the list
    // we should return NULL;

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Initialization of the process from RML
///
void TRestEventSelectionProcess::InitFromConfigFile() {
    /// TOBE implemented. Read event ids, or define filename used to identify event ids
}

///////////////////////////////////////////////
/// \brief Prints on screen the process data members
///
void TRestEventSelectionProcess::PrintMetadata() {
    BeginPrintProcess();

    /// TOBE implemented. Probably it should print just the event Ids that are in the selection.

    EndPrintProcess();
}
