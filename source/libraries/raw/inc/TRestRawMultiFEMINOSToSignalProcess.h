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

#ifndef RestCore_TRestRawMultiFEMINOSToSignalProcess
#define RestCore_TRestRawMultiFEMINOSToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestDetectorSignalEvent.h"

//! A process to read Feminos acquisition cards in single or TCM mode.
class TRestRawMultiFEMINOSToSignalProcess : public TRestRawToSignalProcess {
   private:
    unsigned short pay;

    unsigned int fLastEventId;
    Double_t fLastTimeStamp;
    void LoadDetectorSetupData();

    Int_t fCounter = 0;  //!

   public:
    void InitProcess();
    void Initialize();
    TRestEvent* ProcessEvent(TRestEvent* evInput);
    TString GetProcessName() { return (TString) "MultiFEMINOSToSignal"; }

    Bool_t ReadFrame(void* fr, int fr_sz);

    // Constructor
    TRestRawMultiFEMINOSToSignalProcess();
    TRestRawMultiFEMINOSToSignalProcess(char* cfgFileName);
    // Destructor
    ~TRestRawMultiFEMINOSToSignalProcess();

    ClassDef(TRestRawMultiFEMINOSToSignalProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
