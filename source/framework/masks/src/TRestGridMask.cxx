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
/// This class defines a squared patterned grid. It defines a periodicity
/// and a thickness for the grid pattern. The method TRestGridMask::GetRegion
/// will return a unique id for each square delimited by the grid pattern.
///
/// The grid structure is centered in (0,0) and it can be shifted using
/// the offset defined inside TRestPatternMask. The pattern will be only
/// delimited by the limits imposed inside TRestPatternMask.
///
/// ### Specific grid metadata parameters
///
/// * **gridGap**: This parameter defines the grid periodicity.
/// * **gridThickness**: The thickness of the grid.
/// * **modulus**: A number that defines the range of ids used to identify
/// the different regions inside the grid. If modulus is 10, then we will
/// only be able to identify up to 100 unique regions. If a larger amount
/// of regions is found, it will happen that two regions will be assigned
/// the same id.
///
/// ### Common pattern metadata parameters
///
/// On top of the metadata class parameters, we may define common pattern
/// parameters to induce an offset and rotation to the pattern.
///
/// * **offset**: A parameter to shift the pattern window mask.
/// * **rotationAngle**: An angle given in radians to rotate the pattern.
/// * **maskRadius**: A radius defining the limits of the circular mask.
///
/// ### Examples
///
/// Mask pattern RML definitions can be found inside the file
/// `REST_PATH/examples/masks.rml`.
///
/// The following definition ilustrates a complete RML implementation of a
/// TRestGridMask.
///
/// \code
///	<TRestGridMask name="strongback" verboseLevel="warning">
///		<parameter name="maskRadius" value="20" />
///		<parameter name="offset" value="(1,2)cm" />
///		<parameter name="rotationAngle" value="0.5" />
///
///		<parameter name="gridGap" value="1cm" />
///		<parameter name="gridThickness" value="2mm" />
///	</TRestGridMask>
/// \endcode
///
/// The basic use of this class is provided by the TRestGridMask::GetRegion
/// method. For example:
///
/// \code
///     TRestGridMask mask("masks.rml", "grid");
///     Int_t id = mask.GetRegion( 12.5, 4.3 );
/// 	std::cout << "Region id is : " << id << RESTendl;
/// \endcode
///
/// The following figure may be generated using the TRestPatternMask::DrawMonteCarlo
/// method.
///
/// \code
///     TRestGridMask mask("masks.rml", "grid");
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///     c->Draw();
/// \endcode
///
/// \htmlonly <style>div.image img[src="gridmask.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the montecarlo mask test using DrawMonteCarlo](gridmask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-05: First implementation of TRestGridMask
/// Javier Galan
///
/// \class TRestGridMask
/// \author: Javier Galan - javier.galan@unizar.es
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
/// corresponding TRestGridMask section inside the RML.
///
TRestGridMask::TRestGridMask(const char* cfgFileName, std::string name) : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
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
/// The particle will be counter-rotated to emulate the mask offset and
/// rotation using the method TRestPatternMask::ApplyCommonMaskTransformation
///
Int_t TRestGridMask::GetRegion(Double_t x, Double_t y) {
    if (ApplyCommonMaskTransformation(x, y) == 0) return 0;

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

    return 1 + (fModulus * ycont + xcont) % fMaxRegions;
}

/////////////////////////////////////////////
/// \brief Prints on screen the complete information about the metadata members from this class
///
void TRestGridMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    PrintMaskMembers();
    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// including common pattern headers, but without common metadata headers.
///
void TRestGridMask::PrintMask() {
    PrintCommonPatternMembers();
    RESTMetadata << "----" << RESTendl;
    PrintMaskMembers();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// excluding common metadata headers.
///
void TRestGridMask::PrintMaskMembers() {
    RESTMetadata << " - Grid gap : " << fGridGap << " mm" << RESTendl;
    RESTMetadata << " - Grid thickness : " << fGridThickness << " mm" << RESTendl;
}
