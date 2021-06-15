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
/////////////////////////////////////////////////////////////////////////
/// Write the process description Here
///
/// \class TRestRealTimeAddInputFileProcess
///
///----------------------------------------------------------------------
///
/// By REST process template generator
/// User: nkx
/// Date: 2021-6-10 22:23:16
///
/// <hr>
/////////////////////////////////////////////////////////////////////////

#include "TRestRealTimeAddInputFileProcess.h"

ClassImp(TRestRealTimeAddInputFileProcess);

thread* TRestRealTimeAddInputFileProcess::fMonitorThread = NULL;
int TRestRealTimeAddInputFileProcess::fMonitorFlag = 0;

TRestRealTimeAddInputFileProcess::TRestRealTimeAddInputFileProcess() { Initialize(); }

TRestRealTimeAddInputFileProcess::~TRestRealTimeAddInputFileProcess() {}

void TRestRealTimeAddInputFileProcess::Initialize() {
    SetSectionName(this->ClassName());
    fEvent = NULL;

    // Assign initial values for the parameters here
}

void TRestRealTimeAddInputFileProcess::InitProcess() {
    // Write here the jobs to do before processing
    fMessenger = GetMetadata<TRestMessenger>();

    if (fMessenger == NULL) {
        ferr << "messenger not found!" << endl;
        exit(1);
    }

    fRunInfo->HangUpEndFile();

    if (fMonitorThread == NULL) {
        fMonitorFlag = 1;
        // SysMonitorFunc(fPid, fRefreshRate);
        fMonitorThread = new thread(&TRestRealTimeAddInputFileProcess::FileNotificationFunc, this);
        fMonitorThread->detach();
    }
}

TRestEvent* TRestRealTimeAddInputFileProcess::ProcessEvent(TRestEvent* evInput) {
    fEvent = (TRestEvent*)evInput;

    // Write here the main logic of process: TRestRealTimeAddInputFileProcess
    // Read data from input event, write data to output event, and save observables to tree

    return fEvent;
}

void TRestRealTimeAddInputFileProcess::FileNotificationFunc() {
    while (fMonitorFlag == 1) {
        string message = fMessenger->ConsumeMessage();
        if (message != "") {
            essential << "Recieveing message: " << message << endl;
            // 7788->/data2/2MM/M1/graw/CoBo_AsAd1_2020-07-19T13:50:49.519_0000.graw
            // 7788->finish

            auto runid_filename = Split(message, "->");
            if (runid_filename.size() == 2) {
                int runid = atoi(runid_filename[0].c_str());
                if (runid == fRunInfo->GetRunNumber()) {
                    string msgContent = runid_filename[1];
                    if (TRestTools::fileExists(msgContent)) {
                        essential << "Adding file... " << endl;
                        fRunInfo->AddInputFileExternal(msgContent);
                    } else if (msgContent == "finish") {
                        fRunInfo->ReleaseEndFile();
                    } else {
                        warning << "illegal message!" << endl;
                    }
                } else {
                    // if the runnumber does not match, we put this message back to pool
                    // maybe other processes need it
                    fMessenger->SendMessage(message);
                }
            }
            else {
                warning << "illegal message!" << endl;
            }
        }
        usleep(1000);
    }
}

void TRestRealTimeAddInputFileProcess::EndProcess() {
    // Write here the jobs to do when all the events are processed
    if (fMonitorThread != NULL) {
        fMonitorFlag = 0;
        usleep(2000);
        // fMonitorThread->join();
        delete fMonitorThread;
        fMonitorThread = NULL;
    }
}

void TRestRealTimeAddInputFileProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();
    // Don't need to write code here besides the advanced logic of parameter loading
}
