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

#ifndef RestCore_TRestGeant4AnalysisProcess
#define RestCore_TRestGeant4AnalysisProcess

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>
#include <TRestGas.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

//! A generic analysis REST process to extract valuable information from a
//! *Geant4* simulation produced by *restG4*.
class TRestGeant4AnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    /// A pointer to the specific TRestG4Event input
    TRestG4Event* fInputG4Event;  //!

    /// A pointer to the specific TRestG4Event output
    TRestG4Event* fOutputG4Event;  //!

    /// A pointer to the simulation metadata information stored under
    /// TRestG4Metadata
    TRestG4Metadata* fG4Metadata;  //!

    /// A vector storing the name of observables (xxxVolumeEDep) related to energy
    /// deposition in a particular active volume.
    std::vector<std::string> fEnergyInObservables;  //!

    /// A vector storing the active volume ids of observables (xxxVolumeEDep)
    /// related to energy deposition in a particular active volume.
    std::vector<Int_t> fVolumeID;  //!

    /// A vector storing the name of observables (xxxMeanPosX,Y,Z) related to mean
    /// hits position in a particular active volume.
    std::vector<std::string> fMeanPosObservables;  //!

    /// A vector storing the active volume ids corresponding mean position
    /// observable (xxxMeanPosX,Y,Z).
    std::vector<Int_t> fVolumeID2;  //!

    /// A vector storing the direction X,Y or Z from corresponding mean position
    /// observable (xxxMeanPosX,Y,Z).
    std::vector<std::string> fDirID;               //!
                                                   /// A vector storing the name of observables  related to
                                                   /// processes in a particular active volume.
    std::vector<std::string> fProcessObservables;  //!

    /// A vector storing the active volume ids corresponding process observable .
    std::vector<Int_t> fVolumeID3;  //!

    /// A vector storing the name of processes.
    std::vector<std::string> fProcessName;  //!
    /// A vector storing the name of observables (xxxTrackCounter) related to the
    /// number of tracks of the given particle name.
    std::vector<std::string> fTrackCounterObservables;  //!

    /// A vector storing the name of the particle related to the track counter
    /// observable (xxxTrackCounter).
    std::vector<std::string> fParticleTrackCounter;  //!

    /// A vector storing the name of observables (xxxTrackEdep) related to the
    /// energy deposited by the given particle name.
    std::vector<std::string> fTracksEDepObservables;  //!

    /// A vector storing the name of the particle related to the track energy
    /// observable (xxxTrackEDep).
    std::vector<std::string> fParticleTrackEdep;  //!

#endif
    /// A low energy cut applied to input geant4 events. Events below the
    /// threshold will be not further processed.
    Double_t fLowEnergyCut;

    /// A high energy cut applied to input geant4 events. Events above the
    /// threshold will be not further processed.
    Double_t fHighEnergyCut;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Low energy cut : " << fLowEnergyCut << " keV" << endl;
        metadata << "High energy cut : " << fHighEnergyCut << " keV" << endl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestGeant4AnalysisProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "geant4Analysis"; }

    TRestGeant4AnalysisProcess();
    TRestGeant4AnalysisProcess(char* cfgFileName);
    ~TRestGeant4AnalysisProcess();

    ClassDef(TRestGeant4AnalysisProcess, 1);
};
#endif
