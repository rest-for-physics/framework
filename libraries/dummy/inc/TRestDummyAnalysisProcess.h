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

#ifndef RestCore_TRestDummyAnalysisProcess
#define RestCore_TRestDummyAnalysisProcess

#include "TRestEventProcess.h"

//! TODO Write a one line brief description here!
class TRestDummyAnalysisProcess : public TRestEventProcess {
   private:
    // If the member is followed by //! it will not be written to disk.
    // If the member is followed by //< it will be registered inside the object when written to disk./
    //
    // We use //! usually when we just want to have an auxiliar variable to be initialized and available
    // by ProcessEvent or other methods. I.e. fReadout, fGas, ...

    /// A pointer to the event data
    TRestEvent* fEvent = NULL;  //!

    /// This is just a dummy member.
    Double_t fMember;  //<

   protected:
    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDummyAnalysisProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "DummyAnalysisProcess"; }

    TRestDummyAnalysisProcess();
    TRestDummyAnalysisProcess(char* cfgFileName);

    ~TRestDummyAnalysisProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestDummyAnalysisProcess, 1);
};
#endif
