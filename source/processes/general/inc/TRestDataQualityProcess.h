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

#include "TRestEventProcess.h"

/// A structure to define the rules inside a quality number inside TRestDataQualityProcess
struct DataQualityRules {
    /// The rule type. I.e. obsAverage, metadata, ...
    std::vector<TString> types;

    /// The value of the rule. I.e. observable name, metadata member, ...
    std::vector<TString> values;

    /// The range where it should be found the value of the rule to enable the corresponding bit.
    std::vector<TVector2> ranges;

    /// The bit position for the corresponding rule
    std::vector<Int_t> bits;
};

//! A process to define and store quality numbers
class TRestDataQualityProcess : public TRestEventProcess {
   private:
    /// A vector of custom quality numbers
    std::vector<UInt_t> fQualityNumber;  //<

    /// A vector of tag names for each quality number
    std::vector<TString> fQualityTag;  //<

    /// A vector with the rule definitions for each quality number
    std::vector<DataQualityRules> fRules;  //<

    TRestEvent* fEvent = NULL;  //!

    void InitProcess();
    void EndProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

    Bool_t EvaluateMetadataRule(TString value, TVector2 range);

    /// It sets to 1 the bit of number at position `bitPosition`
    void EnableBit(UInt_t& number, Int_t bitPosition) { number |= (1u << bitPosition); }

    /// It sets to 0 the bit of number at position `bitPosition`
    void DisableBit(UInt_t& number, Int_t bitPosition) { number &= ~(1u << bitPosition); };

    Bool_t isBitEnabled(UInt_t number, Int_t bitPosition) { return number & (1u << bitPosition); }

   protected:
   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDataQualityProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "dataQuality"; }

    TRestDataQualityProcess();
    TRestDataQualityProcess(char* cfgFileName);

    ~TRestDataQualityProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestDataQualityProcess, 1);
};
#endif
