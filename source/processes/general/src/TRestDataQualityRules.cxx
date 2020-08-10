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

//////////////////////////////////////////////////////////////////////////
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-May:  First implementation and concept
///             Javier Galan
///
/// \class      TRestDataQualityRules
/// \author     Javier Galan
///
/// <hr>
///
#include <iostream>

#include "TRestDataQualityRules.h"
using namespace std;

ClassImp(TRestDataQualityRules);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataQualityRules::TRestDataQualityRules() {}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataQualityRules::~TRestDataQualityRules() {}

Bool_t TRestDataQualityRules::EvaluateMetadataRule(TString value, TVector2 range) {
    /*
       vector<string> results = REST_StringHelper::Split((string)value, "::", false, true);

       if (results.size() == 2) {
       if (fRunInfo->GetMetadataClass(results[0])) {
       string val = fRunInfo->GetMetadataClass(results[0])->GetDataMemberValue(results[1]);
       Double_t dblVal = StringToDouble(val);

    // If the metadata value is in range we return true
    if (dblVal >= range.X() && dblVal <= range.Y()) return true;
    } else {
    ferr << "TRestDataQualityRules::EvaluateMetadataRule." << endl;
    ferr << "Metadata class " << results[0] << " is not available inside TRestRun" << endl;
    }
    } else
    ferr << "TRestDataQualityRules::EvaluateMetadataRule. Wrong number of elements found" << endl;
    */
    cout << "TRestDataQualityRules::EvaluateMetadataRule is not implemented!" << endl;
    return false;
}
