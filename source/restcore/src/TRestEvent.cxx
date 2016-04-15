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
    fEventID = 0; 
    fEventTime = 0; 
    fSubEventID = 0;
    fSubEventTag = "";
    fOk = true;
}

void TRestEvent::SetTime( Double_t time )
{ 
    Int_t sec = (Int_t)time;
    Int_t nsec = (Int_t) ((time-sec)*1E9);

    fEventTime.SetSec(sec);
    fEventTime.SetNanoSec(nsec);
}

void TRestEvent::SetTime( Double_t seconds, Double_t nanoseconds )
{
    fEventTime.SetSec( seconds );
    fEventTime.SetNanoSec( nanoseconds );
}

void TRestEvent::PrintEvent()
{
    cout << "*******************************************************" << endl;
    cout << " EVENT ID : " << GetID() << " TIME : " << GetTime() << endl;
    cout << " SUB-EVENT ID : " << GetSubID();
    if ( fSubEventTag != "" ) cout << "   SUB-EVENT TAG : \"" << fSubEventTag << "\""; 
    cout << endl;
    if( fOk ) cout << " Status : OK" << endl;
    else cout << " Status : NOT OK" << endl;
    cout << "*******************************************************" << endl;

}

