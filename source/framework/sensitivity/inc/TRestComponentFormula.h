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

#ifndef REST_TRestComponentFormula
#define REST_TRestComponentFormula

#include <THn.h>

#include "TRestDataSet.h"
#include "TRestComponent.h"

/// It defines a background/signal model distribution in a given parameter space (tipically x,y,en)
class TRestComponentFormula : public TRestComponent {
   private:
    /// A list with the branches that will be used to create the distribution space
    std::vector<std::string> fVariables;  //<

    /// The range of each of the variables used to create the PDF distribution
    std::vector<TVector2> fRanges;  //<

    /// The number of bins in which we should divide each variable
    std::vector<Int_t> fNbins;  //<

    /// A list with the branches that will be used to weight the distribution density
    std::vector<std::string> fWeights;  //<

    /// It is used to parameterize a set of distribution densities (e.g. WIMP or axion mass)
    std::string fParameter = "";  //<

    /// It defines the nodes of the parameterization (Initialized by the dataset)
    std::vector<Double_t> fParameterizationNodes;  //<

    /// It defines the statistics of each parameterization node (Initialized by the dataset)
    std::vector<Int_t> fNodeStatistics;  //<

    /// The function used to initialize the distribution
    /// std::string fFunction = "";  //!
    ///
    /// The function used to initialize the distribution
    /// TFormula fFormula;  //!

    /// A pointer to the component distribution
    // THnD* fDistribution = nullptr;  //!

   protected:
    std::vector<Double_t> ExtractParameterizationNodes();
    std::vector<Int_t> ExtractNodeStatistics();
    void FillHistograms();

    Bool_t VariablesOk();
    Bool_t WeightsOk();

    Int_t GetVariableIndex(std::string varName);

    void InitFromConfigFile() override;

   public:
    Bool_t LoadDataSets();

    /// This method should go to TRestDataSetComponent
    Bool_t IsDataSetLoaded() { return fDataSetLoaded; }

    Double_t GetRate(std::vector<Double_t> point) override;
    Double_t GetTotalRate() override;

    Int_t SetActiveNode(Double_t node);

    THnD* GetDensityForNode(Double_t value);
    THnD* GetDensityForActiveNode();

    void PrintMetadata() override;

    void PrintStatistics();
    void PrintNodes();

    void Initialize() override;
    TRestComponentFormula(const char* configFilename);
    TRestComponentFormula(const char* cfgFileName, const std::string& name);
    TRestComponentFormula();
    ~TRestComponentFormula();

    ClassDefOverride(TRestComponentFormula, 1);
};
#endif
