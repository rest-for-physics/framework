///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestLinearTrack.h
///
///             Event class to store a linearized track
///
///             Feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///
///_______________________________________________________________________________


#include "TRestLinearTrack.h"
using namespace std;

ClassImp(TRestLinearTrack)
//______________________________________________________________________________
    TRestLinearTrack::TRestLinearTrack()
{
   // TRestLinearTrack default constructor
   Initialize();
}

//______________________________________________________________________________
TRestLinearTrack::~TRestLinearTrack()
{
   // TRestLinearTrack destructor
}

void TRestLinearTrack::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestLinearTrack";

    fProjectionType = 0;
}

