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

#ifndef RestCore_TRestMessagerAndReciever
#define RestCore_TRestMessagerAndReciever

#include "TRestMetadata.h"
#include "TRestRun.h"

// This metadata helps to recieve/dispatch messages across processes(UNIX process)
// For example, we start two restManager with TRestMessagerAndReciever, 
// one under dispatch mode and one under recieve mode. For the dispatching one,
// we add a message task after processEvent task. Then, when the process 
// chain finishes, it will dispatch the message, e.g., the output file name. 
// the recieving one will get the message and do some drawings to the output file
//
// Can be overridden by REST packages
class TRestMessagerAndReciever : public TRestMetadata {
   private:
    /////////////// through shm  //////////////
#define Nmsg 100
#define MsgLength 256
    struct messagepool {
        void* owner;
        int currentPos;
        char name[MsgLength];
        char messages[MsgLength][Nmsg];  //! max 100 messages, each 256 char length
        void Reset() {
            owner = NULL;
            for (int i = 0; i < MsgLength; i++) {
                name[i] = 0;
            }
            currentPos = 0;
            for (int i = 0; i < Nmsg; i++) {
                for (int j = 0; j < MsgLength; j++) {
                    messages[i][j] = 0;
                }
            }
        }
    };
    vector<int> fShmIds;                 //!
    vector<messagepool*> fMessagePools;                   //!

   protected:
    enum CommMode { MessagePool_Host, MessagePool_Client, MessagePool_Auto };

    TRestRun* fRun; //!

    CommMode fMode;  
    vector<string> fPoolTokens; // to establish communication  
    vector<string> fPoolSources;  // describes the source of message to be send. e.g. OUTPUTFILE, RUNNUMBER




    virtual void InitFromConfigFile();

    virtual void Initialize();

    void AddPool(string message, int poolid);

    virtual void AddPool(string message, string poolName);

    bool lock(messagepool* pool, int timeoutMs=1000);
    
    bool unlock(messagepool* pool, int timeoutMs = 1000);

    int GetPoolId(string poolName);

   public:
    virtual void SendMessage(string poolName = "", string message = "");

    virtual vector<string> ShowMessagePool(string poolName);

    virtual string ReadMessage(string poolName);

    virtual void PrintMetadata();
    // Constructor
    TRestMessagerAndReciever();
    // Destructor
    ~TRestMessagerAndReciever();

    ClassDef(TRestMessagerAndReciever, 1);  // Template for a REST "event process" class inherited from
                                         // TRestEventProcess
};
#endif
