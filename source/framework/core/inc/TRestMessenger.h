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

#ifndef RestCore_TRestMessenger
#define RestCore_TRestMessenger

#include "TRestMetadata.h"
#include "TRestRun.h"

class TRestMessenger : public TRestMetadata {
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
                provider = nullptr;
                for (int i = 0; i < MsgLength; i++) {
                    content[i] = 0;
                }
            }
            bool IsEmpty() { return provider == nullptr || content[0] == 0; }
        };
        message_t messages[Nmsg];  //! max 100 messages, each 256 char length
        void Reset() {
            owner = nullptr;
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
    std::string fPoolToken;   // to establish communication
    std::string fPoolSource;  // describes the source of message to be sent. e.g. OUTPUTFILE, RUNNUMBER

    virtual void InitFromConfigFile();

    virtual void Initialize();

    virtual void AddPool(std::string message);

    bool lock(messagepool_t* pool, int timeoutMs = 1000);

    bool unlock(messagepool_t* pool, int timeoutMs = 1000);

   public:
    virtual bool IsConnected() { return fMessagePool != nullptr; }

    virtual void SendMessage(std::string message = "");

    virtual vector<std::string> ShowMessagePool();

    virtual std::string ConsumeMessage();

    virtual void PrintMetadata();
    // Constructor
    TRestMessenger();
    // Constructor
    TRestMessenger(int token, std::string mode = "TwoWay");
    // Destructor
    ~TRestMessenger();

    ClassDef(TRestMessenger, 1);  // Template for a REST "event process" class inherited from
                                  // TRestEventProcess
};
#endif
