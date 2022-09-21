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
/// This class defines a set of rings limited by an inner/outter radii.
///
/// The rings radii can be initialized by a set of specific metadata
/// parameters defined inside this class.
///
/// The rings pattern is centered in (0,0) and it can be shifted using
/// the offset defined inside TRestPatternMask. It must be noted that
/// it is the pattern that will be shifted and not the mask.
///
/// ### Specific rings metadata parameters
///
/// * **ringsGap**: This parameter defines the rings periodicity.
/// * **ringsThickness**: The thickness of the rings.
/// * **initialRadius**: The most inner ring radius
/// * **nRings**: The number of rings to be created
///
/// \note Those parameters can be used to define the generation of a
/// periodic ring pattern. However, it is possible to define any arbitrary
/// ring structure using the method TRestRingsMask::GenerateRings. In
/// that case, the previous parametersr should be left undefined.
///
/// ### Common pattern metadata parameters
///
/// On top of the metadata class parameters, we may define common pattern
/// parameters to induce an offset and rotation to the pattern.
///
/// * **offset**: A parameter to shift the rings pattern.
/// * **maskRadius**: A radius defining the limits of the circular mask.
///
/// ### Examples
///
/// Mask pattern RML definitions can be found inside the file
/// `REST_PATH/examples/masks.rml`.
///
/// The following definition ilustrates a complete RML implementation of a
/// TRestRingsMask.
///
/// \code
///	<TRestRingsMask name="rings" verboseLevel="warning">
///		<parameter name="maskRadius" value="9cm" />
///		<parameter name="offset" value="(3,3)mm" />
///
///		<parameter name="ringsGap" value="12mm" />
///		<parameter name="ringsThickness" value="10mm" />
///		<parameter name="initialRadius" value="3cm" />
///		<parameter name="nRings" value="5" />
///	</TRestRingsMask>
/// \endcode
///
/// The basic use of this class is provided by the TRestRingsMask::GetRegion
/// method. For example:
///
/// \code
///     TRestRingsMask mask("masks.rml", "rings");
///     Int_t id = mask.GetRegion( 12.5, 4.3 );
///     std::cout << "Region id is : " << id << endl;
/// \endcode
///
/// The following figure may be generated using the TRestPatternMask::DrawMonteCarlo
/// method.
///
/// \code
///     TRestRingsMask mask("masks.rml", "rings");
///     mask.GenerateRings();
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///     c->Draw();
///     c->Print("output.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="ringsmask.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the montecarlo mask test using DrawMonteCarlo](ringsmask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-05: First implementation of TRestRingsMask
/// Javier Galan
///
/// \class TRestRingsMask
/// \author: Javier Galan - javier.galan@unizar.es
///
/// <hr>
///

#include "TRestRingsMask.h"

#include "TRandom3.h"

ClassImp(TRestRingsMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRingsMask::TRestRingsMask() : TRestPatternMask() { Initialize(); }

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
/// corresponding TRestRingsMask section inside the RML.
///
TRestRingsMask::TRestRingsMask(const char* cfgFileName, std::string name) : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRingsMask::~TRestRingsMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestRingsMask::Initialize() {
    SetSectionName(this->ClassName());
    SetType("Rings");

    if (fNRings > 0) GenerateRings();
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::RotateAndTranslate.
///
Int_t TRestRingsMask::GetRegion(Double_t x, Double_t y) {
    if (TRestPatternMask::GetRegion(x, y) == 0) return 0;

    Double_t r = TMath::Sqrt(x * x + y * y);
    int cont = 0;
    for (const auto& ringRadius : fRingsRadii) {
        if (r < ringRadius.second && r >= ringRadius.first) return cont % fMaxRegions + 1;
        cont++;
    }
    return 0;
}

///////////////////////////////////////////////
/// \brief It will initialize the variable fRingsRadii using the
/// number of radius, initial radius, the rings gap and thickness.
///
void TRestRingsMask::GenerateRings() {
    fRingsRadii.clear();
    for (int n = 0; n < fNRings; n++) {
        Double_t inR = fInitialRadius + n * fRingsGap;
        Double_t outR = fRingsThickness + fInitialRadius + n * fRingsGap;

        std::pair<Double_t, Double_t> p(inR, outR);
        fRingsRadii.push_back(p);
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members from this class
///
void TRestRingsMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    PrintMaskMembers();
    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// including common pattern headers, but without common metadata headers.
///
void TRestRingsMask::PrintMask() {
    PrintCommonPatternMembers();
    RESTMetadata << "----" << RESTendl;
    PrintMaskMembers();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// excluding common metadata headers.
///
void TRestRingsMask::PrintMaskMembers() {
    if (fRingsGap > 0) RESTMetadata << " - Rings gap : " << fRingsGap << " mm" << RESTendl;
    if (fRingsThickness > 0) RESTMetadata << " - Rings thickness : " << fRingsThickness << " mm" << RESTendl;
    if (fNRings > 0) RESTMetadata << " - Number of rings : " << fNRings << RESTendl;
    if (fInitialRadius > 0) RESTMetadata << " - Initial radius : " << fInitialRadius << " mm" << RESTendl;
    if (fRingsRadii.size() > 0) {
        if (fNRings > 0) RESTMetadata << "-----" << RESTendl;
        RESTMetadata << "Inner radii : (";
        for (int n = 0; n < fRingsRadii.size(); n++) {
            if (n > 0) RESTMetadata << ", ";
            RESTMetadata << fRingsRadii[n].first;
        }
        RESTMetadata << ")" << RESTendl;
    }
    if (fRingsRadii.size() > 0) {
        RESTMetadata << "Outter radii : (";
        for (int n = 0; n < fRingsRadii.size(); n++) {
            if (n > 0) RESTMetadata << ", ";
            RESTMetadata << fRingsRadii[n].second;
        }
        RESTMetadata << ")" << RESTendl;
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestRingsMask,
/// excluding common metadata headers.
///
void TRestRingsMask::PrintRings() {
    for (int n = 0; n < fRingsRadii.size(); n++)
        std::cout << n << " - " << fRingsRadii[n].first << " - " << fRingsRadii[n].second << std::endl;
}
