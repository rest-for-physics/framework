/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
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
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestMapGroup
#define RestCore_TRestMapGroup

#include "TRestMetadata.h"

/// The most simple metadata class to be used as a template
class TRestMapGroup : public TRestMetadata {
   protected:
    /// Documentation of my dummy member
    std::vector<string> fGroupKeys;
    std::vector<string> fGroupValues;
    std::map<string, string> fMapGroup;

   public:
    void Initialize();

    // Implement only if necessary
    void InitFromConfigFile();

    // Setters & Getters
    // Should use "inline" when defining functions in the header
    inline std::map<string, string> GetMapGroup() const { return fMapGroup; }
    inline void SetMapGroup(std::map<string, string> mapGroup) { fMapGroup = mapGroup; }

    void ConstructMap();

    void PrintMetadata();

    // Constructor & Destructor
    TRestMapGroup();
    ~TRestMapGroup();

    ClassDef(TRestMapGroup, 1);
};

#endif
