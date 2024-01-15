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

#ifndef REST_TRestComponentDataSet
#define REST_TRestComponentDataSet

#include <THn.h>

#include "TRestComponent.h"
#include "TRestDataSet.h"

/// It defines a background/signal model distribution in a given parameter space (tipically x,y,en)
class TRestComponentDataSet : public TRestComponent {
   private:
    /// A list with the dataset columns used to weight the distribution density and define rate
    std::vector<std::string> fWeights;  //<

    /// It defines the number of entries for each parameterization node (Initialized by the dataset)
    std::vector<Int_t> fNSimPerNode;  //<

    /// The filename of the dataset used
    std::vector<std::string> fDataSetFileNames;  //<

    /// TODO we need to define multiple datasets and weigth. The weight will be used
    /// to create a model, such as weighting different background contaminations or
    /// different signal coupling contributions.

    /// TODO Then we probably need here a `std::vector <TRestDataSet>` and another vector
    /// with the weights (isotope activity/flux/etc).

    /// The dataset used to initialize the distribution
    TRestDataSet fDataSet;  //!

    /// It is true of the dataset was loaded without issues
    Bool_t fDataSetLoaded = false;  //!

    Bool_t ValidDataSet();

   protected:
    std::vector<Double_t> ExtractParameterizationNodes();
    std::vector<Int_t> ExtractNodeStatistics(Double_t precision = 0.01);
    void FillHistograms(Double_t precision = 0.01) override;

    Bool_t VariablesOk();
    Bool_t WeightsOk();

    Bool_t LoadDataSets();

   public:
    Bool_t IsDataSetLoaded() { return fDataSetLoaded; }

    void PrintStatistics();

    void PrintMetadata() override;
    void Initialize() override;
    void InitFromConfigFile() override;

    TRestComponentDataSet();
    TRestComponentDataSet(const char* cfgFileName, const std::string& name);
    ~TRestComponentDataSet();

    ClassDefOverride(TRestComponentDataSet, 3);
};
#endif
