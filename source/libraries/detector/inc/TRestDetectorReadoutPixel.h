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

#ifndef RestCore_TRestDetectorReadoutPixel
#define RestCore_TRestDetectorReadoutPixel

#include <iostream>

#include <TMath.h>
#include <TVector2.h>
#include "TObject.h"
#include "TRestMetadata.h"

/// A class to store the readout pixel definition used in TRestDetectorReadoutChannel.
class TRestDetectorReadoutPixel : public TObject {
   private:
    Double_t fPixelOriginX;  ///< The pixel x-origin position, left-bottom corner.
    Double_t fPixelOriginY;  ///< The pixel y-origin position, left-bottom corner.

    Double_t fPixelSizeX;  ///< The pixel x size.
    Double_t fPixelSizeY;  ///< The pixel y size.

    Double_t fRotation;  ///< The pixel rotation angle in degrees, rotation with
                         ///< axis at the origin position.

    Bool_t fTriangle;  ///< The type of the pixel : false is rectangular, true is
                       ///< triangle
    void Initialize();

   protected:
   public:
    /// Returns the x-coordinate pixel origin.
    Double_t GetOriginX() const { return fPixelOriginX; }

    /// Returns the y-coordinate pixel origin.
    Double_t GetOriginY() const { return fPixelOriginY; }

    /// Returns a TVector2 with the pixel origin.
    TVector2 GetOrigin() const { return TVector2(fPixelOriginX, fPixelOriginY); }

    /// Returns the rotation angle in degrees
    Double_t GetRotation() const { return fRotation; }

    /// Returns the pixel size in X
    Double_t GetSizeX() { return fPixelSizeX; }

    /// Returns the pixel size in Y
    Double_t GetSizeY() { return fPixelSizeY; }

    /// Returns a TVector2 with the pixel size.
    TVector2 GetSize() { return TVector2(fPixelSizeX, fPixelSizeY); }

    /// Returns true if the pixel is a triangle.
    Bool_t GetTriangle() const { return fTriangle; }

    TVector2 GetCenter() const;

    TVector2 GetVertex(int n) const;

    /// Sets the origin of the pixel using the coordinate values *x*,*y*.
    void SetOrigin(Double_t x, Double_t y) {
        fPixelOriginX = x;
        fPixelOriginY = y;
    }

    /// Sets the origin of the pixel using a TVector2.
    void SetOrigin(TVector2 origin) {
        fPixelOriginX = origin.X();
        fPixelOriginY = origin.Y();
    }

    /// Sets the size of the pixel using the coordinate values *sx*,*sy*.
    void SetSize(Double_t sx, Double_t sy) {
        fPixelSizeX = sx;
        fPixelSizeY = sy;
    }

    /// Sets the size of the pixel using a TVector2.
    void SetSize(TVector2 size) {
        fPixelSizeX = size.X();
        fPixelSizeY = size.Y();
    }

    /// Sets the rotation angle of the pixel in degrees
    void SetRotation(Double_t rot) { fRotation = rot; }

    /// Sets the type of the pixel
    void SetTriangle(Bool_t type) { fTriangle = type; }

    Bool_t isInside(TVector2 pos);
    Bool_t isInside(Double_t x, Double_t y);

    TVector2 TransformToPixelCoordinates(TVector2 p);

    void Print();

    // Construtor
    TRestDetectorReadoutPixel();
    // Destructor
    virtual ~TRestDetectorReadoutPixel();

    ClassDef(TRestDetectorReadoutPixel, 4);
};
#endif
