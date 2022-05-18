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

/// UPDATE Write here a brief description. Just one line!
class TRestDataSet : public TRestMetadata {
   private:
    // REMOVE COMMENT. Add here the members/parameters for your metadata class.
    // You can set their default values here together.
    // Note: add "//!" mark at the end of the member definition
    // if you don't want to save them to disk. The following dummy member are given as examples.

    ROOT::RDataFrame fDataset = 0;  //!
    std::string fStartTime = "";    //<
    std::string fEndTime = "";      //<
    std::string fFilePattern = "";  //<
    TTimeStamp* fStartTimeStamp;    //!
    TTimeStamp* fEndTimeStamp;      //!

   public:
    void PrintMetadata();
    void Initialize();

    TRestDataSet();
    TRestDataSet(const char* cfgFileName, std::string name = "");
    ~TRestDataSet();

    // REMOVE COMMENT. ROOT class definition helper. Increase the number in it every time
    // you add/rename/remove the metadata members
    ClassDef(TRestDataSet, 1);
};
#endif
