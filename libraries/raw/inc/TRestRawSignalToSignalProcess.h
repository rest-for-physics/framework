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

#ifndef RestCore_TRestRawSignalToSignalProcess
#define RestCore_TRestRawSignalToSignalProcess

#include <TRestRawSignalEvent.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

//! A process to convert a TRestRawSignalEvent into a TRestSignalEvent
class TRestRawSignalToSignalProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fInputSignalEvent;  //!

    /// A pointer to the specific TRestSignalEvent input
    TRestSignalEvent* fOutputSignalEvent;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    /// The sampling time used to transform the binned data to time information
    Double_t fSampling;

    /// The corresponding time of the first bin inside the raw signal
    Int_t fTriggerStarts;

    /// A factor the data values will be multiplied by at the output signal.
    Double_t fGain;

   public:
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        std::cout << "Sampling time : " << fSampling << " us" << std::endl;
        std::cout << "Trigger starts : " << fTriggerStarts << " us" << std::endl;
        std::cout << "Gain : " << fGain << std::endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestRawSignalToSignalProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "rawSignalToSignal"; }

    // Constructor
    TRestRawSignalToSignalProcess();
    TRestRawSignalToSignalProcess(char* cfgFileName);

    // Destructor
    ~TRestRawSignalToSignalProcess();

    ClassDef(TRestRawSignalToSignalProcess, 1);
};
#endif
