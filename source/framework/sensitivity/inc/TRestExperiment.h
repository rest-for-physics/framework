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

#include "TRestComponent.h"
#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// It includes a model definition and experimental data used to obtain a final experimental sensitivity
class TRestExperiment : public TRestMetadata {
   private:
    /// The exposure time. If 0 it will be extracted from the tracking dataset
    Double_t fExposureTime = 0;  //<

    /// A pointer to the background component
    TRestComponent* fBackground = nullptr;  //<

    /// A pointer to the signal component
    TRestComponent* fSignal = nullptr;  //<

    /// It contains the experimental data (should contain same columns as the components)
    TRestDataSet fTrackingData;  //<

    /// If enabled the tracking data will be MC-generated following background compatibility
    Bool_t fMockTracking = false;  //<

   protected:
    void InitFromConfigFile() override;

   public:
    void GenerateMockDataSet();

    void Initialize() override;

    void PrintMetadata() override;

    TRestExperiment(const char* cfgFileName, const std::string& name = "");
    TRestExperiment();
    ~TRestExperiment();

    ClassDefOverride(TRestExperiment, 1);
};
#endif
