///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4toHitsProcess.cxx
///
///
///             Simple process to convert a TRestG4Event class into a 
///    		    TRestHitsEvent, that is, we just "extract" the hits information
///             Date : oct/2016
///             Author : I. G. Irastorza
///
///_______________________________________________________________________________


#include "TRestG4toHitsProcess.h"
using namespace std;


ClassImp(TRestG4toHitsProcess)
    //______________________________________________________________________________
TRestG4toHitsProcess::TRestG4toHitsProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestG4toHitsProcess::TRestG4toHitsProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestG4toHitsProcess::~TRestG4toHitsProcess()
{
    delete fG4Event;
    delete fHitsEvent;
}

//______________________________________________________________________________
void TRestG4toHitsProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );

    cout << "G4 to hits metadata not found. Loading default values" << endl;
}

//______________________________________________________________________________
void TRestG4toHitsProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fG4Event = new TRestG4Event();
    fHitsEvent = new TRestHitsEvent();

    fOutputEvent = fHitsEvent;
    fInputEvent = fG4Event;
}

void TRestG4toHitsProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestG4toHitsProcess::InitProcess()
{
//    TRestEventProcess::ReadObservables();

//    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );
}

//______________________________________________________________________________
void TRestG4toHitsProcess::BeginOfEventProcess() 
{
    fHitsEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestG4toHitsProcess::ProcessEvent( TRestEvent *evInput )
{
    fG4Event = (TRestG4Event *) evInput;


    fHitsEvent->SetRunOrigin( fG4Event->GetRunOrigin() );
    fHitsEvent->SetSubRunOrigin( fG4Event->GetSubRunOrigin() );
    fHitsEvent->SetID( fG4Event->GetID() );
    fHitsEvent->SetSubID( fG4Event->GetSubID() );
    fHitsEvent->SetSubEventTag( fG4Event->GetSubEventTag() );
    fHitsEvent->SetTimeStamp( fG4Event->GetTimeStamp() );
    fHitsEvent->SetState( fG4Event->isOk() );

    Int_t i,j;
    Double_t x,y,z,E;
    
    for ( i = 0; i < fG4Event->GetNumberOfTracks(); i++ )
    {
        for ( j = 0; j < fG4Event->GetTrack(i)->GetNumberOfHits(); j++ )
        {
           // read x,y,z and E of every hit in the G4 event
            x = fG4Event->GetTrack(i)->GetHits()->fX[j];
            y = fG4Event->GetTrack(i)->GetHits()->fY[j];
            z = fG4Event->GetTrack(i)->GetHits()->fZ[j];
            E = fG4Event->GetTrack(i)->GetHits()->fEnergy[j];

            // and write them in the output hits event:
            fHitsEvent->AddHit (x, y, z, E);
        }
    }

    return fHitsEvent;
}

//______________________________________________________________________________
void TRestG4toHitsProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestG4toHitsProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestG4toHitsProcess::InitFromConfigFile( )
{

}

