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

#ifndef RestCore_TRestRawSignalRecoverChannelsProcess
#define RestCore_TRestRawSignalRecoverChannelsProcess

#include "TRestRawSignalEvent.h"
#include "TRestReadout.h"

#include "TRestEventProcess.h"

//! A process allowing to recover selected channels from a TRestRawSignalEvent
class TRestRawSignalRecoverChannelsProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    /// A pointer to the specific TRestSignalEvent input
    TRestRawSignalEvent* fInputSignalEvent;  //!

    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fOutputSignalEvent;  //!

    /// A pointer to the readout previously defined inside REST.
    TRestReadout* fReadout;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

    void GetAdjacentSignalIds(Int_t signalId, Int_t& idLeft, Int_t& idRight);

   protected:
    std::vector<Int_t> fChannelIds;

   public:
    void BeginOfEventProcess();
    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        for (unsigned int n = 0; n < fChannelIds.size(); n++)
            metadata << "Channel id to recover : " << fChannelIds[n] << endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestRawSignalRecoverChannelsProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "recoverChannels"; }

    // Constructor
    TRestRawSignalRecoverChannelsProcess();
    TRestRawSignalRecoverChannelsProcess(char* cfgFileName);

    // Destructor
    ~TRestRawSignalRecoverChannelsProcess();

    ClassDef(TRestRawSignalRecoverChannelsProcess, 1);
};
#endif
