#include "TRestMessagerAndReciever.h"

#include <sys/ipc.h>
#include <sys/shm.h>

#include "TRestDataBase.h"
#include "TRestManager.h"
#include "TRestProcessRunner.h"
#include "TRestStringOutput.h"

ClassImp(TRestMessagerAndReciever);

//______________________________________________________________________________
TRestMessagerAndReciever::TRestMessagerAndReciever() { Initialize(); }

TRestMessagerAndReciever ::~TRestMessagerAndReciever() {
    // clear the shared memories
    for (int i = 0; i < fMessagePools.size(); i++) {
        shmdt(fMessagePools[i]);
    }
}

#define SHMFLAG_CREATEUNIQUE (0640 | IPC_CREAT | IPC_EXCL)
#define SHMFLAG_CREATEOROPEN (0640 | IPC_CREAT)
#define SHMFLAG_OPEN (0640)

// <TRestMessagerAndReciever mode="host">
//    <pool name="chainfile" messageSource="outputfile" token="116027"/>
// </TRestMessagerAndReciever>
void TRestMessagerAndReciever::InitFromConfigFile() {
    fRun = fHostmgr != NULL ? fHostmgr->GetRunInfo() : NULL;
    string modestr = GetParameter("mode", "auto");
    if (ToUpper(modestr) == "HOST") {
        fMode = MessagePool_Host;
    } else if (ToUpper(modestr) == "CLIENT") {
        fMode = MessagePool_Client;
    } else {
        fMode = MessagePool_Auto;
    }

    TiXmlElement* poolele = fElement->FirstChildElement("pool");
    while (poolele != NULL) {
        string token = GetParameter("token", poolele, "116027");
        string name = GetParameter("name", poolele, "default");
        string source = GetParameter("messageSource", poolele, "OUTPUTFILE");
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
                    warning << "TRestMessagerAndReciever: unknown error!" << endl;
                    poolele = poolele->NextSiblingElement("pool");
                    continue;
                } else {
                    created = true;
                }
            } else {
                warning << "TRestMessagerAndReciever: shmget error!" << endl;
                warning << "Shared memory not deleted? type \"ipcrm -m " << shmid << "\" in the bash" << endl;
                poolele = poolele->NextSiblingElement("pool");
                continue;
            }
        } else if (fMode == MessagePool_Client) {
            if (shmid == -1) {
                warning << "TRestMessagerAndReciever: shmget error!" << endl;
                warning << "Shared memory not initialized? Launch Host process first!" << endl;
                poolele = poolele->NextSiblingElement("pool");
                continue;
            }
        } else if (fMode == MessagePool_Auto) {
            if (shmid == -1) {
                shmid = shmget(key, 30000, SHMFLAG_CREATEUNIQUE);
                if (shmid == -1) {
                    warning << "TRestMessagerAndReciever: unknown error!" << endl;
                    poolele = poolele->NextSiblingElement("pool");
                    continue;
                } else {
                    created = true;
                }
            }
        }

        messagepool_t* message = (messagepool_t*)shmat(shmid, NULL, 0);
        if (message == NULL) {
            printf("shmat error\n");
            poolele = poolele->NextSiblingElement("pool");
            continue;
        }
        if (created) {
            message->Reset();
            strcpy(message->name, name.c_str());
            cout << "Created shared memory: " << shmid << endl;
        } else {
            cout << "Attached to shared memory: " << shmid << endl;
        }

        fShmIds.push_back(shmid);
        fMessagePools.push_back(message);
        fPoolTokens.push_back(token);
        fPoolSources.push_back(source);

        poolele = poolele->NextSiblingElement("pool");
    }
}

bool TRestMessagerAndReciever::lock(messagepool_t* pool, int timeoutMs) {
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

bool TRestMessagerAndReciever::unlock(messagepool_t* pool, int timeoutMs) {
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

int TRestMessagerAndReciever::GetPoolId(string poolName) {
    for (int i = 0; i < fMessagePools.size(); i++) {
        if (strcmp(fMessagePools[i]->name, poolName.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}

void TRestMessagerAndReciever::AddPool(string message, int poolid) {
    if (message.size() > MsgLength) {
        message = message.substr(0, MsgLength - 2);
    }
    if (message == "") {
        warning << "cannot add empty message!" << endl;
        return;
    }
    if (poolid == -1) {
        return;
    }

    messagepool_t* pool = fMessagePools[poolid];
    if (!lock(pool)) {
        warning << "cannot add message to pool: " << poolid << ": lock failed!" << endl;
        return;
    }

    int pos = pool->RequirePos();
    if (pos !=-1) {
        messagepool_t::message_t msg;
        msg.provider = this;
        strcpy(msg.content, message.c_str());
        memcpy(&pool->messages[pos], &msg, sizeof(msg));
    }


    unlock(pool);
}

void TRestMessagerAndReciever::AddPool(string message, string poolName) {
    int id = GetPoolId(poolName);
    if (id == -1) {
        warning << "cannot find message pool with name: " << poolName << endl;
        return;
    }
    AddPool(message, id);
}

void TRestMessagerAndReciever::SendMessage(string poolName, string message) {
    int id;
    if (poolName == "" && fMessagePools.size() > 0) {
        id = 0;
    } else {
        id = GetPoolId(poolName);
    }
    if (id == -1) {
        warning << "cannot find message pool with name: " << poolName << endl;
        return;
    }

    string source = fPoolSources[id];
    if (message == "") {
        if (ToUpper(source) == "OUTPUTFILE") {
            if (fRun != NULL) {
                message = fRun->GetOutputFileName();
            }
        } else if (ToUpper(source) == "TIME") {
            message = ToDateTimeString(time(0));
        } else {
            message = source;
        }
    }

    AddPool(message, id);
}

vector<string> TRestMessagerAndReciever::ShowMessagePool(string poolName) {
    vector<string> result;

    int id;
    if (poolName == "" && fMessagePools.size() > 0) {
        id = 0;
    } else {
        id = GetPoolId(poolName);
    }
    if (id == -1) {
        warning << "cannot find message pool with name: " << poolName << endl;
        return result;
    }

    if (!lock(fMessagePools[id])) {
        warning << "cannot read message to pool: " << poolName << ": lock failed!" << endl;
        return result;
    }

    for (int i = 0; i < Nmsg; i++) {
        if (!fMessagePools[id]->messages[i].IsEmpty()) {
            string msg = string(fMessagePools[id]->messages[i].content);
            if (msg != "") {
                result.push_back(msg);
            }
        }
    }

    unlock(fMessagePools[id]);
    return result;
}

string TRestMessagerAndReciever::ConsumeMessage(string poolName) {
    int id;
    if (poolName == "" && fMessagePools.size() > 0) {
        id = 0;
    } else {
        id = GetPoolId(poolName);
    }
    if (id == -1) {
        warning << "cannot find message pool with name: " << poolName << endl;
        return "";
    }

    if (!lock(fMessagePools[id])) {
        warning << "cannot read message to pool: " << poolName << ": lock failed!" << endl;
        return "";
    }

    string msg = "";
    for (int i = 0; i < Nmsg; i++) {
        if (!fMessagePools[id]->messages[i].IsEmpty()) {
            // the process shall not consume the message provided by itself
            if (fMessagePools[id]->messages[i].provider != this) {
                // form the message
                msg = string(fMessagePools[id]->messages[i].content);
                // clear this message because it will be consumed
                fMessagePools[id]->messages[i].Reset();
                break;
            }
        }
    }

    unlock(fMessagePools[id]);
    return msg;
}

void TRestMessagerAndReciever::Initialize() {
    fRun = NULL;
    fMode = MessagePool_Auto;
}

void TRestMessagerAndReciever::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Connected message pools : " << endl;
    for (int i = 0; i < fMessagePools.size(); i++) {
        metadata << fMessagePools[i]->name << ": (token: " << fPoolTokens[i] << ", shmid: " << fShmIds[i]
                 << ", source: " << fPoolSources[i] << ")" << endl;
    }
    metadata << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    metadata << endl;
    metadata << endl;
}
