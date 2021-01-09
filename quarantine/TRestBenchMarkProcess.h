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

#ifndef RestProcess_TRestBenchMarkProcess
#define RestProcess_TRestBenchMarkProcess

#include <TH1D.h>

#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

//! A system performance monitor process for event flow rate, reading speed, cpu stress, etc
class TRestBenchMarkProcess : public TRestEventProcess {
   private:
    TRestEvent* fEvent;  //!

    int fCPUNumber;
    int fMemNumber;  // in kB
    pid_t fPid;

    double fRefreshRate;  // in Hz

    static thread* fMonitorThread;   //!
    static int fMonitorFlag;         //!   //0: return, 1: run
    static float fCPUUsageInPct;     //!
    static float fMemUsageInMB;      //!
    static float fReadingInMBs;      //!
    static float fProcessSpeedInHz;  //!
    static int fLastEventNumber;     //!
    static ULong64_t fStartTime;     //!

    void InitFromConfigFile();

    void Initialize();

    void SysMonitorFunc(int pid, double refreshrate = 1);

   protected:
   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void PrintMetadata();

    // Constructor
    TRestBenchMarkProcess();
    // Destructor
    ~TRestBenchMarkProcess() {}

    ClassDef(TRestBenchMarkProcess, 1);  // Template for a REST "event process" class inherited from
                                         // TRestEventProcess
};
#endif
