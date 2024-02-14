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

#ifndef RESTProc_TRestRealTimeAddInputFileProcess
#define RESTProc_TRestRealTimeAddInputFileProcess

#include "TRestEvent.h"
#include "TRestEventProcess.h"
#include "TRestMessenger.h"

class TRestRealTimeAddInputFileProcess : public TRestEventProcess {
   private:
    // We define specific input/output event data holders
    TRestEvent* fEvent;                  //!
    TRestMessenger* fMessenger;          //!
    static std::thread* fMonitorThread;  //!
    static int fMonitorFlag;             //!   //0: return, 1: run

    void InitFromConfigFile() override;

    void Initialize() override;

    // Add here the members or parameters for your event process.
    // You should set their initial values here together.
    // Note: add "//!" mark at the end of the member definition
    // if you don't want to save them as "metadata".

   public:
    RESTValue GetInputEvent() const override { return fEvent; }
    RESTValue GetOutputEvent() const override { return fEvent; }

    void InitProcess() override;

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void EndProcess() override;

    void PrintMetadata() override {
        BeginPrintProcess();

        // Write here how to print the added process members and parameters.

        EndPrintProcess();
    }

    void FileNotificationFunc();

    TRestRealTimeAddInputFileProcess();
    ~TRestRealTimeAddInputFileProcess();

    const char* GetProcessName() const override { return "RealTimeAddInputFileProcess"; }

    ClassDefOverride(TRestRealTimeAddInputFileProcess, 1);
};
#endif
