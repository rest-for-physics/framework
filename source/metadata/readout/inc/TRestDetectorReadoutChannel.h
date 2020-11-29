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

#ifndef RestCore_TRestDetectorReadoutChannel
#define RestCore_TRestDetectorReadoutChannel

#include <iostream>

#include "TObject.h"
#include "TRestMetadata.h"

#include "TRestDetectorReadoutPixel.h"

enum TRestDetectorReadoutChannelType {
    Channel_NoType = 0,
    Channel_Pixel = 1,
    Channel_X = 2,
    Channel_Y = 3,
    Channel_U = 4,
    Channel_V = 5,
    Channel_W = 6,
};

/// A class to store the readout channel definition used in TRestDetectorReadoutModule.
/// It allows to integrate any number of independent readout pixels.
class TRestDetectorReadoutChannel : public TObject {
   private:
    Int_t fDaqID;  ///< Defines the corresponding daq channel id. See decoding
                   ///< details at TRestDetectorReadout.
    std::vector<TRestDetectorReadoutPixel> fReadoutPixel;  ///< A vector storing the different
                                                           ///< TRestDetectorReadoutPixel definitions.

    Short_t fChannelId = -1;  ///< It stores the corresponding physical readout channel

    void Initialize();

   public:
    /// Returns the corresponding daq channel id
    Int_t GetDaqID() { return fDaqID; }

    /// Returns the corresponding channel id
    Int_t GetChannelId() { return fChannelId; }

    /// Returns the total number of pixels inside the readout channel
    Int_t GetNumberOfPixels() { return fReadoutPixel.size(); }

    TRestDetectorReadoutPixel& operator[](int n) { return fReadoutPixel[n]; }

    /// Returns a pointer to the pixel *n* by index.
    TRestDetectorReadoutPixel* GetPixel(int n) {
        if (n >= GetNumberOfPixels()) return NULL;
        return &fReadoutPixel[n];
    }

    void SetType(TRestDetectorReadoutChannelType type) {
        // in future we may implement this
    }

    TRestDetectorReadoutChannelType GetType() {
        // in future we may implement this
        return Channel_NoType;
    }

    /// Sets the daq channel number id
    void SetDaqID(Int_t id) { fDaqID = id; }

    /// Sets the readout channel number id
    void SetChannelID(Int_t id) { fChannelId = id; }

    /// Adds a new pixel to the readout channel
    void AddPixel(TRestDetectorReadoutPixel pix) { fReadoutPixel.push_back(pix); }

    Int_t isInside(Double_t x, Double_t y);

    void Print(int DetailLevel = 0);

    // Construtor
    TRestDetectorReadoutChannel();
    // Destructor
    virtual ~TRestDetectorReadoutChannel();

    ClassDef(TRestDetectorReadoutChannel, 3);  // REST run class
};
#endif
