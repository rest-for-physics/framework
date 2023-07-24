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

#include <THnSparse.h>

#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// It defines a background/signal model distribution in a given parameter space (tipically x,y,en)
class TRestComponent : public TRestMetadata {
   private:
    //// This will not be necessary the day TRestComponent is a pure abstract class.
    //// We will create an instance of TRestDataSetComponent or TRestFormulaComponent
    /// It defines how the distribution is initialized (dataset/formula)
    std::string fType = "dataset";

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

    ////////// This should be implemented in TRestDataSetComponent
    //////////

    /// The filename of the dataset used
    std::vector<std::string> fDataSetFileNames;  //<

    /// The generated N-dimensional variable space density for a given node
    std::vector<THnSparse*> fNodeDensity;  //<

    /// The generated N-dimensional variable space density for the complete dataset
    THnSparse* fTotalDensity = nullptr;  //<

    /// The dataset used to initialize the distribution
    TRestDataSet fDataSet;  //!

    /// It is true of the dataset was loaded without issues
    Bool_t fDataSetLoaded = false;  //!

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
    std::vector<Double_t> ExtractParameterizationNodes();
    std::vector<Int_t> ExtractNodeStatistics();
    void GenerateSparseHistograms();

    Bool_t VariablesOk();
    Bool_t WeightsOk();

    Int_t GetVariableIndex(std::string varName);

    void InitFromConfigFile() override;

   public:
    Bool_t LoadDataSets();

    /// This method should go to TRestDataSetComponent
    Bool_t IsDataSetLoaded() { return fDataSetLoaded; }

    Double_t GetRate(std::vector<Double_t> point);

    THnSparse* GetDensityForNode(Double_t value);

    TH1D* GetHistogram(Double_t node, std::string varName);
    TH2D* GetHistogram(Double_t node, std::string varName1, std::string varName2);
    TH3D* GetHistogram(Double_t node, std::string varName1, std::string varName2, std::string varName3);

    void PrintMetadata() override;

    void PrintStatistics();
    void PrintNodes();

    void Initialize() override;
    TRestComponent(const char* configFilename);
    TRestComponent(const char* cfgFileName, const std::string& name);
    TRestComponent();
    ~TRestComponent();

    ClassDefOverride(TRestComponent, 1);
};
#endif
