/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
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
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef REST_TRestExperimentList
#define REST_TRestExperimentList

#include "TRestExperiment.h"
#include "TRestMetadata.h"

/// A helper metadata class to create a list of TRestExperiment instances
class TRestExperimentList : public TRestMetadata {
   private:
    /// A fullpath filename pattern helping to initialize the component files vector
    std::string fComponentPattern = "";  //<

    /// A vector with filenames containing the components
    std::vector<std::string> fComponentFiles;  //<

    /// A fullpath filename pattern helping to initialize the dataset files vector
    std::string fDataSetPattern = "";  //<

    /// A vector with filenames containing the datasets with experimental data
    std::vector<std::string> fDataSetFilenames;  //<

    /// A file where we define experiment components, exposureTime, and tracking data of each experiment
    std::string fExperimentsFile = "";  //< Exposure/TrackingData - SignalComponent - BackgroundComponent

    /// A table with the experiment file information
    std::vector<std::vector<std::string> > fExperimentsTable;  //<

    /// A vector with a list of experiments includes the background components in this model
    std::vector<TRestExperiment*> fExperiments;  //<

    /// The fusioned list of parameterization nodes found at each experiment signal
    std::vector<Double_t> fParameterizationNodes;  //<

    /// The mock dataset will be generated using the mean counts instead of a real MonteCarlo
    Bool_t fUseAverage = false;  //<

    /// If not zero this will be the common exposure time in micro-seconds (standard REST units)
    Double_t fExposureTime = 0;

    /// In case an exposure time is given it defines how to assign time to each experiment (equal/ksvz).
    std::string fExposureStrategy = "equal";

    /// The factor used on the exponential exposure time as a function of the experiment number
    Double_t fExposureFactor = 0;

    /// If not null this will be the common signal used in each experiment
    TRestComponent* fSignal = nullptr;  //<

    /// If not null this will be the common signal used in each experiment
    TRestComponent* fBackground = nullptr;  //<

   protected:
    TRestComponent* GetComponent(std::string compName);

    void InitFromConfigFile() override;

   public:
    void Initialize() override;

    void SetExposure(const Double_t& exposure) { fExposureTime = exposure; }
    void SetSignal(TRestComponent* comp) { fSignal = comp; }
    void SetBackground(TRestComponent* comp) { fBackground = comp; }

    void ExtractExperimentParameterizationNodes();
    std::vector<Double_t> GetParameterizationNodes() { return fParameterizationNodes; }
    void PrintParameterizationNodes();

    std::vector<TRestExperiment*> GetExperiments() { return fExperiments; }
    TRestExperiment* GetExperiment(const size_t& n) {
        if (n >= GetNumberOfExperiments())
            return nullptr;
        else
            return fExperiments[n];
    }

    size_t GetNumberOfExperiments() { return fExperiments.size(); }

    void PrintMetadata() override;

    TRestExperimentList(const char* cfgFileName, const std::string& name);

    TRestExperimentList();
    ~TRestExperimentList();

    ClassDefOverride(TRestExperimentList, 2);
};
#endif
