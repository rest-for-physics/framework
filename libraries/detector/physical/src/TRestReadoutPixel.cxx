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

//////////////////////////////////////////////////////////////////////////
///
/// This class stores the readout pixel geometrical description, origin
/// position, orientation, and size. A readout pixel is the most elementary
/// component of a readout used to construct a readout channel.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-aug:  First concept.
///            Javier Galan
///
/// \class      TRestReadoutPixel
/// \author     Javier Galan
///
/// <hr>
///

double delta = 1.e-6;

#include "TRestReadoutPixel.h"
using namespace std;

ClassImp(TRestReadoutPixel)
    ///////////////////////////////////////////////
    /// \brief TRestReadoutPixel default constructor
    ///
    TRestReadoutPixel::TRestReadoutPixel() {
    Initialize();
}

///////////////////////////////////////////////
/// \brief TRestReadoutPixel default destructor
///
TRestReadoutPixel::~TRestReadoutPixel() {}

///////////////////////////////////////////////
/// \brief Initializes the pixel members
///
void TRestReadoutPixel::Initialize() {}

///////////////////////////////////////////////
/// \brief Returns the center TVector2 position of the pixel
///
TVector2 TRestReadoutPixel::GetCenter() const {
    TVector2 center(0, 0);
    TVector2 origin(fPixelOriginX, fPixelOriginY);
    TVector2 opositeVertex = GetVertex(2);

    if (fTriangle)
        center = (opositeVertex - origin) / 4. + origin;
    else
        center = (origin + opositeVertex) / 2.;

    return center;
    //*/
}

///////////////////////////////////////////////
/// \brief Returns the specified pixel vertex position
///
/// \param n A value between 0-3 definning the vertex position to be returned.
///
TVector2 TRestReadoutPixel::GetVertex(int n) const {
    TVector2 vertex(0, 0);
    TVector2 origin(fPixelOriginX, fPixelOriginY);

    if (n % 4 == 0)
        return origin;
    else if (n % 4 == 1) {
        vertex.Set(fPixelSizeX, 0);
        vertex = vertex.Rotate(fRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    } else if (n % 4 == 2) {
        vertex.Set(fPixelSizeX, fPixelSizeY);
        vertex = vertex.Rotate(fRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    } else if (n % 4 == 3) {
        vertex.Set(0, fPixelSizeY);
        vertex = vertex.Rotate(fRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    }
    return vertex;
}

///////////////////////////////////////////////
/// \brief Determines if a given *x,y* coordinates are found inside the pixel.
/// The coordinates are referenced to the readout module system.
///
Bool_t TRestReadoutPixel::isInside(Double_t x, Double_t y) {
    TVector2 pos(x, y);
    return isInside(pos);
}

///////////////////////////////////////////////
/// \brief Determines if a given TVector2 *pos* coordinates are found inside
/// the pixel. The coordinates are referenced to the readout module system.
///
Bool_t TRestReadoutPixel::isInside(TVector2 pos) {
    pos = TransformToPixelCoordinates(pos);
    Double_t const x = pos.X();
    if (pos.X() >= -delta && pos.X() <= fPixelSizeX + delta)  // Condition on X untouched
    {
        if (fTriangle && pos.Y() >= -delta &&
            pos.Y() <= fPixelSizeY + delta -
                           x * (fPixelSizeY / fPixelSizeX))  // if triangle, third condition depends on x
            return true;
        if (!fTriangle && pos.Y() >= -delta &&
            pos.Y() <= fPixelSizeY + delta)  // for a normal rectangular pixel, same
                                             // simple conditions
            return true;
    }
    return false;
}

///////////////////////////////////////////////
/// \brief Transforms the coordinates given in a TVector2 to the internal
/// pixel coordinate system. The coordinates are referenced to the readout
/// module system.
///
TVector2 TRestReadoutPixel::TransformToPixelCoordinates(TVector2 p) {
    TVector2 pos(p.X() - fPixelOriginX, p.Y() - fPixelOriginY);
    pos = pos.Rotate(-fRotation * TMath::Pi() / 180.);
    return pos;
}

///////////////////////////////////////////////
/// \brief Prints on screen the pixel details, origin, size, rotation
///
void TRestReadoutPixel::Print(int index) {
    cout << "      ## Pixel : " << index << " position : (" << GetOriginX() << "," << GetOriginY()
         << ") mm size : (" << GetSizeX() << "," << GetSizeY() << ") mm rotation : " << fRotation
         << " degrees"
         << " type : ";
    if (fTriangle)
        cout << "triangle" << endl;
    else
        cout << "rectangle" << endl;
}
