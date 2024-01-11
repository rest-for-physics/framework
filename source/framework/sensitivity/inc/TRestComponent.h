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

#include <TCanvas.h>
#include <THn.h>

#include "TRestDataSet.h"
#include "TRestMetadata.h"
#include "TRestResponse.h"

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

    /// A pointer to the detector response
    TRestResponse* fResponse = nullptr;  //<

    /// A canvas for drawing the active node component
    TCanvas* fCanvas = nullptr;  //!

    /// It returns true if any nodes have been defined.
    Bool_t HasNodes() { return !fParameterizationNodes.empty(); }

    /// It returns true if the node has been properly identified
    Bool_t ValidNode(Double_t node) {
        SetActiveNode(node);
        if (GetActiveNode() >= 0) return true;
        return false;
    }

    Int_t GetVariableIndex(std::string varName);

    void InitFromConfigFile() override;

   public:
    TRestResponse* GetResponse() const { return fResponse; }

    virtual Double_t GetRate(std::vector<Double_t> point) = 0;
    virtual Double_t GetTotalRate() = 0;

    Double_t GetNormalizedRate(std::vector<Double_t> point);
    Double_t GetRateWithResponse(std::vector<Double_t> point);
    Double_t GetNormalizedRateWithResponse(std::vector<Double_t> point);

    size_t GetDimensions() { return fVariables.size(); }

    Int_t GetActiveNode() { return fActiveNode; }
    Int_t SetActiveNode(Double_t node);
    Double_t GetActiveNodeValue() { return fParameterizationNodes[fActiveNode]; }

    void LoadResponse(const TRestResponse& resp);

    void PrintMetadata() override;

    void PrintStatistics();
    void PrintNodes();

    void Initialize() override;
    TRestComponent(const char* cfgFileName, const std::string& name = "");
    TRestComponent();
    ~TRestComponent();

    ClassDefOverride(TRestComponent, 2);
};
#endif
