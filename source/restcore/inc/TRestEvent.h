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
   Int_t fEventID;              //< third identificative number
   // with these 3 numbers one should be able to uniquely identify an event
   // the first and second ones can refer to a run and subrun and the third one to the order number
   // of the event inside the subrun, hence their names...
   // the precise meaning of the indexes may depend on the inherited class
   // (simulation or daq, e.g.)
   TTimeStamp fEventTime;              ///< Event absolute time
   TString fEventClassName;		

 public:
   //Setters
   void SetEventID(Int_t eventid) { fEventID = eventid; }
   void SetEventTime(Double_t time) { 
   Int_t sec = (Int_t)time;
   Int_t nsec = (Int_t) ((time-sec)*1E9);
   
   fEventTime.SetSec(sec);
   fEventTime.SetNanoSec(nsec);
   
   }

   //Getters
   Int_t GetEventID() { return fEventID; }
   Double_t GetEventTime() { return fEventTime.AsDouble(); }
   TString GetClassName() { return fEventClassName; }

   // Must be set on the derived events to remove content
   virtual void Initialize() = 0; 

   virtual void PrintEvent();
   
   virtual TPad *DrawEvent( ){ return NULL; }
   
   //Construtor
   TRestEvent();
   //Destructor
   virtual ~ TRestEvent();

   ClassDef(TRestEvent, 2);     // REST event superclass
};
#endif
