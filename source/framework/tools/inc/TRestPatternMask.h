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

#ifndef REST_TRestPatternMask
#define REST_TRestPatternMask

#include <TCanvas.h>

#include <TRestMetadata.h>

/// An abstract class used to encapsulate different mask pattern class definitions.
class TRestPatternMask : public TRestMetadata {
   private:
    /// It is used to introduce an offset on the pattern
    TVector2 fOffset = TVector2(0, 0);  //<

    /// An angle (in radians) used to introduce a rotation to the pattern
    Double_t fRotationAngle = 0;  //<

    /// The pattern type (Stripped/Grid/Spider/Rings)
    std::string fPatternType = "";  //<

    /// A canvas for drawing
    TCanvas* fCanvas = nullptr;  //!

   protected:
    /// The maximum mask radius in mm (if 0 it will be infinite)
    Double_t fMaskRadius = 0;

    /// It defines the mask type. To be called by the inherited class constructor.
    void SetType(const std::string& type) { fPatternType = type; }

    Int_t ApplyCommonMaskTransformation(Double_t& x, Double_t& y);

    TRestPatternMask();
    TRestPatternMask(const char* cfgFileName, std::string name = "");

   public:
    Bool_t HitsPattern(Double_t x, Double_t y);

    virtual Int_t GetRegion(Double_t x, Double_t y) = 0;

    /// It returns the mask pattern type
    std::string GetType() { return fPatternType; }

    /// It returns the rotation angle
    Double_t GetRotationAngle() { return fRotationAngle; }

    /// It returns the rotation angle
    TVector2 GetOffset() { return fOffset; }

    /// It returns the mask radius
    Double_t GetMaskRadius() { return fMaskRadius; }

    /// It defines the rotation angle
    void SetRotationAngle(const Double_t& angle) { fRotationAngle = angle; }

    /// It defines the pattern offset
    void SetOffset(const TVector2& offset) { fOffset = offset; }

    /// It defines the mask radius
    void SetMaskRadius(const Double_t& radius) { fMaskRadius = radius; }

    void PrintMetadata();

    TCanvas* DrawMonteCarlo(Int_t nSamples = 10000);

    ~TRestPatternMask();

    ClassDef(TRestPatternMask, 1);
};
#endif
