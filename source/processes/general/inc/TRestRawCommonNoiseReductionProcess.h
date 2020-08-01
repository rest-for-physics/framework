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

#ifndef RestCore_TRestRawCommonNoiseReductionProcess
#define RestCore_TRestRawCommonNoiseReductionProcess

#include <TRestRawSignalEvent.h>
#include "TRestEventProcess.h"

//! A REST process to reduce the Common Noise from RawSignal type of data.
class TRestRawCommonNoiseReductionProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fInputEvent;
    TRestRawSignalEvent* fOutputEvent;

// TRestRawSignalEvent *fSignalEvent;
// std::vector <std::string> fSignalAnalysisObservables;
#endif
    // Parameters
    Int_t fMode = 0;
    Int_t fbinNumber = 0;
    Int_t fcenterWidth = 0;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fInputEvent; }
    any GetOutputEvent() { return fOutputEvent; }

    void InitProcess();

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " fMode : [" << fMode << "]";
        if (fMode == 0) metadata << " --> Mode 0 activated." << endl;
        if (fMode == 1) metadata << " --> Mode 1 activated." << endl;
        metadata << " fcenterWidth : " << fcenterWidth << endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestRawCommonNoiseReductionProcess; }

    TString GetProcessName() { return (TString) "rsCommonNoiseReduction"; }

    // Constructor
    TRestRawCommonNoiseReductionProcess();
    TRestRawCommonNoiseReductionProcess(char* cfgFileName);

    // Destructor
    ~TRestRawCommonNoiseReductionProcess();

    ClassDef(TRestRawCommonNoiseReductionProcess,
             1);  // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

