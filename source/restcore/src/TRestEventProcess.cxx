///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventProcess.cxx
///
///             Base class from which to inherit all REST process classes inherit
///             A process class in REST deals with one input, ouput (or both) 
///             single REST events. RestMetadata controls the access to config 
///             files.
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestEventProcess.h"

ClassImp(TRestEventProcess)
//______________________________________________________________________________
    TRestEventProcess::TRestEventProcess()
{
   // TRestEventProcess default constructor
}


//______________________________________________________________________________
TRestEventProcess::~TRestEventProcess()
{
   // TRestEventProcess destructor
}

/*
//______________________________________________________________________________
void TRestEventProcess::InitProcess()
{
   // virtual function to be executed once at the beginning of process
   // (before starting the process of the events)
   cout << GetName() << ": Process initialization..." << endl;
}

//______________________________________________________________________________
void TRestEventProcess::ProcessEvent( TRestEvent *eventInput )
{
   // virtual function to be executed for every event to be processed
}

//______________________________________________________________________________
void TRestEventProcess::EndProcess()
{
   // virtual function to be executed once at the end of the process 
   // (after all events have been processed)
   cout << GetName() << ": Process ending..." << endl;
}
*/
