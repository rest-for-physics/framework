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

#ifndef REST_TRestRunMerger
#define REST_TRestRunMerger

#include "TRestMetadata.h"

/// This class is meant to merge different TRestRun files
class TRestRunMerger : public TRestMetadata {
   private:
    // Input file patternt to match
    TString fInputFilePattern = "";
    // Name of the output file
    TString fOutputFileName = "";
    // Remove input files that are merged
    Bool_t fRemoveInputFiles = false;

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    void PrintMetadata() override;

    void MergeFiles();
    Int_t MergeFiles(const std::vector<std::string>& files, std::string outputFileName,
                     bool removeInputFiles = false);

    TRestRunMerger();
    TRestRunMerger(const char* configFilename, std::string name = "");
    ~TRestRunMerger();

    ClassDefOverride(TRestRunMerger, 1);
};
#endif
