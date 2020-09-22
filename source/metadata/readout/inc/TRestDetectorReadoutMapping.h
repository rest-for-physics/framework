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

#ifndef RestCore_TRestDetectorReadoutMapping
#define RestCore_TRestDetectorReadoutMapping

#include <iostream>

#include <TMatrixD.h>
#include <TObject.h>

/// This class defines a uniform 2-dimensional grid relating its nodes to the
/// pixels of a readout.
class TRestDetectorReadoutMapping : public TObject {
   private:
    Int_t fNodesX;  ///< The number of nodes in the x-axis.
    Int_t fNodesY;  ///< The number of nodes in the y-axis.

    Double_t fNetSizeX;  ///< The size of the net/grid in the x-axis.
    Double_t fNetSizeY;  ///< The size of the net/grid in the y-axis.

    TMatrixD fChannel;  ///< A matrix containning the channel id for the
                        ///< corresponding XY-node.
    TMatrixD fPixel;    ///< A matrix containning the pixel id of fChannel for the
                        ///< corresponding XY-node.

   public:
    // Getters
    /// Returns the number of nodes in X.
    Int_t GetNumberOfNodesX() { return fNodesX; }

    /// Returns the number of nodes in Y.
    Int_t GetNumberOfNodesY() { return fNodesY; }

    /// Gets the channel id correspoding to a given node (i,j)
    Int_t GetChannelByNode(Int_t i, Int_t j) { return fChannel[i][j]; }

    /// Gets the pixel id correspoding to a given node (i,j)
    Int_t GetPixelByNode(Int_t i, Int_t j) { return fPixel[i][j]; }

    Bool_t isNodeSet(Int_t i, Int_t j);

    Bool_t AllNodesSet();

    Int_t GetNumberOfNodesNotSet();

    Int_t GetNodeX_ForChannelAndPixel(Int_t ch, Int_t px);

    Int_t GetNodeY_ForChannelAndPixel(Int_t ch, Int_t px);

    Int_t GetNodeX(Double_t x);

    Int_t GetNodeY(Double_t y);

    Double_t GetX(Int_t nodeX);

    Double_t GetY(Int_t nodeY);

    Int_t GetChannel(Double_t x, Double_t y);

    Int_t GetPixel(Double_t x, Double_t y);

    void SetNode(Int_t i, Int_t j, Int_t ch, Int_t pix);

    void Initialize(Int_t nX, Int_t nY, Double_t sX, Double_t sY);

    // Construtor
    TRestDetectorReadoutMapping();
    // Destructor
    ~TRestDetectorReadoutMapping();

    ClassDef(TRestDetectorReadoutMapping, 1);
};
#endif
