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

#ifndef REST_TRestDataSet
#define REST_TRestDataSet

#include "ROOT/RDataFrame.hxx"
#include "TRestMetadata.h"
#include "TTimeStamp.h"

/// It allows to group a number of runs that satisfy given metadata conditions
class TRestDataSet : public TRestMetadata {
   private:
    /// All the selected runs will have a starting date after fStartTime
    std::string fStartTime = "2000/01/01";  //<

    /// All the selected runs will have an ending date before fEndTime
    std::string fEndTime = "3000/12/31";  //<

    /// A glob file pattern that must be satisfied by all files
    std::string fFilePattern = "";  //<

    ROOT::RDataFrame fDataset = 0;   //!
    TTimeStamp fStartTimeStamp = 0;  //!
    TTimeStamp fEndTimeStamp = 0;    //!

    /// A list populated by the FileSelection method using the conditions of the dataset
    std::vector<std::string> fFileSelection;  //!

    /// A list of metadata members where filters will be applied
    std::vector<std::string> fFilterMetadata;  //!

    /// If not empty it will check if the metadata member contains the value
    std::vector<std::string> fFilterContains;  //!

    /// If the corresponding element is not empty it will check if the metadata member is greater
    std::vector<Double_t> fFilterGreaterThan;  //!

    /// If the corresponding element is not empty it will check if the metadata member is lower
    std::vector<Double_t> fFilterLowerThan;  //!

    void InitFromConfigFile() override;

   public:
    void PrintMetadata();
    void Initialize();
    std::vector<std::string> FileSelection();
    TRestDataSet();
    TRestDataSet(const char* cfgFileName, std::string name = "");
    ~TRestDataSet();

    ClassDef(TRestDataSet, 1);
};
#endif
