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
/// and a thickness for the strips. The method TRestRadialStrippedMask::GetRegion
/// will return a unique id for each region in between strips.
///
/// The stripped structure is centered in (0,0) and it can be shifted using
/// the offset defined inside TRestPatternMask. The pattern will be only
/// delimited by the limits imposed inside TRestPatternMask.
///
/// ### Specific stripped metadata parameters
///
/// * **stripsAngle**: This parameter defines the strips angular periodicity.
/// * **stripsThickness**: The thickness of the strips.
/// * **modulus**: A number that defines the range of ids used to identify
/// the different regions inside the stripped pattern. If modulus is 10,
/// then we will only be able to identify up to 10 unique regions. If a
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
/// TRestRadialStrippedMask.
///
/// \code
///	<TRestRadialStrippedMask name="strongback" verboseLevel="warning">
///		<parameter name="maskRadius" value="30cm" />
///		<parameter name="offset" value="(5,5)cm" />
///		<parameter name="rotationAngle" value="30degrees" />
///
///		<parameter name="stripsAngle" value="60degrees" />
///		<parameter name="stripsThickness" value="2.5cm" />
///	</TRestRadialStrippedMask>
/// \endcode
///
/// The basic use of this class is provided by the TRestRadialStrippedMask::GetRegion
/// method. For example:
///
/// \code
///     TRestRadialStrippedMask mask("masks.rml", "radialStrips");
///     Int_t id = mask.GetRegion( 12.5, 4.3 );
/// 	std::cout << "Region id is : " << id << endl;
/// \endcode
///
/// The following figure may be generated using the TRestPatternMask::DrawMonteCarlo
/// method.
///
/// \code
///     TRestRadialStrippedMask mask("masks.rml", "radialStrips");
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///		c->Draw();
///     c->Print("radialstrippedmask.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="radialstrippedmask.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the montecarlo mask test using DrawMonteCarlo](strippedmask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-05: First implementation of TRestRadialStrippedMask
/// Javier Galan
///
/// \class TRestRadialStrippedMask
/// \author: Javier Galan - javier.galan@unizar.es
///
/// <hr>
///

#include "TRestRadialStrippedMask.h"

#include "TRandom3.h"

ClassImp(TRestRadialStrippedMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRadialStrippedMask::TRestRadialStrippedMask() : TRestPatternMask() { Initialize(); }

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
/// corresponding TRestRadialStrippedMask section inside the RML.
///
TRestRadialStrippedMask::TRestRadialStrippedMask(const char* cfgFileName, std::string name)
    : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRadialStrippedMask::~TRestRadialStrippedMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestRadialStrippedMask::Initialize() {
    SetSectionName(this->ClassName());
    SetType("RadialStripped");
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::ApplyCommonMaskTransformation
///
Int_t TRestRadialStrippedMask::GetRegion(Double_t& x, Double_t& y) {
    if (TRestPatternMask::GetRegion(x, y)) return 0;

	TVector2 point(x, y);
	Double_t phi = point.Phi();

	/// phi determines the region where the point is found
	Int_t region = (Int_t) (phi/fStripsAngle);
	region = 1 + region%fMaxRegions;
	std::cout << "Phi: " << phi << " angle: " << fStripsAngle << " Region: " << region << std::endl;

	Double_t angle = 0;
	/// Checking if we hit an arm
	while( angle < 2*TMath::Pi() )
	{
		if( point.Y() < fStripsThickness/2. && point.Y() > -fStripsThickness/2. && point.X() >= 0 )
			return 0;

		point = point.Rotate(fStripsAngle);
		angle += fStripsAngle;
	}

    return 1 + region % fModulus;
}

/////////////////////////////////////////////
/// \brief Prints on screen the complete information about the metadata members from this class
///
void TRestRadialStrippedMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    PrintMaskMembers();
    RESTMetadata << "++++" << RESTendl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// including common pattern headers, but without common metadata headers.
///
void TRestRadialStrippedMask::PrintMask() {
    PrintCommonPatternMembers();
    RESTMetadata << "----" << RESTendl;
    PrintMaskMembers();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// excluding common metadata headers.
///
void TRestRadialStrippedMask::PrintMaskMembers() {
    RESTMetadata << " - Strips angle : " << fStripsAngle * units("degrees") << " degrees" << RESTendl;
    RESTMetadata << " - Strips thickness : " << fStripsThickness << " mm" << RESTendl;
}
