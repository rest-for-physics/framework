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

/// A class used to define a rings mask pattern
class TRestSpiderMask : public TRestPatternMask {
   private:
    void Initialize();

    /// The periodity of the rings structure in mm. Optionally used to initialize the rings.
    Double_t fSpiderGap = 0;  //!

    /// The width of the rings structure in mm. Optionally used to initialize the rings.
    Double_t fSpiderThickness = 0;  //!

    /// The number of rings inside the structure.
    Int_t fNSpider = 0;  //!

    /// The initial radius for the inner ring
    Double_t fInitialRadius = 0;  //!

    /// A pair containing inner/outter radius for each ring
    std::vector<std::pair<Double_t, Double_t>> fSpiderRadii;  //<

   public:
    void GenerateSpider();

    virtual Int_t GetRegion(Double_t x, Double_t y);

    /// It returns the gap/periodicity of the rings in mm
    Double_t GetSpiderGap() { return fSpiderGap; }

    /// It returns the thickness of the rings in mm
    Double_t GetSpiderThickness() { return fSpiderThickness; }

    /// It returns the number of rings to be generated
    Double_t GetNumberOfSpider() { return fNSpider; }

    /// It returns the most inner ring radius
    Double_t GetInitialRadius() { return fInitialRadius; }

    /// It allows to redefine the inner and outter rings radii directly
    void SetRadii(const std::vector<Double_t>& innerR, const std::vector<Double_t>& outterR) {
        if (innerR.size() != outterR.size()) {
            warning << "TRestSpiderMask::SetRadii. Vectors have not the same size!" << endl;
            return;
        }

        for (int n = 0; n < innerR.size(); n++) {
            std::pair<Double_t, Double_t> p(innerR[n], outterR[n]);
            fSpiderRadii.push_back(p);
        }
    }

    void PrintMetadata();

    TRestSpiderMask();
    TRestSpiderMask(const char* cfgFileName, std::string name = "");
    ~TRestSpiderMask();

    ClassDef(TRestSpiderMask, 1);
};
#endif
