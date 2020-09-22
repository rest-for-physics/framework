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

#ifndef RestCore_TRestHitsGaussAnalysisProcess
#define RestCore_TRestHitsGaussAnalysisProcess

#include <TH1D.h>

#include <TCanvas.h>

#include <TRestDetectorReadout.h>
#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from Hits type of data performing gaussian fits.
class TRestHitsGaussAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestHitsEvent

    TRestHitsEvent* fInputHitsEvent;   //!
    TRestHitsEvent* fOutputHitsEvent;  //!

    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
   public:
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestHitsGaussAnalysisProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "hitsGaussAnalysis"; }

    TRestHitsGaussAnalysisProcess();
    TRestHitsGaussAnalysisProcess(char* cfgFileName);

    ~TRestHitsGaussAnalysisProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestHitsGaussAnalysisProcess, 1);
};
#endif
