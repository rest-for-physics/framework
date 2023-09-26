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

#ifndef RestCore_TRestDataQualityProcess
#define RestCore_TRestDataQualityProcess

#include "TRestDataQualityRules.h"
#include "TRestEventProcess.h"

//! A process to define and store quality numbers
class TRestDataQualityProcess : public TRestEventProcess {
   private:
    /// A std::vector of custom quality numbers
    std::vector<UInt_t> fQualityNumber;  //<

    /// A std::vector of tag names for each quality number
    std::vector<TString> fQualityTag;  //<

    /// A std::vector with the rule definitions for each quality number
    std::vector<TRestDataQualityRules> fRules;  //<

    TRestEvent* fEvent = nullptr;  //!

    void InitProcess() override;
    void EndProcess() override;

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

    Bool_t EvaluateMetadataRule(TString value, TVector2 range);

    /// It sets to 1 the bit of number at position `bitPosition`
    void EnableBit(UInt_t& number, Int_t bitPosition) { number |= (1u << bitPosition); }

    /// It sets to 0 the bit of number at position `bitPosition`
    void DisableBit(UInt_t& number, Int_t bitPosition) { number &= ~(1u << bitPosition); };

    Bool_t isBitEnabled(UInt_t number, Int_t bitPosition) { return number & (1u << bitPosition); }

   protected:
   public:
    RESTValue GetInputEvent() const override { return fEvent; }
    RESTValue GetOutputEvent() const override { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override;

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDataQualityProcess; }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "dataQuality"; }

    TRestDataQualityProcess();
    TRestDataQualityProcess(const char* configFilename);

    ~TRestDataQualityProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDefOverride(TRestDataQualityProcess, 1);
};
#endif
