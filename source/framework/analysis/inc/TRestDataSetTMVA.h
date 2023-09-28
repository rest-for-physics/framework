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

#ifndef REST_TRestDataSetTMVA
#define REST_TRestDataSetTMVA

#include "TH1F.h"
#include "TMVA/Types.h"
#include "TRestCut.h"
#include "TRestMetadata.h"

/// This class is meant to evaluate several TMVA methods in datasets
class TRestDataSetTMVA : public TRestMetadata {
   private:
    /// Name of the output file
    std::string fOutputFileName = "";  //<

    /// Name of the signal dataSet
    std::string fDataSetSignal = "";  //<

    /// Name of the background dataset
    std::string fDataSetBackground = "";  //<

    /// Name of the output path for the xml files
    std::string fOutputPath = "";  //<

    /// Vector containing different obserbable names
    std::vector<std::string> fObsName;  //<

    /// Add method to compute TMVA, https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf for more details
    std::vector<std::pair<std::string, std::string> > fMethod;  //<

    /// Cuts over background dataset for PDF selection
    TRestCut* fBackgroundCut = nullptr;  //<

    /// Cuts over signal dataset for PDF selection
    TRestCut* fSignalCut = nullptr;  //<

    /// If true display ROC curve after evaluating all methods
    bool fDrawROCCurve = true;  //<

    /// Map with supported TMVA methods, please add more if something is missing
    const std::map<std::string, TMVA::Types::EMVA> fMethodMap = {
        //<
        {"Likelihood", TMVA::Types::kLikelihood},  // Likelihood ("naive Bayes estimator")
        {"LikelihoodKDE",
         TMVA::Types::kLikelihood},        // Use a kernel density estimator to approximate the PDFs
        {"Fisher", TMVA::Types::kFisher},  // Fisher discriminant (same as LD)
        {"BDT", TMVA::Types::kBDT},        // Boosted Decision Trees
        {"MLP", TMVA::Types::kMLP}         // Multi-Layer Perceptron (Neural Network)
    };

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    void PrintMetadata() override;

    void ComputeTMVA();

    inline void SetDataSetSignal(const std::string& dSName) { fDataSetSignal = dSName; }
    inline void SetDataSetBackground(const std::string& dSName) { fDataSetBackground = dSName; }
    inline void SetOutputFileName(const std::string& outName) { fOutputFileName = outName; }
    inline void SetOutputPath(const std::string& outPath) { fOutputPath = outPath; }

    TRestDataSetTMVA();
    TRestDataSetTMVA(const char* configFilename, std::string name = "");
    ~TRestDataSetTMVA();

    ClassDefOverride(TRestDataSetTMVA, 1);
};
#endif
