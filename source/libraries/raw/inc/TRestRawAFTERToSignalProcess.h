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

#ifndef RestCore_TRestRawAFTERToSignalProcess
#define RestCore_TRestRawAFTERToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestDetectorSignalEvent.h"

// ATENTION: new T2K Daq versions 2.X need to read one extra word
#define NEW_DAQ_T2K_2_X

//--------------------------------------------------------
// Structure acquisition data
//--------------------------------------------------------
struct EventHeader {
    int eventSize;
    int eventNumb;
    // int eventTime;
    // unsigned short dummy;
};

#ifdef NEW_DAQ_T2K_2_X
// ATENTION!!!!!
// New verison of the DaqT2K (2.x)
// added 30th July 2012 (JuanAn)
struct DataPacketHeader {
    unsigned short size;
    unsigned short dcc;
    unsigned short hdr;
    unsigned short args;
    unsigned short ts_h;
    unsigned short ts_l;
    unsigned short ecnt;
    unsigned short scnt;
};
#else
struct DataPacketHeader {
    unsigned short size;
    unsigned short hdr;
    unsigned short args;
    unsigned short ts_h;
    unsigned short ts_l;
    unsigned short ecnt;
    unsigned short scnt;
};
#endif

struct DataPacketEnd {
    unsigned short crc1;
    unsigned short crc2;
};

//! A process to read binary files produced with AFTER electronics
class TRestRawAFTERToSignalProcess : public TRestRawToSignalProcess {
   protected:
    unsigned int prevTime;
    double reducedTime;

   public:
    void Initialize();
    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* evInput);
    TString GetProcessName() { return (TString) "AFTERToSignal"; }
    TRestMetadata* GetProcessMetadata() { return NULL; }

    Bool_t isExternal() { return true; }

    TRestRawAFTERToSignalProcess();
    TRestRawAFTERToSignalProcess(char* cfgFileName);
    ~TRestRawAFTERToSignalProcess();

    ClassDef(TRestRawAFTERToSignalProcess, 1);
};
#endif
