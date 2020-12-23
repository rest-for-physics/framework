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

#ifndef RestCore_TRestDetectorSignalToRawSignalProcess
#define RestCore_TRestDetectorSignalToRawSignalProcess

#include <TRestRawSignalEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! A process to convert a TRestDetectorSignalEvent into a TRestRawSignalEvent
class TRestDetectorSignalToRawSignalProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorSignalEvent input
    TRestDetectorSignalEvent* fInputSignalEvent;  //!

    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fOutputRawSignalEvent;  //!

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    /// The sampling time from the binned raw output signal
    Double_t fSampling;

    /// The number of points of the resulting output signal
    Int_t fNPoints;

    /// It is used to define the way the time start will be fixed
    TString fTriggerMode;

    /// The number of time bins the time start is delayed in the resulting output
    /// signal.
    Int_t fTriggerDelay;

    /// A factor the data values will be multiplied by at the output signal.
    Double_t fGain;

    /// This parameter is used by integralWindow trigger mode to define the
    /// acquisition window.
    Double_t fIntegralThreshold;

   public:
    any GetInputEvent() { return fInputSignalEvent; }
    any GetOutputEvent() { return fOutputRawSignalEvent; }

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Sampling time : " << fSampling << " us" << endl;
        metadata << "Points per channel : " << fNPoints << endl;
        metadata << "Trigger mode : " << fTriggerMode << endl;
        metadata << "Trigger delay : " << fTriggerDelay << " time units" << endl;
        metadata << "ADC gain : " << fGain << endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDetectorSignalToRawSignalProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "signalToRawSignal"; }

    // Constructor
    TRestDetectorSignalToRawSignalProcess();
    TRestDetectorSignalToRawSignalProcess(char* cfgFileName);

    // Destructor
    ~TRestDetectorSignalToRawSignalProcess();

    ClassDef(TRestDetectorSignalToRawSignalProcess, 2);
};
#endif
