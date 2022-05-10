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

#ifndef RestCore_TRestEvent
#define RestCore_TRestEvent

#include <TBufferFile.h>
#include <TObject.h>
#include <TPad.h>
#include <TString.h>
#include <TTimeStamp.h>

#include <iostream>

#include "TRestTools.h"

class TRestRun;

/// A base class for any REST event
class TRestEvent : public TObject {
   protected:
    Int_t fRunOrigin;       ///< Run ID number of the event
    Int_t fSubRunOrigin;    ///< Sub-run ID number of the event
    Int_t fEventID;         ///< Event identification number
    Int_t fSubEventID;      ///< Sub-Event identification number
    TString fSubEventTag;   ///< A short length label to identify the sub-Event
    TTimeStamp fEventTime;  ///< Absolute event time
    Bool_t fOk;  ///< Flag to be used by processes to define an event status. fOk=true is the default.

#ifndef __CINT__

    TPad* fPad;  //!
#endif

    void RestartPad(Int_t nElements);

   public:
    // Setters
    inline void SetRunOrigin(Int_t run_origin) { fRunOrigin = run_origin; }
    inline void SetSubRunOrigin(Int_t sub_run_origin) { fSubRunOrigin = sub_run_origin; }

    inline void SetID(Int_t id) { fEventID = id; }
    inline void SetSubID(Int_t id) { fSubEventID = id; }
    inline void SetSubEventTag(const TString& tag) { fSubEventTag = tag; }

    void SetTime(Double_t time);
    void SetTime(Double_t seconds, Double_t nanoseconds);
    inline void SetTimeStamp(const TTimeStamp& time) { fEventTime = time; }

    inline void SetState(Bool_t state) { fOk = state; }
    inline void SetOK(Bool_t state) { fOk = state; }

    void SetEventInfo(TRestEvent* eve);

    // Getters
    inline Int_t GetID() const { return fEventID; }
    inline Int_t GetSubID() const { return fSubEventID; }
    inline TString GetSubEventTag() const { return fSubEventTag; }

    inline Int_t GetRunOrigin() const { return fRunOrigin; }
    inline Int_t GetSubRunOrigin() const { return fSubRunOrigin; }

    inline Double_t GetTime() const { return fEventTime.AsDouble(); }
    inline TTimeStamp GetTimeStamp() const { return fEventTime; }

    inline Bool_t isOk() const { return fOk; }

    virtual void Initialize() = 0;
    virtual void InitializeWithMetadata(TRestRun* r);

    virtual void PrintEvent() const;

    //////////////////////////////////////////////////////////////////////////
    /// \brief Draw the event
    ///
    /// To be implemented in the derived class
    virtual TPad* DrawEvent(const TString& option = "") {
        UNUSED(option);
        return new TPad();
    }

    virtual void CloneTo(TRestEvent* target);

    // Constructor
    TRestEvent();
    // Destructor
    virtual ~TRestEvent();

    ClassDef(TRestEvent, 1);
};
#endif
