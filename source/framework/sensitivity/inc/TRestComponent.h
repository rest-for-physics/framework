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

#include <THn.h>

#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// It defines a background/signal model distribution in a given parameter space (tipically x,y,en)
class TRestComponent : public TRestMetadata {

   protected:
    /// A list with the branches that will be used to create the distribution space
    std::vector<std::string> fVariables;  //<

    /// The range of each of the variables used to create the PDF distribution
    std::vector<TVector2> fRanges;  //<

    /// The number of bins in which we should divide each variable
    std::vector<Int_t> fNbins;  //<

    /// It is used to parameterize a set of distribution densities (e.g. WIMP or axion mass)
    std::string fParameter = "";  //<

    /// It defines the nodes of the parameterization (Initialized by the dataset)
    std::vector<Double_t> fParameterizationNodes;  //<

    /// It is used to define the node that will be accessed for rate retrieval
    Int_t fActiveNode = -1;  //<

    /// It returns true if any nodes have been defined.
    Bool_t HasNodes() { return !fParameterizationNodes.empty(); }

    Int_t GetVariableIndex(std::string varName);

    void InitFromConfigFile() override;

   public:
    virtual Double_t GetRate(std::vector<Double_t> point) = 0;
    virtual Double_t GetTotalRate() = 0;

    Int_t SetActiveNode(Double_t node);

    /*
THnD* GetDensityForNode(Double_t value);
THnD* GetDensityForActiveNode();

TH1D* GetHistogram(Double_t node, std::string varName);
TH2D* GetHistogram(Double_t node, std::string varName1, std::string varName2);
TH3D* GetHistogram(Double_t node, std::string varName1, std::string varName2, std::string varName3);

TH1D* GetHistogram(std::string varName);
TH2D* GetHistogram(std::string varName1, std::string varName2);
TH3D* GetHistogram(std::string varName1, std::string varName2, std::string varName3);
    */

    void PrintMetadata() override;

    void PrintStatistics();
    void PrintNodes();

    void Initialize() override;
    TRestComponent(const char* cfgFileName, const std::string& name = "");
    TRestComponent();
    ~TRestComponent();

    ClassDefOverride(TRestComponent, 1);
};
#endif
