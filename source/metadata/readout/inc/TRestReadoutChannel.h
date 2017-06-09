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

//! A class to store the readout channel definition used in TRestReadoutModule. It allows to integrate any number of independent readout pixels.
class TRestReadoutChannel : public TObject {
    private:
        Int_t fChannelID; //!< Defines the physical readout channel id
        Int_t fDaqID;     //!< Defines the corresponding daq channel id. See decoding details at TRestReadout.
        std::vector <TRestReadoutPixel> fReadoutPixel;  //!< A vector storing the different TRestReadoutPixel definitions.

        void Initialize();

    protected:

    public:

        /// Returns the physical readout channel id
        Int_t GetID() { return fChannelID; }

        /// Returns the corresponding daq channel id
        Int_t GetDaqID() { return fDaqID; }

        /// Returns the total number of pixels inside the readout channel
        Int_t GetNumberOfPixels( ) { return fReadoutPixel.size(); }

        /// Returns a pointer to the pixel *n* by index.
        TRestReadoutPixel *GetPixel( int n ) { return &fReadoutPixel[n]; }

        /// Sets the physical channel id
        void SetID( Int_t id ) { fChannelID = id; }

        /// Sets the daq channel number id
        void SetDaqID( Int_t id ) { fDaqID = id; }

        /// Adds a new pixel to the readout channel
        void AddPixel( TRestReadoutPixel pix ) { fReadoutPixel.push_back( pix ); }

        TRestReadoutPixel *GetPixelByID( int id );
        
        void Print( );

        //Construtor
        TRestReadoutChannel();
        //Destructor
        virtual ~ TRestReadoutChannel();


        ClassDef(TRestReadoutChannel, 1);     // REST run class
};
#endif
