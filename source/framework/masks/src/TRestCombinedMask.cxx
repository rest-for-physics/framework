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
/// This class is used to generate a combined mask structure by combining
/// any of the predefined existing masks inheriting from TRestPatternMask.
///
/// The implementation of TRestCombinedMask::GetRegion method will use the
/// region ids of each internal mask to generate a new unique region id.
///
/// ### Examples
///
/// Mask pattern RML definitions can be found inside the file
/// `REST_PATH/examples/masks.rml`.
///
/// The following definition ilustrates a complete RML implementation of a
/// TRestCombinedMask.
///
/// \code
///  <TRestCombinedMask name="combined3" verboseLevel="info">
///       <TRestSpiderMask ...>
///           ...
///       </TRestSpiderMask>
///
///       <TRestRingsMask ...>
///           ...
///       </TRestRingsMask>
///  </TRestCombinedMask>
/// \endcode
///
/// The basic use of this class is provided by the TRestCombinedMask::GetRegion
/// method. For example:
///
/// \code
///     TRestCombinedMask mask("masks.rml", "combined");
///     Int_t id = mask.GetRegion( 12.5, 4.3 );
///     std::cout << "Region id is : " << id << endl;
/// \endcode
///
/// The following figure may be generated using the TRestPatternMask::DrawMonteCarlo
/// method, using the `combined` definition.
///
/// \code
///     TRestCombinedMask mask("masks.rml", "combined");
///     mask.GenerateCombined();
///     TCanvas *c = mask.DrawMonteCarlo(30000);
///     c->Draw();
///     c->Print("combined.png");
/// \endcode
///
/// \htmlonly <style>div.image img[src="combinedmask.png"]{width:600px;}</style> \endhtmlonly
/// ![An illustration of the montecarlo mask test using DrawMonteCarlo](combinedmask.png)
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-06: First implementation of TRestCombinedMask
/// Javier Galan
///
/// \class TRestCombinedMask
/// \author: Javier Galan - javier.galan@unizar.es
///
/// <hr>
///

#include "TRestCombinedMask.h"

#include "TRandom3.h"

ClassImp(TRestCombinedMask);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestCombinedMask::TRestCombinedMask() : TRestPatternMask() { Initialize(); }

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
/// corresponding TRestCombinedMask section inside the RML.
///
TRestCombinedMask::TRestCombinedMask(const char* cfgFileName, std::string name)
    : TRestPatternMask(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestCombinedMask::~TRestCombinedMask() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestCombinedMask::Initialize() {
    SetSectionName(this->ClassName());
    SetType("Combined");

    for (const auto mask : fMasks) {
        mask->Initialize();
        if (mask->GetMaskRadius() > fMaskRadius) fMaskRadius = mask->GetMaskRadius();
    }
}

///////////////////////////////////////////////
/// \brief It returns a number identifying the region where the particle
/// with coordinates (x,y) felt in. The method returns 0 if the particle
/// hits the pattern.
///
/// The particle will be counter-rotated to emulate the mask rotation
/// using the method TRestPatternMask::ApplyCommonMaskTransformation
///
Int_t TRestCombinedMask::GetRegion(Double_t& x, Double_t& y) {
    Int_t region = 0;
    for (const auto mask : fMasks) {
        Int_t id = mask->GetRegion(x, y);
        RESTDebug << "TRestCombinedMask::GetRegion. Mask type: " << mask->GetType() << " region : " << id
                  << RESTendl;
        if (id == 0) return 0;

        region = id + region * mask->GetMaxRegions();
    }

    return region;
}

///////////////////////////////////////////////
/// \brief Implements class initialization through RML
///
void TRestCombinedMask::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    int cont = 0;
    TRestPatternMask* msk = (TRestPatternMask*)this->InstantiateChildMetadata(cont, "Mask");
    while (msk != nullptr) {
        AddMask(msk);
        cont++;
        msk = (TRestPatternMask*)this->InstantiateChildMetadata(cont, "Mask");
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the complete information about the metadata members from this class
///
void TRestCombinedMask::PrintMetadata() {
    TRestPatternMask::PrintMetadata();

    PrintMask();

    Int_t cont = 1;
    for (const auto mask : fMasks) {
        RESTMetadata << " == MASK " << cont << " == " << RESTendl;
        cont++;
        mask->PrintMask();
        RESTMetadata << "++++" << RESTendl;
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members from this class,
/// including common pattern headers, but without common metadata headers.
///
void TRestCombinedMask::PrintMask() {
    PrintMaskMembers();
    RESTMetadata << "++++" << RESTendl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members from this class
/// excluding common metadata headers and any formatting.
///
void TRestCombinedMask::PrintMaskMembers() {
    RESTMetadata << " - Number of masks : " << fMasks.size() << RESTendl;
}
