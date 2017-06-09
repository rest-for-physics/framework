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
/// \class      TRestReadoutChannel
/// \author     Javier Galan
///
/// <hr>
///


#include "TRestReadoutChannel.h"
using namespace std;

ClassImp(TRestReadoutChannel)
///////////////////////////////////////////////
/// \brief TRestReadoutChannel default constructor
///
    TRestReadoutChannel::TRestReadoutChannel()
{
    Initialize();

}

///////////////////////////////////////////////
/// \brief TRestReadoutChannel default destructor
///
TRestReadoutChannel::~TRestReadoutChannel()
{
}

///////////////////////////////////////////////
/// \brief Initializes the channel members
/// 
void TRestReadoutChannel::Initialize()
{
    fChannelID = -1;
}

///////////////////////////////////////////////
/// \brief Returns a TRestReadoutPixel pointer using the internal pixel id.
/// 
TRestReadoutPixel *TRestReadoutChannel::GetPixelByID( int id )
{
    Int_t pxNumber = -1;
    for( unsigned int i = 0; i < fReadoutPixel.size(); i++ )
        if( fReadoutPixel[i].GetID() == id )
        {
            if( pxNumber != -1 ) cout << "REST Warning : Found several pixels with the same ID" << endl;
            pxNumber = i;
        }
    if( pxNumber != -1 )
        return &fReadoutPixel[pxNumber];

    cout << "REST Warning : Readout pixel with ID : " << id << " not found in channel : " << fChannelID << endl;

    return NULL; 
}

///////////////////////////////////////////////
/// \brief Prints the details of the readout channel including pixel coordinates.
/// 
void TRestReadoutChannel::Print( )
{
        cout << "++++ Channel ID : " << GetID( ) << " Daq channel : " << GetDaqID() << " Total pixels : " << GetNumberOfPixels() << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

        for( int n = 0; n < GetNumberOfPixels(); n++ )
        {
            GetPixel(n)->Print();
        }
}
