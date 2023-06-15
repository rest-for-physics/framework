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

#ifndef REST_TRestDataSetTMVAClassification
#define REST_TRestDataSetTMVAClassification

#include "TH1F.h"
#include "TRestCut.h"
#include "TRestMetadata.h"

/// This class is meant to classify a given dataset with a particular TMVA method
class TRestDataSetTMVAClassification : public TRestMetadata {
   private:
    /// Name of the output file
    std::string fOutputFileName = ""; //<

    /// Name of the dataSet to classify
    std::string fDataSetName = ""; //<

    /// Name of the TMVA method
    std::string fTmvaMethod = ""; //<

    /// Name of the TMVA weights file
    std::string fTmvaFile = ""; //<

    /// Vector containing different obserbable names
    std::vector<std::string> fObsName; //<

    /// Cuts over the dataset for PDF selection
    TRestCut* fCut = nullptr; //<

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    void PrintMetadata() override;

    void ClassifyTMVA();

    inline void SetDataSet(const std::string& dSName) { fDataSetName = dSName; }
    inline void SetTMVAMethod(const std::string& method) { fTmvaMethod = method; }
    inline void SetTMVAFile(const std::string& file) { fTmvaFile = file; }
    inline void SetOutputFileName(const std::string& outName) { fOutputFileName = outName; }

    TRestDataSetTMVAClassification();
    TRestDataSetTMVAClassification(const char* configFilename, std::string name = "");
    ~TRestDataSetTMVAClassification();

    ClassDefOverride(TRestDataSetTMVAClassification, 1);
};
#endif
