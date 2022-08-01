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
/// TRestEvent is one of the core classes of REST. Absract class
/// from which all REST "event classes" must derive.
/// REST event classes represent the different holders to store
/// specific event data. TRestEvent contains common data members
/// to all type of events, as it is the timestamp. or the references to
/// the event belongs to, as run and subrun numbers. It contains
/// virtual functions that must be implemented in derived classes
/// like Initialize(), PrintEvent() or DrawEvent().
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///            code (REST v2)
///            Igor G. Irastorza
///
/// \class TRestEvent
///
/// <hr>
///
//////////////////////////////////////////////////////////////////////////

#include "TRestEvent.h"

#include <fmt/color.h>
#include <fmt/core.h>

using namespace std;
using namespace fmt;

ClassImp(TRestEvent);

//////////////////////////////////////////////////////////////////////////
/// TRestEvent default constructor
///
TRestEvent::TRestEvent() {}

//////////////////////////////////////////////////////////////////////////
/// TRestEvent default destructor
///
TRestEvent::~TRestEvent() {}

//////////////////////////////////////////////////////////////////////////
/// Run to initialize event data. To be re-implemented in derived class,
/// you may want to call the base class at the end of the re-implemented,
/// by calling TRestEvent::Initialize();
///
void TRestEvent::Initialize() {
    fEventID = 0;
    fEventTime = 0;
    fSubEventID = 0;
    fSubEventTag = "";
    fOk = true;
    fPad = nullptr;
}

//////////////////////////////////////////////////////////////////////////
/// Set the time of the event
///
void TRestEvent::SetTime(Double_t time) {
    auto seconds = (Int_t)time;
    auto nanoseconds = (Int_t)((time - seconds) * 1E9);

    fEventTime.SetSec(seconds);
    fEventTime.SetNanoSec(nanoseconds);
}

//////////////////////////////////////////////////////////////////////////
/// \brief Clone the content of this TRestEvent object to another
///
/// This method uses default root streamer to do the copying. The efficiency is
/// low. Override recommended.
void TRestEvent::CloneTo(TRestEvent* target) {
    if (this->ClassName() != target->ClassName()) {
        cout << "In TRestEvent::CloneTo() : Event type doesn't match! (This :" << this->ClassName()
             << ", Target : " << target->ClassName() << ")" << endl;
        return;
    }

    TBufferFile buffer(TBuffer::kWrite);
    buffer.MapObject(this);  // register obj in map to handle self reference
    {
        Bool_t isRef = this->TestBit(kIsReferenced);
        ((TObject*)this)->ResetBit(kIsReferenced);

        ((TObject*)this)->Streamer(buffer);

        if (isRef) ((TObject*)this)->SetBit(kIsReferenced);
    }

    // read new object from buffer
    buffer.SetReadMode();
    buffer.ResetMap();
    buffer.SetBufferOffset(0);
    buffer.MapObject(target);  // register obj in map to handle self reference
    target->Streamer(buffer);
    target->ResetBit(kIsReferenced);
    target->ResetBit(kCanDelete);
}

//////////////////////////////////////////////////////////////////////////
/// Set the time of the event
///
void TRestEvent::SetTime(Double_t seconds, Double_t nanoseconds) {
    fEventTime.SetSec((Int_t)seconds);
    fEventTime.SetNanoSec((Int_t)nanoseconds);
}

//////////////////////////////////////////////////////////////////////////
/// Copy the six universal information in TRestEvent from another TRestEvent
///
void TRestEvent::SetEventInfo(TRestEvent* eve) {
    if (eve != nullptr) {
        SetID(eve->GetID());
        SetSubID(eve->GetSubID());
        SetTimeStamp(eve->GetTimeStamp());
        SetSubEventTag(eve->GetSubEventTag());
        SetOK(eve->isOk());
    }
}

void TRestEvent::RestartPad(Int_t nElements) {
    if (fPad != nullptr) {
        delete fPad;
        fPad = nullptr;
    }

    fPad = new TPad(this->GetName(), "", 0., 0., 1., 1.);

    if (nElements == 1) fPad->Divide(1, 1);
    if (nElements == 2) fPad->Divide(2, 1);
    if (nElements == 3 || nElements == 4) fPad->Divide(2, 2);
    if (nElements == 5) fPad->Divide(3, 2);
    if (nElements == 6) fPad->Divide(3, 2);
    if (nElements > 6) fPad->Divide(3, 3);

    if (nElements > 9) {
        cout << "REST_WARNING. TRestEvent::RestartPad. Maximum number of pad "
                "elements reached!"
             << endl;
        cout << "Setting the pad elements to 9" << endl;
        nElements = 9;
    }

    fPad->Draw();
}

void TRestEvent::InitializeWithMetadata(TRestRun* run) { Initialize(); }

void TRestEvent::InitializeReferences(TRestRun* run) { fRun = run; }

//////////////////////////////////////////////////////////////////////////
/// Run to print event data info on console
/// To be re-implemented in derived class.
/// You may want to call the base class inside the re-implemented one,
/// by calling TRestEvent::PrintEvent();
///
void TRestEvent::PrintEvent() const {
    print("\n");
    print(fg(color::orange_red) | emphasis::bold, "EventID: {} - SubEventID: {}\n", fEventID, fSubEventID);
    print(fg(color::steel_blue), "- Timestamp: {}\n", fEventTime.AsString());
    if (!fSubEventTag.IsNull()) {
        print("- Tag: {}\n", fSubEventTag);
    }
    if (!fOk) {
        print(fg(color::red) | emphasis::bold, "- Status not OK\n");
    }
}
