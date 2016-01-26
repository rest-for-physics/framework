///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEvent.cxx
///
///             Base class from which to inherit all other event classes in REST 
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestEvent.h"
using namespace std;

ClassImp(TRestEvent)
//______________________________________________________________________________
    TRestEvent::TRestEvent()
{

   // TRestEvent default constructor
}

//______________________________________________________________________________
TRestEvent::~TRestEvent()
{
   // TRestEvent destructor
}

void TRestEvent::Initialize()
{
 fEventID = 0; fEventTime=0; fEventClassName="TRestEvent"; 
}

void TRestEvent::PrintEvent()
{
    cout << "*******************************************************" << endl;
    cout << " EVENT : " << GetEventID() << " TIME : " << GetEventTime() << endl;
    cout << "*******************************************************" << endl;

}

