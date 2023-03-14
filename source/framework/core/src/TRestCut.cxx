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
///   <cut name="cc3" variable="sAna_ThresholdIntegral" condition=">0">
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
/// 2023-March:
///
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
    while (ele != nullptr) {
        string name = GetParameter("name", ele, "");
        if (name.empty() || name == "Not defined") {
            RESTError << "< cut does not contain a name!" << RESTendl;
            exit(1);
        }

        string cutStr = GetParameter("value", ele, "");
        string variable = GetParameter("variable", ele, "");
        string condition = GetParameter("condition", ele, "");

        if (!cutStr.empty()) {
            cutStr = Replace(cutStr, " AND ", " && ");
            cutStr = Replace(cutStr, " OR ", " || ");
            fCutStrings.push_back(cutStr);
            AddCut(TCut(name.c_str(), cutStr.c_str()));
        } else if (!variable.empty() && !condition.empty()) {
            fParamCut.push_back(std::make_pair(variable, condition));
            string cutVar = variable + condition;
            AddCut(TCut(name.c_str(), cutVar.c_str()));
        } else {
            RESTError << "TRestCut does not contain a valid parameter/condition or cut string!" << RESTendl;
            RESTError << "<cut name='cc1' value='XX>10 AND XX<90'/>" << RESTendl;
            RESTError << "<cut name='cc3' variable='sAna_ThresholdIntegral' condition='>0'" << RESTendl;
            exit(1);
        }

        ele = GetNextElement(ele);
    }
}

void TRestCut::AddCut(TCut cut) {
    if ((string)cut.GetName() == "") {
        RESTWarning << "TRestCut::AddCut: cannot add cut without name!" << RESTendl;
    }
    if ((string)cut.GetTitle() == "") {
        RESTWarning << "TRestCut::AddCut: cannot add empty cut!" << RESTendl;
    }
    for (auto c : fCuts) {
        if ((string)c.GetName() == (string)cut.GetName()) {
            RESTWarning << "TRestCut::AddCut: cut with name \"" << c.GetName() << "\" already added!"
                        << RESTendl;
            return;
        }
    }
    fCuts.push_back(cut);
}

TCut TRestCut::GetCut(string name) {
    for (auto c : fCuts) {
        if ((string)c.GetName() == name) {
            return c;
        }
    }
    return TCut();
}

void TRestCut::PrintMetadata() {
    TRestMetadata::PrintMetadata();
    RESTMetadata << " " << RESTendl;
    RESTMetadata << "Cuts added: " << RESTendl;
    for (const auto& cut : fCuts) {
        RESTMetadata << cut.GetName() << " " << cut.GetTitle() << RESTendl;
    }
    RESTMetadata << "+++" << RESTendl;
}

Int_t TRestCut::Write(const char* name, Int_t option, Int_t bufsize) {
    // write cuts to file.

    for (auto c : fCuts) {
        c.Write();
    }

    return TRestMetadata::Write(name, option, bufsize);
}
