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

#ifndef REST_TRestSensitivity
#define REST_TRestSensitivity

#include "TRestExperiment.h"

/// It combines a number of experimental conditions allowing to calculate a combined experimental sensitivity
class TRestSensitivity : public TRestMetadata {
   private:
    /// A list of experimental conditions included to get a final sensitivity plot
    std::vector<TRestExperiment*> fExperiments;  //!

    /// The fusioned list of parameterization nodes found at each experiment signal
    std::vector<Double_t> fParameterizationNodes;  //<

    /// A vector of calculated sensitivity curves defined as a funtion of the parametric node
    std::vector<std::vector<Double_t>> fCurves;  //<

    /// A flag that will frozen adding more experiments in the future.
    Bool_t fFrozen = false;  //<  Only needed if we add experiments by other means than RML

    /// It is used to generate a histogram with the signal distribution produced with different signal samples
    TH1D* fSignalTest = nullptr;

    /// A canvas to draw
    TCanvas* fCanvas = nullptr;  //!

   protected:
    void InitFromConfigFile() override;

    Double_t UnbinnedLogLikelihood(const TRestExperiment* experiment, Double_t node, Double_t g4 = 0);
    Double_t ApproachByFactor(Double_t node, Double_t g4, Double_t chi0, Double_t target, Double_t factor);

   public:
    void Initialize() override;

    void ExtractExperimentParameterizationNodes(Bool_t rescan = false);
    std::vector<Double_t> GetParameterizationNodes() { return fParameterizationNodes; }
    void PrintParameterizationNodes();

    Double_t GetCoupling(Double_t node, Double_t sigma = 2, Double_t precision = 0.01);
    void AddCurve(const std::vector<Double_t>& curve) { fCurves.push_back(curve); }
    void ImportCurve(const std::vector<Double_t>& curve) { AddCurve(curve); }
    void GenerateCurve();
    void GenerateCurves(Int_t N);

    std::vector<Double_t> GetCurve(size_t n = 0);
    std::vector<Double_t> GetAveragedCurve();
    std::vector<std::vector<Double_t>> GetLevelCurves(const std::vector<Double_t>& levels);

    void ExportCurve(std::string fname, int n);
    void ExportAveragedCurve(std::string fname);

    TH1D* SignalStatisticalTest(Double_t node, Int_t N);

    void Freeze() { fFrozen = true; }

    std::vector<TRestExperiment*> GetExperiments() { return fExperiments; }
    TRestExperiment* GetExperiment(const size_t& n) {
        if (n >= GetNumberOfExperiments())
            return nullptr;
        else
            return fExperiments[n];
    }

    size_t GetNumberOfExperiments() { return fExperiments.size(); }
    size_t GetNumberOfCurves() { return fCurves.size(); }
    size_t GetNumberOfNodes() { return fParameterizationNodes.size(); }

    void PrintMetadata() override;

    TCanvas* DrawCurves();
    TCanvas* DrawLevelCurves();

    TRestSensitivity(const char* cfgFileName, const std::string& name = "");
    TRestSensitivity();
    ~TRestSensitivity();

    ClassDefOverride(TRestSensitivity, 2);
};
#endif
