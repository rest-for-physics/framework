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

#ifndef RestCore_TRestRawVetoAnalysisProcess
#define RestCore_TRestRawVetoAnalysisProcess

#include "TRestRawSignalEvent.h"
#include "TRestEventProcess.h"

//! A template process to serve as an example to create new TRestRawSignalEventProcess
class TRestRawVetoAnalysisProcess : public TRestEventProcess {
   private:

	/// The daq channel id that will be considered as a veto signal
	Int_t fVetoSignalId; //<

	/// The range used to calculate the baseline parameters from the veto signal
    TVector2 fBaseLineRange; //<


	/// The range used to calculate the veto signal parameters
    TVector2 fRange; //<

    /// A pointer to the specific TRestRawSignalEvent
    TRestRawSignalEvent* fInputRawSignalEvent;   //!
    TRestRawSignalEvent* fOutputRawSignalEvent;  //!

    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:

   public:
    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestRawVetoAnalysisProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "vetoAnalysis"; }

    TRestRawVetoAnalysisProcess();
    TRestRawVetoAnalysisProcess(char* cfgFileName);

    ~TRestRawVetoAnalysisProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestRawVetoAnalysisProcess, 1);
};
#endif
