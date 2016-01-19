///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToSignalProcess.cxx
///
///             oct 2015:  Javier Galan
///_______________________________________________________________________________


#include "TRestHitsToSignalProcess.h"

#include <TRandom.h>

ClassImp(TRestHitsToSignalProcess)
    //______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess()
{
    Initialize();

}

//______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfig( "hitsToSignalProcess", cfgFileName ) == -1 ) LoadDefaultConfig( );

    fReadout = new TRestReadout( cfgFileName );

    // TRestHitsToSignalProcess default constructor
}

//______________________________________________________________________________
TRestHitsToSignalProcess::~TRestHitsToSignalProcess()
{
    if( fReadout != NULL ) delete fReadout;

    delete fHitsEvent;
    delete fSignalEvent;
    // TRestHitsToSignalProcess destructor
}

void TRestHitsToSignalProcess::LoadDefaultConfig( )
{
    SetName( "hitsToSignalProcess-Default" );
    SetTitle( "Default config" );


    fSampling = 1;

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::Initialize()
{

    fHitsEvent = new TRestHitsEvent();

    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fHitsEvent;
    fOutputEvent = fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    if( fReadout == NULL ) cout << "REST ERRORRRR : Readout has not been initialized" << endl;

    cout << __PRETTY_FUNCTION__ << endl;

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::BeginOfEventProcess() 
{
    cout << "Begin of event process" << endl;
    fSignalEvent->Initialize(); 

    cout << "Number of signals : " << fSignalEvent->GetNumberOfSignals() << endl;
}

Int_t TRestHitsToSignalProcess::FindModule( Double_t x, Double_t y )
{
    for ( int md = 0; md < fReadout->GetNumberOfModules(); md++ )
        if( fReadout->GetReadoutModule( md )->isInside( x, y ) ) return md;

    return -1;
}

Int_t TRestHitsToSignalProcess::FindChannel( Int_t module, Double_t x, Double_t y )
{
            return fReadout->GetReadoutModule( module )->FindChannel( x, y );
}


//______________________________________________________________________________
TRestEvent* TRestHitsToSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    fHitsEvent = (TRestHitsEvent *) evInput;

    /*
    cout << "Event ID : " << fHitsEvent->GetEventID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;
    cout << "--------------------------" << endl;
    */

    fSignalEvent->SetEventTime( fHitsEvent->GetEventTime() );
    fSignalEvent->SetEventID( fHitsEvent->GetEventID() );


    for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ )
    {
        Double_t x = fHitsEvent->GetX( hit );
        Double_t y = fHitsEvent->GetY( hit );

        Int_t mod = FindModule( x, y );
        if( mod >= 0 )
        {
            Int_t chnl = FindChannel ( mod, x, y );
            if( chnl >= 0 )
            {
                Int_t channelID = 0;
                for( int n = 0; n < mod-1; n++ )
                    channelID += fReadout->GetReadoutModule(n)->GetNumberOfChannels();
                channelID += chnl;

                //cout << "Hit found in channel : " << chnl << " chID : " << channelID << endl;

                /* This is now done internally in TRestSignalEvent
                   if ( !fSignalEvent->signalIDExists( channelID ) )
                   {
                   TRestSignal sgnl;
                   sgnl.SetSignalID( channelID );
                   fSignalEvent->AddSignal( sgnl );
                   }
                   */

                Double_t energy = fHitsEvent->GetEnergy( hit );
                Double_t time = fHitsEvent->GetZ( hit );

                time = fSampling * (Int_t) (time/fSampling);

                fSignalEvent->AddChargeToSignal( channelID, time, energy );
            }
        }
    }

    fSignalEvent->SortSignals();

    fSignalEvent->PrintEvent();


    return fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitFromConfigFile( )
{
    cout << __PRETTY_FUNCTION__ << " --> TOBE implemented" << endl;

}
