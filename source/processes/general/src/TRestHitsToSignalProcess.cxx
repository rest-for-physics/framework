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

Int_t TRestHitsToSignalProcess::FindModuleAndChannel( Double_t x, Double_t y, Int_t &module, Int_t &channel )
{
    for ( int md = 0; md < fReadout->GetNumberOfModules(); md++ )
    {
        if( fReadout->GetReadoutModule( md )->isInside( x, y ) )
            for( int ch = 0; ch < fReadout->GetReadoutModule( md )->GetNumberOfChannels(); ch++ )
            {
                if ( fReadout->GetReadoutModule( md )->isInsideChannel( ch, x , y ) )
                {
                    module = md;
                    channel = ch;
                    return 1;
                }
            }
    }
    return 0;

}

//______________________________________________________________________________
TRestEvent* TRestHitsToSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    fHitsEvent = (TRestHitsEvent *) evInput;

    cout << "Event ID : " << fHitsEvent->GetEventID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;

    fSignalEvent->SetEventTime( fHitsEvent->GetEventTime() );
    fSignalEvent->SetEventID( fHitsEvent->GetEventID() );

    Int_t mod = -1;
    Int_t chnl = -1;

    for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ )
    {
        Double_t x = fHitsEvent->GetX( hit );
        Double_t y = fHitsEvent->GetY( hit );

        if( FindModuleAndChannel( x, y, mod, chnl ) )
        {
            Int_t channelID = 0;
            for( int n = 0; n < mod-1; n++ )
                channelID += fReadout->GetReadoutModule(n)->GetNumberOfChannels();
            channelID += chnl;

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

    fSignalEvent->SortSignals();

 //   fSignalEvent->PrintEvent();


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
