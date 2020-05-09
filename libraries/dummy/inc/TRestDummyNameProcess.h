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

#ifndef RestCore_TRestDummyNameProcess
#define RestCore_TRestDummyNameProcess

#include "TRestDummyEvent.h"
#include "TRestEventProcess.h"

//! A template process to serve as an example to create new TRestDummyEventProcess
class TRestDummyNameProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDummyEvent
    TRestDummyEvent* fDummyEvent;  //!

    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
   public:
    any GetInputEvent() { return fDummyEvent; }
    any GetOutputEvent() { return fDummyEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDummyNameProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "dummyName"; }

    TRestDummyNameProcess();
    TRestDummyNameProcess(char* cfgFileName);

    ~TRestDummyNameProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestDummyNameProcess, 1);
};
#endif
