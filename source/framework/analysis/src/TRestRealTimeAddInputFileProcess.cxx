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
/// A process that does nothing to dataflow but starts a server listening through 
/// TRestMessenger metadata interface to get new files to add to TRestRun. 
/// 
/// Once placed in the process chain, it will call TRestRun to hang up at the 
/// end of external file. Then it will listen through TRestMessenger specific 
/// message. If the message is valid, it will call TRestRun to add file lively
/// 
/// For example, in bash window 1, we launch restManager using example rml: 
/// 
/// \code
/// restManager --c realtimeCoboAnalysis.rml --d 7786 --batch --i "/data2/2MM/M1/graw/CoBo_AsAd*_2020-07-13T13:43:13*_0000.graw" --o abc.root
/// \endcode
/// 
/// Here `--d 7786` means we define the run number as 7786. REST will run the process
/// And hang up after the file is finished. Then in bash window 2, we run: 
/// 
/// \code
/// restSendMessage 116027 "7786->/data2/2MM/M1/graw/CoBo_AsAd2_2020-07-13T13:43:13.698_0001.graw"
/// \endcode
/// 
/// Here 116027 is a token for TRestMessenger connection, defined in the previous rml. 
/// This command means sending a message to token 116027 poll telling to add a new file 
/// to run 7786. We can see in window 1 the process runs again. 
/// Finally in window 2 we input:
/// 
/// \code
/// restSendMessage 116027 "7786->finish"
/// \endcode
/// 
/// We can see from window 1 the process quit normally after all the files has been processed.
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

    if (fMonitorThread == NULL) {
        fRunInfo->HangUpEndFile();
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
