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

#ifndef REST_TRestCombinedMask
#define REST_TRestCombinedMask

#include <TRestPatternMask.h>

/// A class used to define and generate a combined structure mask
class TRestCombinedMask : public TRestPatternMask {
   private:
    std::vector<TRestPatternMask*> fMasks;

   public:
    void Initialize() override;

    void AddMask(TRestPatternMask* mask) { fMasks.push_back(mask); }

    Int_t GetRegion(Double_t x, Double_t y) override;

    void InitFromConfigFile() override;

    void PrintMetadata() override;
    void PrintMaskMembers() override;
    void PrintMask() override;

    TRestCombinedMask();
    TRestCombinedMask(const char* cfgFileName, std::string name = "");
    ~TRestCombinedMask();

    ClassDefOverride(TRestCombinedMask, 1);
};
#endif
