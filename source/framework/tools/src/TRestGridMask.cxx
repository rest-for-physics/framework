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
/// YEAR-Month: First implementation of TRestGridMask
/// WRITE YOUR FULL NAME
///
/// \class TRestGridMask
/// \author: TODO. Write full name and e-mail:        jgalan
///
/// <hr>
///

#include "TRestGridMask.h"

#include "TRandom3.h"

ClassImp(TRestGridMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestGridMask::TRestGridMask() : TRestPatternMask() { Initialize(); }

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
TRestGridMask::TRestGridMask(const char* cfgFileName, std::string name) : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGridMask::~TRestGridMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestGridMask::Initialize() {
    SetSectionName(this->ClassName());
    SetType("Grid");
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::RotateAndTranslate.
///
Int_t TRestGridMask::GetRegion(Double_t x, Double_t y) {
    RotateAndTranslate(x, y);

    if (fMaskRadius > 0 && x * x + y * y > fMaskRadius * fMaskRadius) return 0;

    Double_t xEval = fGridThickness / 2. + x;

    Int_t xcont = 0;
    if (xEval > 0) {
        while (xEval > fGridGap) {
            xEval -= fGridGap;
            xcont++;
        }
    } else {
        while (xEval < 0) {
            xEval += fGridGap;
            xcont--;
        }
    }

    if (xEval < fGridThickness) return 0;

    Double_t yEval = fGridThickness / 2. + y;

    Int_t ycont = 0;
    if (yEval > 0) {
        while (yEval > fGridGap) {
            yEval -= fGridGap;
            ycont++;
        }
    } else {
        while (yEval < 0) {
            yEval += fGridGap;
            ycont--;
        }
    }

    if (yEval < fGridThickness) return 0;

    xcont = xcont % fModulus;
    if (xcont < 0) xcont += fModulus;

    ycont = ycont % fModulus;
    if (ycont < 0) ycont += fModulus;

    return 1 + fModulus * ycont + xcont;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestGridMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    metadata << "-----" << endl;
    metadata << " - Grid gap : " << fGridGap << " mm" << endl;
    metadata << " - Grid thickness : " << fGridThickness << " mm" << endl;
    metadata << "+++++" << endl;
}
