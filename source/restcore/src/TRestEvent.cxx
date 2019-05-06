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
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// \class      TRestEvent
/// A base class for any REST event
///
/// One of the core classes of REST. Absract class
/// from which all REST "event classes" must derive.
/// REST event classes represent the different holders to store
/// single event data. TRestEvent contain common data members
/// to all type of events, like the timestamp or the references to
/// the run and subrun numbers which they belong to. It contains
/// virtual functions that must be implemented in derived classes
/// like Initialize(), PrintEvent() or DrawEvent().
///
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///            code (REST v2)
///            Igor G. Irastorza
///
/// 2017-Aug:  Major change to xml reading and class startup procedure
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestEvent.h"
using namespace std;

ClassImp(TRestEvent)

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

  fPad = NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Set the time of the event
///
void TRestEvent::SetTime(Double_t time) {
  Int_t sec = (Int_t)time;
  Int_t nsec = (Int_t)((time - sec) * 1E9);

  fEventTime.SetSec(sec);
  fEventTime.SetNanoSec(nsec);
}

//////////////////////////////////////////////////////////////////////////
/// \brief Clone the content of this TRestEvent object to another
///
/// This method uses default root streamer to do the copying. The efficiency is
/// low. Override recommanded.
void TRestEvent::CloneTo(TRestEvent* target) {
  if (this->ClassName() != target->ClassName()) {
    cout << "In TRestEvent::CloneTo() : Event type doesn't match! (This :"
         << this->ClassName() << ", Target : " << target->ClassName() << ")"
         << endl;
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
  fEventTime.SetSec(seconds);
  fEventTime.SetNanoSec(nanoseconds);
}

//////////////////////////////////////////////////////////////////////////
/// Copy the six univeral information in TRestEvent from another TRestEvent
///
void TRestEvent::SetEventInfo(TRestEvent* eve) {
  if (eve != NULL) {
    SetID(eve->GetID());
    SetSubID(eve->GetSubID());
    SetTimeStamp(eve->GetTimeStamp());
    SetSubEventTag(eve->GetSubEventTag());
    SetOK(eve->isOk());
  }
}

void TRestEvent::RestartPad(Int_t nElements) {
  if (fPad != NULL) {
    delete fPad;
    fPad = NULL;
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

//////////////////////////////////////////////////////////////////////////
/// Run to print event data info on console
/// To be re-implemented in derived class.
/// You may want to call the base class inside the re-implemented one,
/// by calling TRestEvent::PrintEvent();
///
void TRestEvent::PrintEvent() {
  cout << "*******************************************************" << endl;
  cout << " EVENT ID : " << GetID() << endl;
  cout << " TIME : " << GetTimeStamp().AsString() << endl;
  cout << " SUB-EVENT ID : " << GetSubID();
  if (fSubEventTag != "")
    cout << "   SUB-EVENT TAG : \"" << fSubEventTag << "\"";
  cout << endl;
  if (fOk)
    cout << " Status : OK" << endl;
  else
    cout << " Status : NOT OK" << endl;
  cout << "*******************************************************" << endl;
}
