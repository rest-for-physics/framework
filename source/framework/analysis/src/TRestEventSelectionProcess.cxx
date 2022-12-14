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
/// The TRestEventSelectionProcess allows procesing of selected events only.
///
/// There are two ways of selecting events:
///
/// * Providing a txt file with the IDs of the events to be processed (fileWithIDs).
/// It reads the list, if an event is not in the list it returns NULL,
/// ends the processing and continues with the next event.
///
/// * Providing a root file (fileWithIDs) and the conditions to select the events (conditions).
/// Only events that satisfy the conditions will be processed.
///
/// Examples for rml files:
/// <addProcess type="TRestEventSelectionProcess" name="evSelection" fileWithIDs="/path/to/file/IDs.txt"
/// value="ON"  verboseLevel="info"/>
///
/// <addProcess type="TRestEventSelectionProcess" name="evSelection" fileWithIDs="/path/to/file/File.root"
/// conditions="observable<N" value="ON"  verboseLevel="info"/>
///
/// <hr>
///
/// \warning **? REST is under continous development.** This documentation
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
/// 2021-Jan: Read IDs from txt
///              David Diez
///
/// 2021-Mar: Read IDs from root with conditions
///              David Diez
///
/// \class      TRestEventSelectionProcess
/// \author     Javier Galan
/// \author     David Diez
///
/// <hr>
///

#include "TRestEventSelectionProcess.h"

using namespace std;
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

    fEvent = nullptr;
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
///
void TRestEventSelectionProcess::InitProcess() {
    if (fFileWithIDs.substr(fFileWithIDs.length() - 4) == ".txt") {
        string line;
        ifstream File(fFileWithIDs);

        if (File.is_open()) {
            while (getline(File, line)) {
                fList.push_back(stoi(line));
            }
            File.close();
        }
    }

    else if (fFileWithIDs.substr(fFileWithIDs.length() - 4) == "root") {
        TRestRun* run = new TRestRun(fFileWithIDs);
        fList = run->GetEventIdsWithConditions(fConditions);
        delete run;
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestEventSelectionProcess::ProcessEvent(TRestEvent* inputEvent) {
    fEvent = inputEvent;

    for (unsigned int i = 0; i < fList.size(); i++) {
        if (fList[i] == fEvent->GetID()) {
            return fEvent;
        }
    }

    return nullptr;
}

///////////////////////////////////////////////
/// \brief Prints on screen the process data members
///
void TRestEventSelectionProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << "File with IDs: " << fFileWithIDs << RESTendl;
    if (fFileWithIDs.substr(fFileWithIDs.length() - 4) == "root") {
        RESTMetadata << "Conditions: " << fConditions << RESTendl;
    }

    EndPrintProcess();
}
