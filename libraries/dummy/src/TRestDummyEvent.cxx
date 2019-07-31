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
/// TRestDummyEvent is an event class used to define the properties of an
/// specific event data.
///
/// TODO. Create an appropriate documentation here (if needed).
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2019-May: First concept and implementation of TRestDummyEvent class.
///             Javier Galan
///
/// \class      TRestDummyEvent
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestDummyEvent.h"
#include "TRestTools.h"

using namespace std;
using namespace TMath;

ClassImp(TRestDummyEvent);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDummyEvent::TRestDummyEvent() { fPad = NULL; }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDummyEvent::~TRestDummyEvent() {}

///////////////////////////////////////////////
/// \brief Function to initialize any event members requiring
/// initialization.
///
void TRestDummyEvent::Initialize() { TRestEvent::Initialize(); }

///////////////////////////////////////////////
/// \brief Function to define any drawing of this particular event data
///
TPad* TRestDummyEvent::DrawEvent(TString option) {
    vector<string> optList = TRestTools::GetOptions((string)option);

    // If `print` is found in the options the event data will be dumped
    // on screen before drawing the event
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n] == "print") this->PrintEvent();
    }

    optList.erase(std::remove(optList.begin(), optList.end(), "print"), optList.end());

    if (optList.size() == 0) optList.push_back("TODO");

    if (fPad != NULL) {
        delete fPad;
        fPad = NULL;
    }

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    fPad->Divide(1, 1);
    fPad->Draw();

    // TOBE implemented a drawing method for the event data

    return fPad;
}

///////////////////////////////////////////////
/// \brief Function to print event data members
///
void TRestDummyEvent::PrintEvent() {
    TRestEvent::PrintEvent();

    cout << "Dummy value : " << GetDummyValue() << endl;
    cout << "Direction : ( " << GetDummyVectorX() << ", " << GetDummyVectorY() << ", " << GetDummyVectorZ()
         << " )" << endl;
    cout << endl;
}
