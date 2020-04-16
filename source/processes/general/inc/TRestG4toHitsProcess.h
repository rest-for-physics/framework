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

#ifndef RestCore_TRestG4toHitsProcess
#define RestCore_TRestG4toHitsProcess

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>
#include <TRestGas.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

/// A process to transform a *TRestG4Event* into a *TRestHitsEvent*.
class TRestG4toHitsProcess : public TRestEventProcess {
   private:
    /// A pointer to the input TRestG4Event
    TRestG4Event* fG4Event;  //!

    /// A pointer to the Geant4 simulation conditions stored in TRestG4Metadata
    TRestG4Metadata* fG4Metadata;  //!

    /// A pointer to the output TRestHitsEvent
    TRestHitsEvent* fHitsEvent;  //!

    /// The volume ids from the volumes selected for transfer to TRestHitsEvent
    vector<Int_t> fVolumeId;  //!

    /// The geometry volume names to be transferred to TRestHitsEvent
    vector<TString> fVolumeSelection;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fG4Event; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "g4toHitsEvent"; }

    // Constructor
    TRestG4toHitsProcess();
    TRestG4toHitsProcess(char* cfgFileName);
    // Destructor
    ~TRestG4toHitsProcess();

    ClassDef(TRestG4toHitsProcess, 1);  // Transform a TRestG4Event event to a
                                        // TRestHitsEvent (hits-collection event)
};
#endif
