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

#ifndef RestCore_TRestDummyToDetectorHitsProcess
#define RestCore_TRestDummyToDetectorHitsProcess

#include "TRestDummyEvent.h"
#include "TRestEventProcess.h"
#include "TRestHitsEvent.h"

class TRestDummyToDetectorHitsProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDummyEvent* fDummyEvent;  //!
    TRestHitsEvent* fHitsEvent;    //!
#endif
    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        // print any metadata members of the class here. Using: metadata << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "dummyToDetectorHitsEvent"; }

    // Constructor
    TRestDummyToDetectorHitsProcess();
    TRestDummyToDetectorHitsProcess(char* cfgFileName);
    // Destructor
    ~TRestDummyToDetectorHitsProcess();

    ClassDef(TRestDummyToDetectorHitsProcess, 1);  // Transform a TRestDummyEvent event to a
                                                   // TRestHitsEvent (hits-collection event)
};
#endif
