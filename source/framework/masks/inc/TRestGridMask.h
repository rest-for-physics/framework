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

#ifndef REST_TRestGridMask
#define REST_TRestGridMask

#include <TRestPatternMask.h>

/// A class used to define a grid mask pattern
class TRestGridMask : public TRestPatternMask {
   private:
    void Initialize() override;

    /// The periodity of the grid structure in mm
    Double_t fGridGap = 1;  //<

    /// The width of the grid structure in mm
    Double_t fGridThickness = 0.5;  //<

    /// It defines the maximum number of cells/regions in each axis
    Int_t fModulus = 10;  //<

   public:
    virtual Int_t GetRegion(Double_t& x, Double_t& y) override;

    /// It returns the gap/periodicity of the grid in mm
    Double_t GetGridGap() { return fGridGap; }

    /// It returns the thickness of the grid in mm
    Double_t GetGridThickness() { return fGridThickness; }

    /// It returns the modulus used to define a finite set of ids
    Int_t GetModulus() { return fModulus; }

    void PrintMetadata() override;
    void PrintMaskMembers() override;
    void PrintMask() override;

    TRestGridMask();
    TRestGridMask(const char* cfgFileName, std::string name = "");
    ~TRestGridMask();

    ClassDefOverride(TRestGridMask, 1);
};
#endif
