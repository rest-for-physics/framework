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
/// TRestMessengerAndReceiver
///
/// This metadata helps to recieve/dispatch messages across processes(UNIX process)
/// For example, we start two restManager with TRestMessengerAndReceiver added as metadata.
/// For one of them, we add a "sendMessage" task after "processEvent". Then, when the process
/// chain finishes, it will dispatch the message containing, e.g., the output file name.
/// Then, the other restManager can get the message and be notified that the file is done.
/// It can therefore do some operations to this file, e.g., add to TRestAnalysisPlot file list
/// and update the plots.
///
/// By default it uses shared memory. Can be overridden by REST packages, adding more
/// communication methods(http, kafka, etc.)
///
/// Each TRestMessengerAndReceiver connects to a message pool in shared memory. Each message pool
/// is limited with 100 messages. Each message is limited with 256 bytes. When sending
/// message, the message is added to the message pool. When receiving message, the logic
/// is more like "consuming": message is taken out from the pool and given to the specific
/// process. It will be erased after being consumed. TRestMessengerAndReceiver cannot consume
/// the message sent by self process.
///
/// The rml definition is like follows. We need to add serval <pool sections in its config
/// section to define the message pools. The token of the pool is equivalent to the shm key,
/// where two processes with a same token can access to a same memory.
///
/// ```
/// <TRestManager name="CoBoDataAnalysis" title="Example" verboseLevel="info" >
///   <TRestMessengerAndReceiver name="Messager" title="Example" verboseLevel="info"
///     messageSource="outputfile" token="116027"/>
///   <addTask command="Messager->SendMessage()" value="ON"/>
/// </TRestManager>
/// ```
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-Aug:   First implementation and concept
///             Ni Kaixiang
///
/// \class      TRestMessengerAndReceiver
/// \author     Ni Kaixiang
///
/// <hr>
///

#include "TRestMessengerAndReceiver.h"

#include <sys/ipc.h>
#include <sys/shm.h>

#include "TRestDataBase.h"
#include "TRestManager.h"
#include "TRestProcessRunner.h"
#include "TRestStringOutput.h"

ClassImp(TRestMessengerAndReceiver);

//______________________________________________________________________________
TRestMessengerAndReceiver::TRestMessengerAndReceiver() { Initialize(); }

TRestMessengerAndReceiver::~TRestMessengerAndReceiver() {
    // clear the shared memories
    if (fMessagePool != NULL) {
        if (fMessagePool->owner == this) {
            unlock(fMessagePool);
        }
        shmdt(fMessagePool);
    }
}

TRestMessengerAndReceiver::TRestMessengerAndReceiver(int token, string mode) {
    Initialize();
    LoadConfigFromElement(StringToElement("<TRestMessengerAndReceiver token=\"" + ToString(token) +
                                          "\" mode=\"" + mode + "\"/>"),
                          NULL, {});
}

void TRestMessengerAndReceiver::Initialize() {
    fRun = NULL;
    fMode = MessagePool_TwoWay;
}

#define SHMFLAG_CREATEUNIQUE (0640 | IPC_CREAT | IPC_EXCL)
#define SHMFLAG_CREATEOROPEN (0640 | IPC_CREAT)
#define SHMFLAG_OPEN (0640)

// Example rml structure:
//   <TRestMessengerAndReceiver name="Messager" title="Example" verboseLevel="info"
//     messageSource="outputfile" token="116027" mode="auto"/>
void TRestMessengerAndReceiver::InitFromConfigFile() {
    fRun = fHostmgr != NULL ? fHostmgr->GetRunInfo() : NULL;
    string modestr = GetParameter("mode", "twoway");
    if (ToUpper(modestr) == "HOST") {
        fMode = MessagePool_Host;
    } else if (ToUpper(modestr) == "CLIENT") {
        fMode = MessagePool_Client;
    } else if (ToUpper(modestr) == "TWOWAY" || ToUpper(modestr) == "AUTO") {
        fMode = MessagePool_TwoWay;
    }

    string token = GetParameter("token", "116027");
    string source = GetParameter("messageSource", "OUTPUTFILE");
    key_t key = StringToInteger(token);
    int flag;
    if (fMode == MessagePool_Host) {
        flag = SHMFLAG_CREATEUNIQUE;
    } else {
        flag = SHMFLAG_OPEN;
    }

    bool created = false;
    int shmid = shmget(key, sizeof(messagepool_t), SHMFLAG_OPEN);
    if (fMode == MessagePool_Host) {
        if (shmid == -1) {
            shmid = shmget(key, 30000, SHMFLAG_CREATEUNIQUE);
            if (shmid == -1) {
                warning << "TRestMessengerAndReceiver: unknown error!" << endl;
                return;
            } else {
                created = true;
            }
        } else {
            warning << "TRestMessengerAndReceiver: shmget error!" << endl;
            warning << "Shared memory not deleted? type \"ipcrm -m " << shmid << "\" in the bash" << endl;
            return;
        }
    } else if (fMode == MessagePool_Client) {
        if (shmid == -1) {
            warning << "TRestMessengerAndReceiver: shmget error!" << endl;
            warning << "Shared memory not initialized? Launch Host process first!" << endl;
            return;
        }
    } else if (fMode == MessagePool_TwoWay) {
        if (shmid == -1) {
            shmid = shmget(key, 30000, SHMFLAG_CREATEUNIQUE);
            if (shmid == -1) {
                warning << "TRestMessengerAndReceiver: unknown error!" << endl;
                return;
            } else {
                created = true;
            }
        }
    }

    messagepool_t* message = (messagepool_t*)shmat(shmid, NULL, 0);
    if (message == NULL) {
        printf("shmat error\n");
        return;
    }

    if ((string)this->GetName() == "defaultName") SetName(message->name);

    if (created) {
        message->Reset();
        strcpy(message->name, this->GetName());
        cout << "Created shared memory: " << shmid << endl;
    } else {
        if (strcmp(message->name, this->GetName()) != 0) {
            warning << "TRestMessengerAndReceiver: connected message pool name(" << message->name
                    << ") is different with this(" << this->GetName() << ")!" << endl;
        }
        cout << "Connected to shared memory: " << shmid << endl;
    }

    fShmId = shmid;
    fMessagePool = message;
    fPoolToken = token;
    fPoolSource = source;
}

bool TRestMessengerAndReceiver::lock(messagepool_t* pool, int timeoutMs) {
    int i = 0;
    while (pool->owner != NULL && pool->owner != (void*)this) {
        usleep(1000);
        i++;
        if (i >= timeoutMs) {
            return false;
        }
    }

    pool->owner = (void*)this;

    return true;
}

bool TRestMessengerAndReceiver::unlock(messagepool_t* pool, int timeoutMs) {
    int i = 0;
    while (pool->owner != NULL && pool->owner != (void*)this) {
        usleep(1000);
        i++;
        if (i >= timeoutMs) {
            ferr << "unlocking pool failed!" << endl;
            abort();
        }
    }

    pool->owner = NULL;

    return true;
}

void TRestMessengerAndReceiver::AddPool(string message) {
    if (!IsConnected()) {
        warning << "TRestMessengerAndReceiver: Not connected!" << endl;
        return;
    }

    if (message.size() > MsgLength) {
        message = message.substr(0, MsgLength - 2);
    }
    if (message == "") {
        warning << "cannot add empty message!" << endl;
        return;
    }

    messagepool_t* pool = fMessagePool;
    if (!lock(pool)) {
        warning << "cannot add message to pool: " << pool->name << ": lock failed!" << endl;
        return;
    }

    int pos = pool->RequirePos();
    if (pos != -1) {
        messagepool_t::message_t msg;
        msg.provider = this;
        strcpy(msg.content, message.c_str());
        memcpy(&pool->messages[pos], &msg, sizeof(msg));
    } else {
        warning << "cannot send message: message pool is full!" << endl;
    }

    unlock(pool);
}

void TRestMessengerAndReceiver::SendMessage(string message) {
    if (!IsConnected()) {
        warning << "TRestMessengerAndReceiver: Not connected!" << endl;
        return;
    }
    if (fMode == MessagePool_Client) {
        warning << "TRestMessengerAndReceiver: Forbidden to send message from client!" << endl;
        return;
    }

    if (message == "") {
        if (ToUpper(fPoolSource) == "OUTPUTFILE") {
            if (fRun != NULL) {
                message = fRun->GetOutputFileName();
            }
        } else if (ToUpper(fPoolSource) == "TIME") {
            message = ToDateTimeString(time(0));
        } else {
            message = fPoolSource;
        }
    }

    AddPool(message);
}

vector<string> TRestMessengerAndReceiver::ShowMessagePool() {
    vector<string> result;

    if (!IsConnected()) {
        warning << "TRestMessengerAndReceiver: Not connected!" << endl;
        return result;
    }

    if (!lock(fMessagePool)) {
        warning << "cannot read message to pool: " << fMessagePool->name << ": lock failed!" << endl;
        return result;
    }

    for (int i = 0; i < Nmsg; i++) {
        if (!fMessagePool->messages[i].IsEmpty()) {
            string msg = string(fMessagePool->messages[i].content);
            if (msg != "") {
                result.push_back(msg);
            }
        }
    }

    unlock(fMessagePool);
    return result;
}

string TRestMessengerAndReceiver::ConsumeMessage() {
    if (!IsConnected()) {
        warning << "TRestMessengerAndReceiver: Not connected!" << endl;
        return "";
    }
    if (fMode == MessagePool_Host) {
        warning << "TRestMessengerAndReceiver: Forbidden to consume message from host!" << endl;
        return "";
    }

    if (!lock(fMessagePool)) {
        warning << "cannot read message to pool: " << fMessagePool->name << ": lock failed!" << endl;
        return "";
    }

    string msg = "";
    for (int i = 0; i < Nmsg; i++) {
        if (!fMessagePool->messages[i].IsEmpty()) {
            // the process shall not consume the message provided by itself
            if (fMessagePool->messages[i].provider != this) {
                // form the message
                msg = string(fMessagePool->messages[i].content);
                // clear this message because it will be consumed
                fMessagePool->messages[i].Reset();
                break;
            }
        }
    }

    unlock(fMessagePool);
    return msg;
}

void TRestMessengerAndReceiver::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    if (IsConnected()) {
        metadata << "Connected : "
                 << " (token: " << fPoolToken << ", shmid: " << fShmId << ", source: " << fPoolSource << ")"
                 << endl;
    } else {
        metadata << "Not Connected" << endl;
    }
    metadata << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    metadata << endl;
    metadata << endl;
}
