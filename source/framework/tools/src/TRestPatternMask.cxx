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

/////////////////////////////////////////////////////////////////////////
/// Write the class description Here
///
/// ### Parameters
/// Describe any parameters this process receives:
/// * **parameter1**: This parameter ...
/// * **parameter2**: This parameter is ...
///
///
/// ### Examples
/// Give examples of usage and RML descriptions that can be tested.
/// \code
///     <WRITE A CODE EXAMPLE HERE>
/// \endcode
///
/// ### Running pipeline example
/// Add the examples to a pipeline to guarantee the code will be running
/// on future framework upgrades.
///
///
/// Please, add any figure that may help to illustrate the process
///
/// \htmlonly <style>div.image img[src="trigger.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the trigger definition](trigger.png)
///
/// The png image should be uploaded to the ./images/ directory
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// YEAR-Month: First implementation of TRestPatternMask
/// WRITE YOUR FULL NAME
///
/// \class TRestPatternMask
/// \author: TODO. Write full name and e-mail:        jgalan
///
/// <hr>
///

#include "TRestPatternMask.h"

ClassImp(TRestPatternMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestPatternMask::TRestPatternMask() : TRestMetadata() { Initialize(); }

/////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestAxionMagneticField section inside the RML.
///
TRestPatternMask::TRestPatternMask(const char* cfgFileName, std::string name) : TRestMetadata(cfgFileName) {
    // Initialize();

    if (GetVerboseLevel() >= REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestPatternMask::~TRestPatternMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
// void TRestPatternMask::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief It produces an effective mask rotation and translation for the
/// point x,y.
///
/// Remark: The window is centered, it is just the pattern that we want
/// to shift.
///
void TRestPatternMask::RotateAndTranslate(Double_t& x, Double_t& y) {
    TVector2 pos(x, y);

    pos = pos.Rotate(-fRotationAngle);
    pos -= fOffset;

    x = pos.X();
    y = pos.Y();
}

///////////////////////////////////////////////
/// \brief Returns true if the pattern was hitted.
///
bool TRestPatternMask::HitsPattern(Double_t x, Double_t y) {
    if (GetRegion(x, y) > 0) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method MUST be implemented at the
/// inherited class.
///
/// To include mask radius limits and rotation at the inherited class, just
/// call this method at the re-implementation.
///
/// ```
/// if( TRestPatternMask::GetRegion(x,y) == 0 ) return 0;
/// ```
///
/// Then the values of x,y will be compatible with the offset and rotation
/// defined inside this class
///
Int_t TRestPatternMask::GetRegion(Double_t& x, Double_t& y) {
    if (fMaskRadius > 0 && x * x + y * y > fMaskRadius * fMaskRadius) return 0;

    RotateAndTranslate(x, y);

    return 1;
}

/////////////////////////////////////////////
/// \brief Initialization of TRestPatternMask field members through a RML file
///
/*
void TRestPatternMask::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    // If we recover the metadata class from ROOT file we will need to call Initialize ourselves
    this->Initialize();
} */

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestPatternMask::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << " - Type : " << fPatternType << endl;
    metadata << " - Offset : (" << fOffset.X() << ", " << fOffset.Y() << ") mm" << endl;
    metadata << " - Rotation angle : " << fRotationAngle * 180. / TMath::Pi() << " degrees" << endl;
    metadata << "----" << endl;
}

