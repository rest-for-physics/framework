///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateEvent.h
///
///             Template Event class from REST
///
///             aug 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestTemplateEvent
#define RestCore_TRestTemplateEvent

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TRestEvent.h>

class TRestTemplateEvent: public TRestEvent {

    protected:

        // Insert here the fields from TRestTemplateEvent

    public:

        // Add getters and setters for this template event

        virtual void Initialize();

        virtual void PrintEvent();


        //Construtor
        TRestTemplateEvent();
        //Destructor
        virtual ~ TRestTemplateEvent();

        ClassDef(TRestTemplateEvent, 1);     // REST event superclass
};
#endif
