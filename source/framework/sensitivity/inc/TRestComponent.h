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

#ifndef REST_TRestComponent
#define REST_TRestComponent

//#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// It defines a background/signal distribution in a given parameter space (tipically x,y,en)
class TRestComponent : public TRestMetadata {
   private:
    /// It defines how the distribution is initialized (dataset/formula)
    std::string fType = "dataset";

    /// A list (comma separated) with the branches that will be used to create the distribution
    std::string fVariables;  //<

    /// A list (comma separated) with the branches that will be used to weight the binning
    std::string fWeights;  //<

    /// The range of each of the variables used to create the distribution
    std::vector<TVector2> fRange;  //<

    /// The number of bins in which we should divide each variable
    std::vector<Int_t> fNbins;  //<

    /// The dataset used to initialize the distribution
    // TRestDataSet fDataSet;  //!

    /// The function used to initialize the distribution
    std::string fFunction = "";  //!

    /// The function used to initialize the distribution
    TFormula fFormula;  //!

    /// A pointer to the component distribution
    // THnD* fDistribution = nullptr;  //!

   public:
    Double_t GetRate(std::vector<Double_t> point);

    void PrintMetadata() override;

    void Initialize() override;
    TRestComponent();
    ~TRestComponent();

    ClassDefOverride(TRestComponent, 1);
};
#endif
