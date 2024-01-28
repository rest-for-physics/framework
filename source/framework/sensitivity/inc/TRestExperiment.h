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

#ifndef REST_TRestExperiment
#define REST_TRestExperiment

#include <TRandom3.h>

#include "TRestComponent.h"
#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// It includes a model definition and experimental data used to obtain a final experimental sensitivity
class TRestExperiment : public TRestMetadata {
   private:
    /// The exposure time. If 0 it will be extracted from the tracking dataset (In us, standard REST unit)
    Double_t fExposureTime = 0;  //<

    /// A pointer to the background component
    TRestComponent* fBackground = nullptr;  //<

    /// A pointer to the signal component
    TRestComponent* fSignal = nullptr;  //<

    /// It defines the filename used to load the dataset
    std::string fExperimentalDataSet = "";

    /// It contains the experimental data (should contain same columns as the components)
    TRestDataSet fExperimentalData;  //<

    /// If enabled it means that the experimental data was MC-generated
    Bool_t fMockData = false;  //<

    /// Internal process random generator
    TRandom3* fRandom = nullptr;  //!

    /// Seed used in random generator
    UInt_t fSeed = 0;  //<

   protected:
    void InitFromConfigFile() override;

   public:
    void GenerateMockDataSet();

    Bool_t IsMockData() { return fMockData; }

    void SetExposureInSeconds(const Double_t exposure) { fExposureTime = exposure / units("s"); }
    void SetSignal(TRestComponent* comp) { fSignal = comp; }
    void SetBackground(TRestComponent* comp) { fBackground = comp; }

    void SetExperimentalDataSetFile(const std::string& filename) {
        fExperimentalDataSet = SearchFile(filename);
        fExperimentalData.Import(fExperimentalDataSet);
        fExposureTime = fExperimentalData.GetTotalTimeInSeconds() * units("s");

        fMockData = false;

        /// TODO : We need to check here that the experimental data got the same variables as the components.
        /// Or we need to create a way to connect the column names to be used in the dataset with the
        /// variables
    }

    Double_t GetExposureInSeconds() { return fExposureTime * units("s"); }
    TRestComponent* GetBackground() const { return fBackground; }
    TRestComponent* GetSignal() const { return fSignal; }
    ROOT::RDF::RNode GetExperimentalData() { return fExperimentalData.GetDataFrame(); }

    void Initialize() override;

    void PrintMetadata() override;

    TRestExperiment(const char* cfgFileName, const std::string& name = "");
    TRestExperiment();
    ~TRestExperiment();

    ClassDefOverride(TRestExperiment, 1);
};
#endif
