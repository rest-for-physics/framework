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

#ifndef REST_TRestSpiderMask
#define REST_TRestSpiderMask

#include <TRestPatternMask.h>

/// A class used to define and generate a spider structure mask
class TRestSpiderMask : public TRestPatternMask {
   private:
    /// The angle between two consecutive spider arms measured in radians.
    Double_t fArmsSeparationAngle = 0;  //<

    /// The width of each specific spider arm. Measured in radians. Default is 2.5 degrees.
    Double_t fArmsWidth = TMath::Pi() / 18. / 4.;  //<

    /// The spider structure will be effective from this radius, in mm. Default is from 20 mm.
    Double_t fInitialRadius = 20.;  //<

    /// Radius of an internal circular region defined inside the fInitialRadius. If 0, there will be no region
    Double_t fInternalRegionRadius = 0.;  //<

    /// Used internally to define the forbidden (cosine) angular ranges imposed by the spider structure (0,Pi)
    std::vector<std::pair<Double_t, Double_t>> fPositiveRanges;  //!

    /// Used internally to define the forbidden (cosine) ang. ranges imposed by the spider structure (Pi,2Pi)
    std::vector<std::pair<Double_t, Double_t>> fNegativeRanges;  //!

   public:
    void Initialize() override;

    void GenerateSpider();

    virtual Int_t GetRegion(Double_t& x, Double_t& y) override;

    /// It returns the gap/periodicity of the spider structure arms in radians
    Double_t GetArmsSeparationAngle() { return fArmsSeparationAngle; }

    /// It returns the angular width of each spider arm in radians
    Double_t GetArmsWidth() { return fArmsWidth; }

    /// It returns the number of arms in the spider structure
    size_t GetNumberOfArms() {
        if (fArmsSeparationAngle == 0) return 0;
        return (size_t)(2 * TMath::Pi() / fArmsSeparationAngle);
    }

    /// It returns the inner ring radius that defines the inner start of the spider structure
    Double_t GetInitialRadius() { return fInitialRadius; }

    void PrintMetadata() override;
    void PrintMaskMembers() override;
    void PrintMask() override;

    TRestSpiderMask();
    TRestSpiderMask(const char* cfgFileName, std::string name = "");
    ~TRestSpiderMask();

    ClassDefOverride(TRestSpiderMask, 2);
};
#endif
