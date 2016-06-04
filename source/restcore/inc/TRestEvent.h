///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEvent.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#ifndef RestCore_TRestEvent
#define RestCore_TRestEvent

#include <iostream>

#include "TObject.h"
#include <TString.h>
#include <TTimeStamp.h>
#include <TPad.h>

class TRestEvent:public TObject {
    protected:
	Int_t fRunOrigin;	    //< run Id number
	Int_t fSubRunOrigin;	    //< Sub-run Id number
        Int_t fEventID;             //< Event identificative number
        Int_t fSubEventID;          //< sub-Event identificative number
        TString fSubEventTag;       //< A short length label to identify the sub-Event
        TTimeStamp fEventTime;      //< Absolute event time
        Bool_t fOk;                 //< Flag to be used by processes to define an event status. fOk=true is the default.

    public:
        //Setters
	void SetRunOrigin( Int_t id ) { fRunOrigin = id; }
	void SetSubRunOrigin( Int_t id ) { fSubRunOrigin = id; }
	
        void SetID( Int_t id ) { fEventID = id; }
        void SetSubID( Int_t id ) { fSubEventID = id; }
        void SetSubEventTag( TString tag ) { fSubEventTag = tag; }

        void SetTime( Double_t time );
        void SetTime( Double_t seconds, Double_t nanoseconds );
        void SetTimeStamp( TTimeStamp time ) { fEventTime = time; }

        void SetState( Bool_t state ) { fOk = state; }
        void SetOK( Bool_t state ) { fOk = state; }

        //Getters
        Int_t GetID( ) { return fEventID; }
        Int_t GetSubID( ) { return fSubEventID; }
        TString GetSubEventTag() { return fSubEventTag; }

	Int_t GetRunOrigin(  ) { return fRunOrigin; }
	Int_t GetSubRunOrigin(  ) { return fSubRunOrigin; }

        Double_t GetTime() { return fEventTime.AsDouble(); }
        TTimeStamp GetTimeStamp() { return fEventTime; }

        Bool_t isOk() { return fOk; }

        // Must be set on the derived events to remove content
        virtual void Initialize() = 0; 

        virtual void PrintEvent();

        virtual TPad *DrawEvent( ){ return NULL; }

        //Construtor
        TRestEvent();
        //Destructor
        virtual ~ TRestEvent();

        ClassDef(TRestEvent, 1);     // REST event superclass
};
#endif
