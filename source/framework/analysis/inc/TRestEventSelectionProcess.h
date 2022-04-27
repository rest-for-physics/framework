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

#ifndef RestProcess_TRestEventSelectionProcess
#define RestProcess_TRestEventSelectionProcess

#include <TH1D.h>

#include <iostream>

#include "TRestEventProcess.h"

//! A template process to serve as an example to create new TRestRawSignalEventProcess
class TRestEventSelectionProcess : public TRestEventProcess {
   private:
    TRestEvent* fEvent;  //!
    std::string fFileWithIDs = "";
    std::string fConditions = "";
    std::vector<Int_t> fList;

    /// A list with the event ids that have been selected.
    // std::vector<Int_t> fEventIds;  //<

    // void InitFromConfigFile();

    void Initialize() override;

   protected:
   public:
    any GetInputEvent() const override { return fEvent; }
    any GetOutputEvent() const override { return fEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    // Constructor
    TRestEventSelectionProcess();
    // Destructor
    ~TRestEventSelectionProcess() {}

    const char* GetProcessName() const override { return "EventSelectionProcess"; }

    ClassDefOverride(TRestEventSelectionProcess, 3);
};
#endif
