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

#ifndef RestCore_TRestDetectorHitsGaussAnalysisProcess
#define RestCore_TRestDetectorHitsGaussAnalysisProcess

#include <TH1D.h>

#include <TCanvas.h>

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from Hits type of data performing gaussian fits.
class TRestDetectorHitsGaussAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorHitsEvent

    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:

    /// The number of channels or strips in the X direction
    Int_t fReadoutChannelsX;

    /// The number of channels or strips in the Y direction
    Int_t fReadoutChannelsY;

    /// The last channel poistion in mm
    Int_t fEndChannelPosition;

    /// The first channel position in mm
    Int_t fStartChannelPosition;

    /// The pitch or size of the strips in mm
    Double_t fPitch;


   public:
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDetectorHitsGaussAnalysisProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "hitsGaussAnalysis"; }

    TRestDetectorHitsGaussAnalysisProcess();
    TRestDetectorHitsGaussAnalysisProcess(char* cfgFileName);

    ~TRestDetectorHitsGaussAnalysisProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestDetectorHitsGaussAnalysisProcess, 1);
};
#endif
