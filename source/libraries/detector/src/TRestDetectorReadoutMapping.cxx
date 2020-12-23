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
/// This class defines a uniform coordinate grid with XY-nodes. Each node
/// is associated to a channel and pixel from the readout module definition.
/// It is TRestDetectorReadoutModule::DoReadoutMapping the responsible to establish
/// the correspondence between nodes and the set (channel,pixel).
///
/// In complex readouts the pixel and channel can be hard to identify.
/// This class will be used by TRestDetectorReadoutModule::FindChannel in order to
/// try to guess the pixel and channel where a given coordinate is found.
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
/// \class      TRestDetectorReadoutMapping
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestDetectorReadoutMapping.h"
using namespace std;

ClassImp(TRestDetectorReadoutMapping)
    ///////////////////////////////////////////////
    /// \brief TRestDetectorReadoutMapping constructor
    ///
    TRestDetectorReadoutMapping::TRestDetectorReadoutMapping() {}

///////////////////////////////////////////////
/// \brief TRestDetectorReadoutMapping destructor
///
TRestDetectorReadoutMapping::~TRestDetectorReadoutMapping() {}

///////////////////////////////////////////////
/// \brief Gets the X position of node (i,j)
///
Double_t TRestDetectorReadoutMapping::GetX(Int_t nodeX) { return (fNetSizeX / fNodesX) * nodeX; }

///////////////////////////////////////////////
/// \brief Gets the Y position of node (i,j)
///
Double_t TRestDetectorReadoutMapping::GetY(Int_t nodeY) { return (fNetSizeY / fNodesY) * nodeY; }

///////////////////////////////////////////////
/// \brief Gets the nodeX index corresponding to the x coordinate
///
Int_t TRestDetectorReadoutMapping::GetNodeX(Double_t x) {
    Int_t nX = (Int_t)((x / fNetSizeX) * fNodesX);
    if (nX >= fNodesX) return fNodesX - 1;
    return nX;
}

///////////////////////////////////////////////
/// \brief Gets the nodeY index corresponding to the y coordinate
///
Int_t TRestDetectorReadoutMapping::GetNodeY(Double_t y) {
    Int_t nY = (Int_t)((y / fNetSizeY) * fNodesY);
    if (nY >= fNodesY) return fNodesY - 1;
    return nY;
}

///////////////////////////////////////////////
/// \brief Gets the channel number corresponding to coordinates (x,y)
///
Int_t TRestDetectorReadoutMapping::GetChannel(Double_t x, Double_t y) {
    return fChannel[GetNodeX(x)][GetNodeY(y)];
}

///////////////////////////////////////////////
/// \brief Gets the pixel number corresponding to coordinates (x,y)
///
Int_t TRestDetectorReadoutMapping::GetPixel(Double_t x, Double_t y) {
    return fPixel[GetNodeX(x)][GetNodeY(y)];
}

///////////////////////////////////////////////
/// \brief Returns the number of nodes that have not been initialized.
///
Int_t TRestDetectorReadoutMapping::GetNumberOfNodesNotSet() {
    Int_t counter = 0;
    for (int i = 0; i < fNodesX; i++)
        for (int j = 0; j < fNodesY; j++) {
            if (!isNodeSet(i, j)) counter++;
        }
    return counter;
}

///////////////////////////////////////////////
/// \brief Finds the node index in X for a given channel and pixel ids.
///
Int_t TRestDetectorReadoutMapping::GetNodeX_ForChannelAndPixel(Int_t ch, Int_t px) {
    for (int i = 0; i < fNodesX; i++)
        for (int j = 0; j < fNodesY; j++) {
            if (fChannel[i][j] == ch && fPixel[i][j] == px) return i;
        }
    return -1;
}

///////////////////////////////////////////////
/// \brief Finds the node index in Y for a given channel and pixel ids.
///
Int_t TRestDetectorReadoutMapping::GetNodeY_ForChannelAndPixel(Int_t ch, Int_t px) {
    for (int i = 0; i < fNodesX; i++)
        for (int j = 0; j < fNodesY; j++) {
            if (fChannel[i][j] == ch && fPixel[i][j] == px) return j;
        }
    return -1;
}

///////////////////////////////////////////////
/// \brief Resets the matrix values and allocates memory for the given net size.
///
void TRestDetectorReadoutMapping::Initialize(Int_t nX, Int_t nY, Double_t sX, Double_t sY) {
    fNodesX = nX;
    fNodesY = nY;
    fNetSizeX = sX;
    fNetSizeY = sY;
    fChannel.ResizeTo(fNodesX, fNodesY);
    fPixel.ResizeTo(fNodesX, fNodesY);

    for (int i = 0; i < fNodesX; i++)
        for (int j = 0; j < fNodesY; j++) {
            fChannel[i][j] = -1;
            fPixel[i][j] = -1;
        }
}

///////////////////////////////////////////////
/// \brief Sets the readout channel and pixel corresponding to a mapping node
///
void TRestDetectorReadoutMapping::SetNode(Int_t i, Int_t j, Int_t ch, Int_t pix) {
    fChannel[i][j] = ch;
    fPixel[i][j] = pix;
}

///////////////////////////////////////////////
/// \brief Checks if the node (i,j) is defined.
///
Bool_t TRestDetectorReadoutMapping::isNodeSet(Int_t i, Int_t j) {
    if (fChannel[i][j] == -1 || fPixel[i][j] == -1) return false;
    return true;
}

///////////////////////////////////////////////
/// \brief Checks if all the nodes in the net have been defined.
///
Bool_t TRestDetectorReadoutMapping::AllNodesSet() {
    for (int i = 0; i < fNodesX; i++)
        for (int j = 0; j < fNodesY; j++) {
            if (!isNodeSet(i, j)) {
                cout << "Node : " << i << " , " << j << " is NOT set. Ch : " << fChannel[i][j]
                     << " Pix : " << fPixel[i][j] << endl;
                return false;
            }
        }
    return true;
}
