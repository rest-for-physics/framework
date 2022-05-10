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

#ifndef REST_TRestStrippedMask
#define REST_TRestStrippedMask

#include <TRestPatternMask.h>

/// A class used to define a stripped mask pattern
class TRestStrippedMask : public TRestPatternMask {
   private:
    void Initialize();

    /// The periodity of the stripped structure in mm
    Double_t fStripsGap = 1;  //<

    /// The width of the stripped structure in mm
    Double_t fStripsThickness = 0.5;  //<

    /// It defines the maximum number of cells/regions in each axis
    Int_t fModulus = 10;

   public:
    virtual Int_t GetRegion(Double_t x, Double_t y);

    /// It returns the gap/periodicity of the strips in mm
    Double_t GetStripsGap() { return fStripsGap; }

    /// It returns the thickness of the strips in mm
    Double_t GetStripsThickness() { return fStripsThickness; }

    /// It returns the modulus used to define a finite set of ids
    Int_t GetModulus() { return fModulus; }

    void PrintMetadata();

    TRestStrippedMask();
    TRestStrippedMask(const char* cfgFileName, std::string name = "");
    ~TRestStrippedMask();

    ClassDef(TRestStrippedMask, 1);
};
#endif
