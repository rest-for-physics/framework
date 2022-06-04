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
/// This class defines a stripped pattern. It defines a periodicity
/// and a thickness for the strips. The method TRestStrippedMask::GetRegion
/// will return a unique id for each region in between strips.
///
/// The stripped structure is centered in (0,0) and it can be shifted using
/// the offset defined inside TRestPatternMask. The pattern will be only
/// delimited by the limits imposed inside TRestPatternMask.
///
/// ### Specific stripped metadata parameters
///
/// * **stripsGap**: This parameter defines the strips periodicity.
/// * **stripsThickness**: The thickness of the strips.
/// * **modulus**: A number that defines the range of ids used to identify
/// the different regions inside the stripped pattern. If modulus is 10,
/// then we will only be able to identify up to 100 unique regions. If a
/// larger amount of regions is found, it will happen that two regions will
/// be assigned the same id.
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
/// TRestStrippedMask.
///
/// \code
///	<TRestStrippedMask name="strongback" verboseLevel="warning">
///		<parameter name="maskRadius" value="10" />
///		<parameter name="offset" value="(1,2)cm" />
///		<parameter name="rotationAngle" value="0.2" />
///
///		<parameter name="stripsGap" value="4mm" />
///		<parameter name="stripsThickness" value="0.5mm" />
///	</TRestStrippedMask>
/// \endcode
///
/// The basic use of this class is provided by the TRestStrippedMask::GetRegion
/// method. For example:
///
/// \code
///     TRestStrippedMask mask("masks.rml", "stripped");
///     Int_t id = mask.GetRegion( 12.5, 4.3 );
/// 	std::cout << "Region id is : " << id << endl;
/// \endcode
///
/// The following figure may be generated using the TRestPatternMask::DrawMonteCarlo
/// method.
///
/// \code
///     TRestStrippedMask mask("masks.rml", "stripped");
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///		c->Draw();
///     c->Print("strippedmask.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="strippedmask.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the montecarlo mask test using DrawMonteCarlo](strippedmask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-05: First implementation of TRestStrippedMask
/// Javier Galan
///
/// \class TRestStrippedMask
/// \author: Javier Galan - javier.galan@unizar.es
///
/// <hr>
///

#include "TRestStrippedMask.h"

#include "TRandom3.h"

ClassImp(TRestStrippedMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestStrippedMask::TRestStrippedMask() : TRestPatternMask() { Initialize(); }

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
/// corresponding TRestStrippedMask section inside the RML.
///
TRestStrippedMask::TRestStrippedMask(const char* cfgFileName, std::string name)
    : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestStrippedMask::~TRestStrippedMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestStrippedMask::Initialize() {
    SetSectionName(this->ClassName());
    SetType("Stripped");
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::ApplyCommonMaskTransformation
///
Int_t TRestStrippedMask::GetRegion(Double_t x, Double_t y) {
    if (ApplyCommonMaskTransformation(x, y) == 0) return 0;

    Double_t xEval = fStripsThickness / 2. + x;

    Int_t xcont = 0;
    if (xEval > 0) {
        while (xEval > fStripsGap) {
            xEval -= fStripsGap;
            xcont++;
        }
    } else {
        while (xEval < 0) {
            xEval += fStripsGap;
            xcont--;
        }
    }

    if (xEval < fStripsThickness) return 0;

    xcont = xcont % fModulus;
    if (xcont < 0) xcont += fModulus;

    return 1 + xcont % fMaxRegions;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestStrippedMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    RESTMetadata << "-----" << RESTendl;
    RESTMetadata << " - Strips gap : " << fStripsGap << " mm" << RESTendl;
    RESTMetadata << " - Strips thickness : " << fStripsThickness << " mm" << RESTendl;
    RESTMetadata << "+++++" << RESTendl;
}
