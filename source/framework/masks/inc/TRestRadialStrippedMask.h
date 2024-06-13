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

#ifndef REST_TRestRadialStrippedMask
#define REST_TRestRadialStrippedMask

#include <TRestPatternMask.h>

/// A class used to define a stripped mask pattern
class TRestRadialStrippedMask : public TRestPatternMask {
   private:
    void Initialize() override;

    /// The periodity of the stripped structure in radians
    Double_t fStripsAngle = TMath::Pi()/3;  //<

    /// The width of the stripped structure in mm
    Double_t fStripsThickness = 0.5;  //<

    /// It defines the maximum number of cells/regions in each axis
    Int_t fModulus = 10;

   public:
    virtual Int_t GetRegion(Double_t& x, Double_t& y) override;

    /// It returns the gap/periodicity of the strips in degrees
    Double_t GetStripsAngle() { return fStripsAngle * units("degrees"); }

    /// It returns the thickness of the strips in mm
    Double_t GetStripsThickness() { return fStripsThickness; }

    /// It returns the modulus used to define a finite set of ids
    Int_t GetModulus() { return fModulus; }

    void PrintMetadata() override;
    void PrintMaskMembers() override;
    void PrintMask() override;

    TRestRadialStrippedMask();
    TRestRadialStrippedMask(const char* cfgFileName, std::string name = "");
    ~TRestRadialStrippedMask();

    ClassDefOverride(TRestRadialStrippedMask, 1);
};
#endif
