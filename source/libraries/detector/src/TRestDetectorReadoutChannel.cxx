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
/// \class      TRestDetectorReadoutChannel
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestDetectorReadoutChannel.h"
using namespace std;

ClassImp(TRestDetectorReadoutChannel);
///////////////////////////////////////////////
/// \brief TRestDetectorReadoutChannel default constructor
///
TRestDetectorReadoutChannel::TRestDetectorReadoutChannel() { Initialize(); }

///////////////////////////////////////////////
/// \brief TRestDetectorReadoutChannel default destructor
///
TRestDetectorReadoutChannel::~TRestDetectorReadoutChannel() {}

///////////////////////////////////////////////
/// \brief Initializes the channel members
///
void TRestDetectorReadoutChannel::Initialize() { fDaqID = -1; }

///////////////////////////////////////////////
/// \brief Determines if (x,y) referenced to module coordinates is inside the
/// channel
///
Int_t TRestDetectorReadoutChannel::isInside(Double_t x, Double_t y) {
    for (unsigned int i = 0; i < fReadoutPixel.size(); i++)
        if (fReadoutPixel[i].isInside(x, y)) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief Prints the details of the readout channel including pixel
/// coordinates.
///
void TRestDetectorReadoutChannel::Print(int DetailLevel) {
    if (DetailLevel >= 0) {
        metadata << "++++ Channel : " << GetChannelId() << " Daq channel : " << GetDaqID() << endl;

        string typestr;
        if (GetType() == Channel_NoType)
            typestr = "NoType";
        else if (GetType() == Channel_Pixel)
            typestr = "Pixel";
        else if (GetType() == Channel_X)
            typestr = "X";
        else if (GetType() == Channel_Y)
            typestr = "Y";
        else if (GetType() == Channel_U)
            typestr = "U";
        else if (GetType() == Channel_V)
            typestr = "V";
        else if (GetType() == Channel_W)
            typestr = "W";

        metadata << " Total pixels : " << GetNumberOfPixels() << " Channel type : " << typestr << endl;
        metadata << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        if (DetailLevel - 1 >= 0)
            for (int n = 0; n < GetNumberOfPixels(); n++) {
                fReadoutPixel[n].Print();
            }
    }
}
