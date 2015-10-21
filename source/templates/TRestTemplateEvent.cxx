///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateEvent.h
///
///             G4 Event class to store results from Geant4 REST simulation  
///
///             aug 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestTemplateEvent.h"

ClassImp(TRestTemplateEvent)
//______________________________________________________________________________
    TRestTemplateEvent::TRestTemplateEvent()
{
   // TRestTemplateEvent default constructor
   Initialize();
}

//______________________________________________________________________________
TRestTemplateEvent::~TRestTemplateEvent()
{
   // TRestTemplateEvent destructor
}

void TRestTemplateEvent::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestTemplateEvent";

    // Initialize here the components of TRestTemplateEvent
}

void TRestTemplateEvent::PrintEvent()
{
    TRestEvent::PrintEvent();

    // Print the contents of TRestTemplateEvent here

}
