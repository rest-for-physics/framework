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

#ifndef RestCore_TRestRawFindResponseSignalProcess
#define RestCore_TRestRawFindResponseSignalProcess

#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

//! A process to find a representative signal to generate a response signal
class TRestRawFindResponseSignalProcess : public TRestEventProcess {
   private:
    TRestRawSignalEvent* fInputSignalEvent;   //!
    TRestRawSignalEvent* fOutputSignalEvent;  //!

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fInputSignalEvent; }
    any GetOutputEvent() { return fOutputSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return NULL; }

    TString GetProcessName() { return (TString) "findResponseSignal"; }

    TRestRawFindResponseSignalProcess();
    TRestRawFindResponseSignalProcess(char* cfgFileName);
    ~TRestRawFindResponseSignalProcess();

    ClassDef(TRestRawFindResponseSignalProcess, 1);
};
#endif
