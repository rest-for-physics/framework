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
/// This class is used to generate a spider mask structure defined by arms
/// distributed at a fixed angular period, defined by `fArmsSeparationAngle`,
///
/// The spider pattern is centered in (0,0) and it can be shifted using
/// the offset defined inside TRestPatternMask. It must be noted that
/// it is the pattern that will be shifted and not the mask. The circular
/// mask imposed by TRestPatternMask will still be centered at (0,0).
///
/// ### Specific spider metadata parameters
///
/// * **armsSeparationAngle**: This parameter defines the spider periodicity
/// in radians.
/// * **armsWidth**: The width of each arm in radians.
/// * **initialRadius**: The radius from which the ring structure starts to
/// be effective.
/// * **internalRegionRadius**: If different from 0, which is the default,
/// it will define an additional circular region inside the `initialRadius`
/// where the spider structure starts.
///
/// \note Once those parameters have been initialized, and when using this
/// class in a stand-alone mode, we need to call the method
/// TRestSpiderMask::GenerateSpider in order to initialize the internal
/// data members TRestSpiderMask::fPositiveRanges and
/// TRestSpiderMask::fNegativeRanges that are effectively used inside the
/// TRestSpiderMask::GetRegion calculation.
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
///  <TRestSpiderMask name="spider3" verboseLevel="info">
///    <parameter name="maskRadius" value="20cm"/>
///    <parameter name="offset" value="(0,0)cm"/>
///    <parameter name="rotationAngle" value="30deg"/>
///
///    <parameter name="armsWidth" value="5deg"/>
///    <parameter name="armsSeparationAngle" value="60degrees"/>
///    <parameter name="initialRadius" value="6cm"/>
///  </TRestSpiderMask>
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
/// method, using `spider1`, `spider2` and `spider3` definitions.
///
/// \code
///     TRestSpiderMask mask("masks.rml", "spider2");
///     mask.GenerateSpider();
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///     c->Draw();
///     c->Print("spider2.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="spidermask.png"]{width:1000px;}</style> \endhtmlonly
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
    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();

    Initialize();
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

    if (fArmsSeparationAngle > 0) GenerateSpider();
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::ApplyCommonMaskTransformation
///
Int_t TRestSpiderMask::GetRegion(Double_t& x, Double_t& y) {
    if (TRestPatternMask::GetRegion(x, y)) return 0;

    Double_t d = TMath::Sqrt(x * x + y * y);

    if (fArmsSeparationAngle == 0 || d < fInitialRadius) {
        if (fInternalRegionRadius > 0 && d < fInternalRegionRadius) return 1;

        return 0;
    }

    Double_t cos_angle = y / d;

    if (x >= 0) {
        int region = 1;
        for (unsigned int n = 0; n < fPositiveRanges.size() - 1; n++) {
            if (cos_angle < fPositiveRanges[n].second && cos_angle > fPositiveRanges[n + 1].first)
                return region % fMaxRegions + 1;
            region++;
        }
        if (cos_angle < fPositiveRanges.back().second && cos_angle >= -1) return region % fMaxRegions + 1;

    } else {
        int region = fPositiveRanges.size() + 1;

        if (cos_angle < fNegativeRanges[0].first && cos_angle >= -1) return region % fMaxRegions + 1;
        region++;

        for (unsigned int n = 0; n < fNegativeRanges.size() - 1; n++) {
            if (cos_angle > fNegativeRanges[n].second && cos_angle < fNegativeRanges[n + 1].first)
                return region % fMaxRegions + 1;
            region++;
        }
    }

    return 0;
}

///////////////////////////////////////////////
/// \brief This method is used to initialize the spider arm angles data
/// members that are used to determine if a particle hits the spider
/// pattern.
///
void TRestSpiderMask::GenerateSpider() {
    if (fArmsSeparationAngle <= 0) return;

    fPositiveRanges.clear();
    fNegativeRanges.clear();

    std::pair<Double_t, Double_t> additional_negative = {-1, -1};

    // The angle parameter could introduce an offset but we let finally this task to TRestPatternMask
    Double_t angle = 0;
    do {
        Double_t angle_down = angle - fArmsWidth / 2.;
        Double_t angle_up = angle + fArmsWidth / 2.;

        if (angle_down < 0) {
            additional_negative = {2 * TMath::Pi() + angle_down, 2 * TMath::Pi()};
            fPositiveRanges.push_back({0, angle_up});

        } else if (angle_up > TMath::Pi() && angle_down < TMath::Pi()) {
            fPositiveRanges.push_back({angle_down, TMath::Pi()});
            fNegativeRanges.push_back({TMath::Pi(), angle_up});
        } else if (angle_up < TMath::Pi()) {
            fPositiveRanges.push_back({angle_down, angle_up});
        } else if (angle_down >= TMath::Pi()) {
            fNegativeRanges.push_back({angle_down, angle_up});
        }

        angle += fArmsSeparationAngle;

    } while (angle + 1.e-3 < 2 * TMath::Pi());

    if (additional_negative.first != -1 && additional_negative.second != -1)
        fNegativeRanges.push_back(additional_negative);

    RESTDebug << "Printing positive spider angles" << RESTendl;
    RESTDebug << "-------------------------------" << RESTendl;
    for (unsigned int n = 0; n < fPositiveRanges.size(); n++) {
        RESTDebug << "n : " << n << " from : " << 180 * fPositiveRanges[n].first / TMath::Pi() << " to "
                  << 180 * fPositiveRanges[n].second / TMath::Pi() << RESTendl;
    }

    RESTDebug << "Printing negative spider angles" << RESTendl;
    RESTDebug << "-------------------------------" << RESTendl;
    for (unsigned int n = 0; n < fNegativeRanges.size(); n++) {
        RESTDebug << "n : " << n << " from : " << 180 * fNegativeRanges[n].first / TMath::Pi() << " to "
                  << 180 * fNegativeRanges[n].second / TMath::Pi() << RESTendl;
    }

    for (unsigned int n = 0; n < fNegativeRanges.size(); n++) {
        fNegativeRanges[n].first = TMath::Cos(fNegativeRanges[n].first);
        fNegativeRanges[n].second = TMath::Cos(fNegativeRanges[n].second);
    }

    for (unsigned int n = 0; n < fPositiveRanges.size(); n++) {
        fPositiveRanges[n].first = TMath::Cos(fPositiveRanges[n].first);
        fPositiveRanges[n].second = TMath::Cos(fPositiveRanges[n].second);
    }

    RESTDebug << "Printing positive spider angles" << RESTendl;
    RESTDebug << "-------------------------------" << RESTendl;
    for (unsigned int n = 0; n < fPositiveRanges.size(); n++) {
        RESTDebug << "n : " << n << " from : " << fPositiveRanges[n].first << " to "
                  << fPositiveRanges[n].second << RESTendl;
    }

    RESTDebug << "Printing negative spider cosines" << RESTendl;
    RESTDebug << "--------------------------------" << RESTendl;
    for (unsigned int n = 0; n < fNegativeRanges.size(); n++) {
        RESTDebug << "n : " << n << " from : " << fNegativeRanges[n].first << " to "
                  << fNegativeRanges[n].second << RESTendl;
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the complete information about the metadata members from this class
///
void TRestSpiderMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    PrintMaskMembers();
    RESTMetadata << "----" << RESTendl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members from this class,
/// including common pattern headers, but without common metadata headers.
///
void TRestSpiderMask::PrintMask() {
    PrintCommonPatternMembers();
    RESTMetadata << "----" << RESTendl;
    PrintMaskMembers();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members from this class,
/// excluding common metadata headers.
///
void TRestSpiderMask::PrintMaskMembers() {
    RESTMetadata << " - Arms separation angle : " << fArmsSeparationAngle * units("degrees") << " degrees"
                 << RESTendl;
    RESTMetadata << " - Arms angular width : " << fArmsWidth * units("degrees") << " degrees" << RESTendl;
    RESTMetadata << " - Spider start radius : " << fInitialRadius * units("cm") << " cm" << RESTendl;
    RESTMetadata << " - Internal region radius : " << fInternalRegionRadius * units("cm") << " cm"
                 << RESTendl;

    if (fPositiveRanges.size() > 0) {
        RESTMetadata << "-------------------------------" << RESTendl;
        for (unsigned int n = 0; n < fPositiveRanges.size(); n++) {
            RESTDebug << "n : " << n << " from : " << 180 * fPositiveRanges[n].first / TMath::Pi() << " to "
                      << 180 * fPositiveRanges[n].second / TMath::Pi() << RESTendl;
        }

        RESTMetadata << "Positive ranges : {";
        for (unsigned int n = 0; n < fPositiveRanges.size(); n++) {
            if (n > 0) RESTMetadata << ", ";
            RESTMetadata << "(" << fPositiveRanges[n].first << ", " << fPositiveRanges[n].second << ")";
        }
        RESTMetadata << "}" << RESTendl;

        RESTMetadata << "Negative ranges : {";
        for (unsigned int n = 0; n < fNegativeRanges.size(); n++) {
            if (n > 0) RESTMetadata << ", ";
            RESTMetadata << "(" << fNegativeRanges[n].first << ", " << fNegativeRanges[n].second << ")";
        }
        RESTMetadata << "}" << RESTendl;
    }
}
