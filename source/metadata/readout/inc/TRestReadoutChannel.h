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

#ifndef RestCore_TRestReadoutChannel
#define RestCore_TRestReadoutChannel

#include <iostream>

#include "TObject.h"

#include "TRestReadoutPixel.h"

enum TRestReadoutChannelType {
	Channel_NoType = 0,
	Channel_Pixel = 1,
	Channel_X = 2,
	Channel_Y = 3,
	Channel_U = 4,
	Channel_V = 5,
	Channel_W = 6,
};

/// A class to store the readout channel definition used in TRestReadoutModule. It allows to integrate any number of independent readout pixels.
class TRestReadoutChannel : public TObject {
private:
	//channelID: 
	//-1: initial state
	//0~9999: undefined type
	//10000~19999: pixel channel(one pixel)
	//20000~29999: x channel(pixels have fixed y coordinate)
	//30000~39999: y channel(pixels have fixed x coordinate)
	Int_t fChannelID; ///< Defines the physical readout channel id
	Int_t fDaqID;     ///< Defines the corresponding daq channel id. See decoding details at TRestReadout.
	std::vector <TRestReadoutPixel> fReadoutPixel;  ///< A vector storing the different TRestReadoutPixel definitions.

	void Initialize();

protected:

public:

	/// Returns the physical readout channel id
	Int_t GetID() { return fChannelID % 10000; }

	/// Returns the corresponding daq channel id
	Int_t GetDaqID() { return fDaqID; }

	/// Returns the total number of pixels inside the readout channel
	Int_t GetNumberOfPixels() { return fReadoutPixel.size(); }

	TRestReadoutPixel& operator[] (int n) { return fReadoutPixel[n]; }

	/// Returns a pointer to the pixel *n* by index.
	TRestReadoutPixel *GetPixel( int n ) { return &fReadoutPixel[n]; }

	/// Sets the physical channel id
	void SetID(Int_t id) {
		int type = fChannelID / 10000;
		fChannelID = id + type * 10000;
	}

	void SetType(TRestReadoutChannelType type) {
		if (fChannelID >= 0) {
			fChannelID = type * 10000 + fChannelID % 10000;
		}
		else
		{
			std::cout << "REST WARNING: cannot set channel type before channel id!" << std::endl;
		}
	}

	TRestReadoutChannelType GetType() {
		return TRestReadoutChannelType(fChannelID / 10000);
	}

	/// Sets the daq channel number id
	void SetDaqID(Int_t id) { fDaqID = id; }

	/// Adds a new pixel to the readout channel
	void AddPixel(TRestReadoutPixel pix) { fReadoutPixel.push_back(pix); }

	TRestReadoutPixel *GetPixelByID(int id);

	Int_t isInside(Double_t x, Double_t y);

	void Print(int DetailLevel = 0);

	//Construtor
	TRestReadoutChannel();
	//Destructor
	virtual ~TRestReadoutChannel();


	ClassDef(TRestReadoutChannel, 1);     // REST run class
};
#endif
