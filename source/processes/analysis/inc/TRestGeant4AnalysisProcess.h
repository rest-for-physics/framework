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

#include <TRestGeant4Event.h>
#include <TRestGeant4Metadata.h>

#include "TRestEventProcess.h"

//! A pure analysis process to extract information from a TRestGeant4Event
class TRestGeant4AnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestGeant4Event input
    TRestGeant4Event* fInputG4Event;  //!

    /// A pointer to the specific TRestGeant4Event output
    TRestGeant4Event* fOutputG4Event;  //!

    /// A pointer to the simulation metadata information accessible to TRestRun
    TRestGeant4Metadata* fG4Metadata;  //!

    /// It stores the name of observables `xxxVolumeEDep` related to the energy deposition in volume `xxx`.
    std::vector<std::string> fEnergyInObservables;  //!

    /// A vector storing the active volume ids of observables `xxxVolumeEDep`.
    std::vector<Int_t> fVolumeID;  //!

    /// It stores the name of observables (xxxMeanPosX,Y,Z) related to mean hits position in volume `xxx`.
    std::vector<std::string> fMeanPosObservables;  //!

    /// A vector storing the active volume ids corresponding mean position observable `xxxMeanPosX,Y,Z`.
    std::vector<Int_t> fVolumeID2;  //!

    /// A vector storing the direction X,Y or Z from corresponding mean position observable `xxxMeanPosX,Y,Z`.
    std::vector<std::string> fDirID;  //!

    /// A vector storing the name of observables  related to processes in a particular active volume.
    std::vector<std::string> fProcessObservables;  //!

    /// A vector storing the active volume ids corresponding process observable .
    std::vector<Int_t> fVolumeID3;  //!

    /// A vector storing the name of processes.
    std::vector<std::string> fProcessName;  //!

    /// A vector storing the observable name `xxxTracksCounter` for a given `xxx` particle.
    std::vector<std::string> fTrackCounterObservables;  //!

    /// A vector storing the `xxx` particle name extracted from `xxxTracksCounter`.
    std::vector<std::string> fParticleTrackCounter;  //!

    /// A vector storing the observable name `xxxTracksEDep` for a given `xxx` particle.
    std::vector<std::string> fTracksEDepObservables;  //!

    /// A vector storing the `xxx` particle name extracted from `xxxTracksEDep`.
    std::vector<std::string> fParticleTrackEdep;  //!

    // TODO these two thresholds should become OBSOLETE. We must use now the more generic
    // way ApplyCut() with the <cut ...> definition at the RML process definition.

    /// A low energy cut. Events below the threshold will be not further processed.
    Double_t fLowEnergyCut;

    /// A high energy cut. Events above the threshold will be not further processed.
    Double_t fHighEnergyCut;

    Bool_t fPerProcessSensitiveEnergy = false;
    Bool_t fPerProcessSensitiveEnergyNorm = false;

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fInputG4Event; }
    any GetOutputEvent() { return fOutputG4Event; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
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

    ClassDef(TRestGeant4AnalysisProcess, 2);
};
#endif
