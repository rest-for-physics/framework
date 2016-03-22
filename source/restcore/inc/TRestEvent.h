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
        Int_t fEventID;             //< Event identificative number
        Int_t fSubEventID;          //< sub-Event identificative number
        TString fSubEventTag;       //< A short length label to identify the sub-Event
        TTimeStamp fEventTime;      //< Absolute event time
        Bool_t fOk;                 //< Flag to be used by processes to define an event status. fOk=true is the default.

    public:
        //Setters
        void SetEventID( Int_t id ) { fEventID = id; }
        void SetSubEventID( Int_t id ) { fSubEventID = id; }
        void SetSubEventTag( TString tag ) { fSubEventTag = tag; }

        void SetEventTime( Double_t time );
        void SetEventTime( Double_t seconds, Double_t nanoseconds );
        void SetEventTimeStamp( TTimeStamp time ) { fEventTime = time; }

        void SetState( Bool_t state ) { fOk = state; }
        void SetOK( Bool_t state ) { fOk = state; }

        //Getters
        Int_t GetEventID( ) { return fEventID; }
        Int_t GetSubEventID( ) { return fSubEventID; }
        TString GetSubEventTag() { return fSubEventTag; }

        Double_t GetEventTime() { return fEventTime.AsDouble(); }
        TTimeStamp GetEventTimeStamp() { return fEventTime; }

 //       TString GetClassName() { return fEventClassName; }
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
