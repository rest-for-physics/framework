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

#ifndef RestCore_TRestSharedMemoryBufferToRawSignalProcess
#define RestCore_TRestSharedMemoryBufferToRawSignalProcess

#include "TRestEventProcess.h"
#include "TRestRawSignalEvent.h"

typedef struct {
    /// It allows interaction with the buffer generating process
    unsigned int dataReady;

    /// It stores the number of signals that have been registered in the buffer
    unsigned int nSignals;

    /// It stores the event id for the corresponding buffer
    unsigned int eventId;

    /// It stores the unix timestamp for the corresponding buffer
    double timeStamp;

    /// It defines the maximum number of signals used to determine the maximum
    /// size of the buffer
    unsigned int maxSignals;

    /// It defines the maximum number of samples per signal used to determine the
    /// maximum size of the buffer
    unsigned int maxSamples;

    /// It stores the size of the buffer, which should be maxSignals * (
    /// maxSamples + 1 )
    unsigned int bufferSize;

} daqInfo;

//! A process to read a shared buffer created by another external process and
//! create a TRestRawSignalEvent
class TRestSharedMemoryBufferToRawSignalProcess : public TRestEventProcess {
   private:
#ifndef __CINT__

    /// A pointer to the specific TRestRawSignalEvent input
    TRestRawSignalEvent* fOutputRawSignalEvent;  //!

    /// A pointer to the daqInfo data structure containning relevant information
    /// shared by the daq
    daqInfo* fShMem_daqInfo;  //!

    /// It is used internally to control the semaphore
    int fSemaphoreId;  //!

    /// A pointer to a shared buffer previously created by an external process
    /// (i.e. the daq).
    unsigned short int* fShMem_Buffer;  //!

    /// A value used to generate a unique key to access the shared daqInfo
    /// structure, created by a external process (i.e. the daq).
    Int_t fKeyDaqInfo;  //!

    /// A value used to generate a unique key to control the semaphore and manage
    /// access to shared resources.
    Int_t fKeySemaphore;  //!

    /// A value used to generate a unique key to access the shared buffer, created
    /// by a external process (i.e. the daq).
    Int_t fKeyBuffer;  //!

    /// The value in microseconds used in the main event process loop to allow the
    /// daq access the shared resources.
    Int_t fTimeDelay;  //!

    /// If true the shared buffer will be re-set to zero once TRestRawSignal has
    /// been loaded.
    Bool_t fReset;  //!
#endif

    void SemaphoreGreen(int id);
    void SemaphoreRed(int id);

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
   public:
    void InitProcess();

    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestSharedMemoryBufferToRawSignalProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "sharedMemoryBufferToRawSignalEvent"; }

    // Constructor
    TRestSharedMemoryBufferToRawSignalProcess();
    TRestSharedMemoryBufferToRawSignalProcess(char* cfgFileName);

    // Destructor
    ~TRestSharedMemoryBufferToRawSignalProcess();

    ClassDef(TRestSharedMemoryBufferToRawSignalProcess, 1);
};
#endif
