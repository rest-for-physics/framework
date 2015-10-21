///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Hits.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 J. Galan
///_______________________________________________________________________________


#include "TRestG4Hits.h"

ClassImp(TRestG4Hits)
//______________________________________________________________________________
    TRestG4Hits::TRestG4Hits() : TRestHits( )
{
   // TRestG4Hits default constructor
}

//______________________________________________________________________________
TRestG4Hits::~TRestG4Hits()
{
   // TRestG4Hits destructor
}
