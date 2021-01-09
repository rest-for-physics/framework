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

#ifndef RestCore_TRestMessengerAndReceiver
#define RestCore_TRestMessengerAndReceiver

#include "TRestMetadata.h"
#include "TRestRun.h"

class TRestMessengerAndReceiver : public TRestMetadata {
   private:
/////////////// through shm  //////////////
#define Nmsg 100
#define MsgLength 256
    struct messagepool_t {
        void* owner;
        char name[MsgLength];
        struct message_t {
            void* provider;
            char content[MsgLength];
            void Reset() {
                provider = NULL;
                for (int i = 0; i < MsgLength; i++) {
                    content[i] = 0;
                }
            }
            bool IsEmpty() { return provider == NULL || content[0] == 0; }
        };
        message_t messages[Nmsg];  //! max 100 messages, each 256 char length
        void Reset() {
            owner = NULL;
            for (int i = 0; i < MsgLength; i++) {
                name[i] = 0;
            }
            for (int i = 0; i < Nmsg; i++) {
                messages[i].Reset();
            }
        }
        int RequirePos() {
            for (int i = 0; i < Nmsg; i++) {
                if (messages[i].IsEmpty()) {
                    return i;
                }
            }
            return -1;
        }
    };
    int fShmId;                   //!
    messagepool_t* fMessagePool;  //!

   protected:
    enum CommMode { MessagePool_Host, MessagePool_Client, MessagePool_TwoWay };

    TRestRun* fRun;  //!

    CommMode fMode;
    string fPoolToken;   // to establish communication
    string fPoolSource;  // describes the source of message to be send. e.g. OUTPUTFILE, RUNNUMBER

    virtual void InitFromConfigFile();

    virtual void Initialize();

    virtual void AddPool(string message);

    bool lock(messagepool_t* pool, int timeoutMs = 1000);

    bool unlock(messagepool_t* pool, int timeoutMs = 1000);

   public:
    virtual bool IsConnected() { return fMessagePool != NULL; }

    virtual void SendMessage(string message = "");

    virtual vector<string> ShowMessagePool();

    virtual string ConsumeMessage();

    virtual void PrintMetadata();
    // Constructor
    TRestMessengerAndReceiver();
    // Constructor
    TRestMessengerAndReceiver(int token, string mode = "TwoWay");
    // Destructor
    ~TRestMessengerAndReceiver();

    ClassDef(TRestMessengerAndReceiver, 1);  // Template for a REST "event process" class inherited from
                                             // TRestEventProcess
};
#endif
