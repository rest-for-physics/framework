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

#ifndef REST_TRestRingsMask
#define REST_TRestRingsMask

#include <TRestPatternMask.h>

/// A class used to define a rings mask pattern
class TRestRingsMask : public TRestPatternMask {
   private:
    void Initialize();

    /// The periodity of the rings structure in mm. Optionally used to initialize the rings.
    Double_t fRingsGap = 0;  //<

    /// The width of the rings structure in mm. Optionally used to initialize the rings.
    Double_t fRingsThickness = 0;  //<

    /// The number of rings inside the structure.
    Int_t fNRings = 0;  //<

    /// The initial radius for the inner ring
    Double_t fInitialRadius = 0;  //<

    /// A pair containing inner/outter radius for each ring
    std::vector<std::pair<Double_t, Double_t>> fRingsRadii;  //<

   public:
    void GenerateRings();

    virtual Int_t GetRegion(Double_t x, Double_t y);

    /// It returns the gap/periodicity of the rings in mm
    Double_t GetRingsGap() { return fRingsGap; }

    /// It returns the thickness of the rings in mm
    Double_t GetRingsThickness() { return fRingsThickness; }

    /// It returns the number of rings to be generated
    Double_t GetNumberOfRings() { return fNRings; }

    /// It returns the most inner ring radius
    Double_t GetInitialRadius() { return fInitialRadius; }

    /// It allows to redefine the inner and outter rings radii directly
    void SetRadii(const std::vector<Double_t>& innerR, const std::vector<Double_t>& outterR) {
        if (innerR.size() != outterR.size()) {
            RESTWarning << "TRestRingsMask::SetRadii. Vectors have not the same size!" << RESTendl;
            return;
        }

        for (int n = 0; n < innerR.size(); n++) {
            std::pair<Double_t, Double_t> p(innerR[n], outterR[n]);
            fRingsRadii.push_back(p);
        }
    }

    void PrintMetadata();

    TRestRingsMask();
    TRestRingsMask(const char* cfgFileName, std::string name = "");
    ~TRestRingsMask();

    ClassDef(TRestRingsMask, 1);
};
#endif
