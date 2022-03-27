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
/// TRestCut is a Collection of TCut objects initialized from rml and stored with the output file, e.g:
///
/// <TRestCut/>
///   <cut name="cc1" value="XX>10 AND XX<90"/>
///   <cut name="cc2" value="sAna_ThresholdIntegral<100e3"/>
/// </TRestCut>
///
/// Note that the notations " AND " and " OR " will be replaced by " && " and " || "
/// When saved to ROOT file, this class will save together all its TCut objects
/// Then we can easily draw from analysis tree with these cuts:
///
/// AnalysisTree->Draw("xxx",*cc1)
/// AnalysisTree->Draw("xxx",*cc1 && *cc2 )
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-dec: First concept.
///           Ni Kaixiang
///
/// \class TRestCut
///
/// <hr>
///
//////////////////////////////////////////////////////////////////////////

#include "TRestCut.h"

#include "TRestDataBase.h"
#include "TRestManager.h"
#include "TRestProcessRunner.h"
#include "TRestStringOutput.h"

using namespace std;

ClassImp(TRestCut);

TRestCut::TRestCut() { Initialize(); }

void TRestCut::Initialize() { fCuts.clear(); }

void TRestCut::InitFromConfigFile() {
    auto ele = GetElement("cut");
    while (ele) {
        string name = GetParameter("name", ele, "");
        string cutStr = GetParameter("value", ele, "");
        cutStr = Replace(cutStr, " AND ", " && ");
        cutStr = Replace(cutStr, " OR ", " || ");
        AddCut(TCut(name.c_str(), cutStr.c_str()));
        ele = GetNextElement(ele);
    }
}

void TRestCut::AddCut(const TCut& cut) {
    if ((string)cut.GetName() == "") {
        warning << "TRestCut::AddCut: cannot add cut without name!" << endl;
    }
    if ((string)cut.GetTitle() == "") {
        warning << "TRestCut::AddCut: cannot add empty cut!" << endl;
    }
    for (auto c : fCuts) {
        if ((string)c.GetName() == (string)cut.GetName()) {
            warning << "TRestCut::AddCut: cut with name \"" << c.GetName() << "\" already added!" << endl;
            return;
        }
    }
    fCuts.push_back(cut);
}

TCut TRestCut::GetCut(const string& name) {
    for (auto c : fCuts) {
        if ((string)c.GetName() == name) {
            return c;
        }
    }
    return {};
}

void TRestCut::PrintMetadata() {
    TRestMetadata::PrintMetadata();
    metadata << " " << endl;
    metadata << "Number of TCut objects added: " << fCuts.size() << endl;
    metadata << " " << endl;
    metadata << "+++" << endl;
}

Int_t TRestCut::Write(const char* name, Int_t option, Int_t bufsize) {
    // write cuts to file.

    for (auto c : fCuts) {
        c.Write();
    }

    return TRestMetadata::Write(name, option, bufsize);
}
