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

#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// It defines a background/signal distribution in a given parameter space (tipically x,y,en)
class TRestComponent : public TRestMetadata {
   private:
    //// This will not be necessary the day TRestComponent is a pure abstract class.
    //// We will create an instance of TRestDataSetComponent or TRestFormulaComponent
    /// It defines how the distribution is initialized (dataset/formula)
    std::string fType = "dataset";

    /// A list with the branches that will be used to create the distribution space
    std::vector<std::string> fVariables;  //<

    /// The range of each of the variables used to create the distribution
    std::vector<TVector2> fRanges;  //<

    /// The number of bins in which we should divide each variable
    std::vector<Int_t> fNbins;  //<

    /// A list with the branches that will be used to construct the distribution density
    std::vector<std::string> fWeights;  //<

    /// It does not contribute to define the density distribution but allows to parametrize a set of
    /// distribution densities
    std::string fParametricVariable = "";  //<

    /// It defines the nodes of the parametrization
    std::vector<Double_t> fParametrizationNodes;

    /// It defines the binning between the parametrization nodes
    Int_t fParametrizationBinning = 0;

    ////////// This should be implemented in TRestDataSetComponent
    //////////
    /// The dataset used to initialize the distribution
    TRestDataSet fDataSet;  //!
                            //////////
                            ////////// This should be implemented in TRestDataSetComponent

    ////////// This should be implemented in TRestFormulaComponent
    //////////
    /// The function used to initialize the distribution
    /// std::string fFunction = "";  //!
    ///
    /// The function used to initialize the distribution
    /// TFormula fFormula;  //!
    //////////
    ////////// This should be implemented in TRestFormulaComponent

    /// A pointer to the component distribution
    // THnD* fDistribution = nullptr;  //!

   protected:
    void InitFromConfigFile() override;

   public:
    Double_t GetRate(std::vector<Double_t> point);

    void PrintMetadata() override;

    void Initialize() override;
    TRestComponent(const char* cfgFileName, const std::string& name);
    TRestComponent();
    ~TRestComponent();

    ClassDefOverride(TRestComponent, 1);
};
#endif