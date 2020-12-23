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

#ifndef RestCore_TRestDetectorReadout
#define RestCore_TRestDetectorReadout

#include <iostream>

#include "TObject.h"

#include "TRestDetectorReadoutPlane.h"
#include "TRestMetadata.h"

/// A metadata class to generate/store a readout description.
class TRestDetectorReadout : public TRestMetadata {
   private:
    void InitFromConfigFile();

    void Initialize();

    Bool_t fDecoding;  ///< Defines if a decoding file was used to set the relation
                       ///< between a physical readout channel id and a signal daq id

    Int_t fNReadoutPlanes;  ///< Number of readout planes present on the readout
    std::vector<TRestDetectorReadoutPlane>
        fReadoutPlanes;  ///< A vector storing the TRestDetectorReadoutPlane definitions.

#ifndef __CINT__
    Int_t fMappingNodes;  //!///< Number of nodes per axis used on the readout
                          //! coordinate mapping. See also TRestDetectorReadoutMapping.
    vector<TRestDetectorReadoutModule> fModuleDefinitions;  //!///< A vector storing the different
                                                            //! TRestDetectorReadoutModule definitions.
#endif

    void ValidateReadout();

   public:
    TRestDetectorReadoutPlane& operator[](int p) { return fReadoutPlanes[p]; }

    TRestDetectorReadoutPlane* GetReadoutPlane(int p);
    void AddReadoutPlane(TRestDetectorReadoutPlane plane);

    /////////////////////////////////////
    //{
    TRestDetectorReadoutPlane* GetReadoutPlaneWithID(int id);
    TRestDetectorReadoutModule* GetReadoutModuleWithID(int id);
    TRestDetectorReadoutChannel* GetReadoutChannelWithdaqID(int daqId);
    //}
    /////////////////////////////////////

    Int_t GetNumberOfReadoutPlanes();
    Int_t GetNumberOfModules();
    Int_t GetNumberOfChannels();

    Int_t GetModuleDefinitionId(TString name);

    /////////////////////////////////////
    //{
    TRestDetectorReadoutModule* ParseModuleDefinition(TiXmlElement* moduleDefinition);
    void GetPlaneModuleChannel(Int_t daqID, Int_t& planeID, Int_t& moduleID, Int_t& channelID);
    Int_t GetHitsDaqChannel(TVector3 hitpos, Int_t& planeID, Int_t& moduleID, Int_t& channelID);
    Double_t GetX(Int_t signalID);
    Double_t GetY(Int_t signalID);
    //}
    /////////////////////////////////////

    Double_t GetX(Int_t planeID, Int_t modID, Int_t chID);
    Double_t GetY(Int_t planeID, Int_t modID, Int_t chID);

    // Detal Level:
    // 0->this readout
    // 1->+all readout plane
    // 2->+all readout module
    // 3->+all readout channel
    // 4->+all readout pixel
    void PrintMetadata() { PrintMetadata(1); }
    void PrintMetadata(Int_t DetailLevel);

    void Draw();

    // Construtor
    TRestDetectorReadout();
    TRestDetectorReadout(const char* cfgFileName);
    TRestDetectorReadout(const char* cfgFileName, std::string name);
    // Destructor
    virtual ~TRestDetectorReadout();

    ClassDef(TRestDetectorReadout, 1);
};
#endif
