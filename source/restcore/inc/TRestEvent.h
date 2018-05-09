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

#include <iostream>

#include "TObject.h"
#include <TString.h>
#include <TTimeStamp.h>
#include <TPad.h>
#include "TBufferFile.h"


class TRestEvent :public TObject {

protected:
	Int_t fRunOrigin;	        ///< Run ID number of the event
	Int_t fSubRunOrigin;	    ///< Sub-run ID number of the event
	Int_t fEventID;             ///< Event identificative number
	Int_t fSubEventID;          ///< Sub-Event identificative number
	TString fSubEventTag;       ///< A short length label to identify the sub-Event
	TTimeStamp fEventTime;      ///< Absolute event time
	Bool_t fOk;                 ///< Flag to be used by processes to define an event status. fOk=true is the default.

public:
	//Setters
	void SetRunOrigin(Int_t id) { fRunOrigin = id; }
	void SetSubRunOrigin(Int_t id) { fSubRunOrigin = id; }

	void PrintAddress()
	{
		std::cout << this << std::endl;
		std::cout << &fRunOrigin << std::endl;
		std::cout << &fSubRunOrigin << std::endl;
		std::cout << &fEventID << std::endl;
		std::cout << &fSubEventID << std::endl;
		std::cout << &fSubEventTag << std::endl;
		std::cout << &fEventTime << std::endl;
		std::cout << &fOk << std::endl;
	}

	void SetID(Int_t id) { fEventID = id; }
	void SetSubID(Int_t id) { fSubEventID = id; }
	void SetSubEventTag(TString tag) { fSubEventTag = tag; }

	void SetTime(Double_t time);
	void SetTime(Double_t seconds, Double_t nanoseconds);
	void SetTimeStamp(TTimeStamp time) { fEventTime = time; }

	void SetState(Bool_t state) { fOk = state; }
	void SetOK(Bool_t state) { fOk = state; }

	void SetEventInfo(TRestEvent*eve);

	//Getters
	Int_t GetID() { return fEventID; }
	Int_t GetSubID() { return fSubEventID; }
	TString GetSubEventTag() { return fSubEventTag; }

	Int_t GetRunOrigin() { return fRunOrigin; }
	Int_t GetSubRunOrigin() { return fSubRunOrigin; }

	Double_t GetTime() { return fEventTime.AsDouble(); }
	TTimeStamp GetTimeStamp() { return fEventTime; }

	Bool_t isOk() { return fOk; }


	virtual void Initialize() = 0;

	virtual void PrintEvent( int maxTracks = 0, int maxHits = 0);

	//////////////////////////////////////////////////////////////////////////
	/// \brief Draw the event
	///
	/// To be implemented in the derived class
	virtual TPad *DrawEvent( TString option = "" )
    { 
        if( option != "" )
        {
            std::cout << "TRestEvent::DrawEvent. Option = " << option << std::endl;
            std::cout << "REST_Warning. DrawEvent has not been implemented." << std::endl;
        }

        return new TPad(); 
    }


	virtual void CloneTo(TRestEvent* target);

	//Construtor
	TRestEvent();
	//Destructor
	virtual ~TRestEvent();

	ClassDef(TRestEvent, 1);     // REST event superclass
};
#endif
