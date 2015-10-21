///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestSignalEvent.h"

ClassImp(TRestSignalEvent)
//______________________________________________________________________________
    TRestSignalEvent::TRestSignalEvent()
{
   // TRestSignalEvent default constructor
   Initialize();
}

//______________________________________________________________________________
TRestSignalEvent::~TRestSignalEvent()
{
   // TRestSignalEvent destructor
}

void TRestSignalEvent::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestSignalEvent";

}

void TRestSignalEvent::PrintEvent()
{
    TRestEvent::PrintEvent();


}
