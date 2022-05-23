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
/// This class defines a set of spider limited by an inner/outter radii.
///
/// The spider radii can be initialized by a set of specific metadata
/// parameters defined inside this class.
///
/// The spider pattern is centered in (0,0) and it can be shifted using
/// the offset defined inside TRestPatternMask. It must be noted that
/// it is the pattern that will be shifted and not the mask.
///
/// ### Specific spider metadata parameters
///
/// * **spiderGap**: This parameter defines the spider periodicity.
/// * **spiderThickness**: The thickness of the spider.
/// * **initialRadius**: The most inner ring radius
/// * **nSpider**: The number of spider to be created
///
/// \note Those parameters can be used to define the generation of a
/// periodic ring pattern. However, it is possible to define any arbitrary
/// ring structure using the method TRestSpiderMask::GenerateSpider. In
/// that case, the previous parametersr should be left undefined.
///
/// ### Common pattern metadata parameters
///
/// On top of the metadata class parameters, we may define common pattern
/// parameters to induce an offset and rotation to the pattern.
///
/// * **offset**: A parameter to shift the spider pattern.
/// * **maskRadius**: A radius defining the limits of the circular mask.
///
/// ### Examples
///
/// Mask pattern RML definitions can be found inside the file
/// `REST_PATH/examples/masks.rml`.
///
/// The following definition ilustrates a complete RML implementation of a
/// TRestSpiderMask.
///
/// \code
///	<TRestSpiderMask name="spider" verboseLevel="warning">
///		<parameter name="maskRadius" value="9cm" />
///		<parameter name="offset" value="(3,3)mm" />
///
///		<parameter name="spiderGap" value="12mm" />
///		<parameter name="spiderThickness" value="10mm" />
///		<parameter name="initialRadius" value="3cm" />
///		<parameter name="nSpider" value="5" />
///	</TRestSpiderMask>
/// \endcode
///
/// The basic use of this class is provided by the TRestSpiderMask::GetRegion
/// method. For example:
///
/// \code
///     TRestSpiderMask mask("masks.rml", "spider");
///     Int_t id = mask.GetRegion( 12.5, 4.3 );
///     std::cout << "Region id is : " << id << endl;
/// \endcode
///
/// The following figure may be generated using the TRestPatternMask::DrawMonteCarlo
/// method.
///
/// \code
///     TRestSpiderMask mask("masks.rml", "spider");
///     mask.GenerateSpider();
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///     c->Draw();
///     c->Print("output.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="spidermask.png"]{width:500px;}</style> \endhtmlonly
/// ![An illustration of the montecarlo mask test using DrawMonteCarlo](spidermask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-05: First implementation of TRestSpiderMask
/// Javier Galan
///
/// \class TRestSpiderMask
/// \author: Javier Galan - javier.galan@unizar.es
///
/// <hr>
///

#include "TRestSpiderMask.h"

#include "TRandom3.h"

ClassImp(TRestSpiderMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestSpiderMask::TRestSpiderMask() : TRestPatternMask() { Initialize(); }

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
/// corresponding TRestSpiderMask section inside the RML.
///
TRestSpiderMask::TRestSpiderMask(const char* cfgFileName, std::string name) : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestSpiderMask::~TRestSpiderMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestSpiderMask::Initialize() {
    SetSectionName(this->ClassName());
    SetType("Spider");

    GenerateSpider();
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::RotateAndTranslate.
///
Int_t TRestSpiderMask::GetRegion(Double_t x, Double_t y) {
    if (ApplyCommonMaskTransformation(x, y) == 0) return 0;

    Double_t r = TMath::Sqrt(x * x + y * y);
    int cont = 1;
    for (const auto& ringRadius : fSpiderRadii) {
        if (r < ringRadius.second && r >= ringRadius.first) return cont;
        cont++;
    }
    return 0;
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::RotateAndTranslate.
///
void TRestSpiderMask::GenerateSpider() {
    fSpiderRadii.clear();
    for (int n = 0; n < fNSpider; n++) {
        Double_t inR = fInitialRadius + n * fSpiderGap;
        Double_t outR = fSpiderThickness + fInitialRadius + n * fSpiderGap;

        std::pair<Double_t, Double_t> p(inR, outR);
        fSpiderRadii.push_back(p);
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestSpiderMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    if (fSpiderGap > 0) metadata << " - Spider gap : " << fSpiderGap << " mm" << endl;
    if (fSpiderThickness > 0) metadata << " - Spider thickness : " << fSpiderThickness << " mm" << endl;
    if (fNSpider > 0) metadata << " - Number of spider : " << fNSpider << endl;
    if (fInitialRadius > 0) metadata << " - Initial radius : " << fInitialRadius << " mm" << endl;
    if (fNSpider > 0) metadata << "-----" << endl;
    if (fSpiderRadii.size() > 0) {
        metadata << "Inner radii : (";
        for (int n = 0; n < fSpiderRadii.size(); n++) {
            if (n > 0) metadata << ", ";
            metadata << fSpiderRadii[n].first;
        }
        metadata << ")" << endl;
    }
    if (fSpiderRadii.size() > 0) {
        metadata << "Outter radii : (";
        for (int n = 0; n < fSpiderRadii.size(); n++) {
            if (n > 0) metadata << ", ";
            metadata << fSpiderRadii[n].second;
        }
        metadata << ")" << endl;
    }
    metadata << "----" << endl;
}
