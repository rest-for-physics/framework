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

#ifndef RestCore_TRestDataQualityRules
#define RestCore_TRestDataQualityRules

#include "TObject.h"
#include "TString.h"
#include "TVector2.h"

//! A class to define the properties of a rule inside TRestDataQualityRules
class TRestDataQualityRules : public TObject {
   private:
    /// The rule type. I.e. obsAverage, metadata, ...
    std::vector<TString> fTypes;  //<

    /// The value of the rule. I.e. observable name, metadata member, ...
    std::vector<TString> fValues;  //<

    /// The range where it should be found the value of the rule to enable the corresponding bit.
    std::vector<TVector2> fRanges;  //<

    /// The bit position for the corresponding rule
    std::vector<Int_t> fBits;  //<

    Bool_t EvaluateMetadataRule(TString value, TVector2 range);

   public:
    void AddRule(TString type, TString value, TVector2 range, Int_t bit) {
        fTypes.push_back(type);
        fValues.push_back(value);
        fRanges.push_back(range);
        fBits.push_back(bit);
    }

    void ClearRules() {
        fTypes.clear();
        fValues.clear();
        fRanges.clear();
        fBits.clear();
    }

    Int_t GetNumberOfRules() { return fTypes.size(); }

    std::vector<TString> GetTypes() { return fTypes; }

    std::vector<TString> GetValues() { return fValues; }

    std::vector<TVector2> GetRanges() { return fRanges; }

    std::vector<Int_t> GetBits() { return fBits; }

    TString GetType(Int_t n) {
        if (fTypes.size() <= n)
            return "";
        else
            return fTypes[n];
    }

    TString GetValue(Int_t n) {
        if (fValues.size() <= n)
            return "";
        else
            return fValues[n];
    }

    TVector2 GetRange(Int_t n) {
        if (fRanges.size() <= n)
            return TVector2(0, 0);
        else
            return fRanges[n];
    }

    Int_t GetBit(Int_t n) {
        if (fBits.size() <= n)
            return -1;
        else
            return fBits[n];
    }

    TRestDataQualityRules();

    ~TRestDataQualityRules();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestDataQualityRules, 1);
};
#endif
