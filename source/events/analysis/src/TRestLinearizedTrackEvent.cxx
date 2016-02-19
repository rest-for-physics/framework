///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestLinearizedTrackEvent.h
///
///             Event class to store linearized track events
///
///             Feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///
///_______________________________________________________________________________


#include "TRestLinearizedTrackEvent.h"
using namespace std;

ClassImp(TRestLinearizedTrackEvent)
//______________________________________________________________________________
    TRestLinearizedTrackEvent::TRestLinearizedTrackEvent()
{
   // TRestLinearizedTrackEvent default constructor
   Initialize();
}

//______________________________________________________________________________
TRestLinearizedTrackEvent::~TRestLinearizedTrackEvent()
{
   // TRestLinearizedTrackEvent destructor
}

void TRestLinearizedTrackEvent::Initialize()
{
    TRestEvent::Initialize();
    fEventClassName = "TRestLinearizedTrackEvent";

    fPad = NULL;

    fLinearTrack.clear();
    fNTracks = 0;

    fMinLength = 1e10;
    fMaxLength = -1e10;
    fMinWidth = 1e10;
    fMaxWidth = -1e10;

    fMinLinearDeposit = 1e10;
    fMaxLinearDeposit = -1e10;
    fMinTransversalDeposit = 1e10;
    fMaxTransversalDeposit = -1e10;
}


void TRestLinearizedTrackEvent::PrintEvent( Bool_t fullInfo )
{
    TRestEvent::PrintEvent();

    for( int tck = 0; tck < GetNumberOfTracks(); tck++ )
    {
        cout << "+++++++++ Linear track : " << tck << " ++++++" << endl;
        fLinearTrack[tck].Print( fullInfo );
    }

}

//Draw current event in a Tpad
TPad *TRestLinearizedTrackEvent::DrawEvent()
{
    if( fPad != NULL ) { delete fPad; fPad=NULL;}

    int nTracks = this->GetNumberOfTracks();

    if( nTracks == 0 )
    {
        cout << "Empty event " << endl;
        return NULL;
    }

    this->PrintEvent( false );

    fPad = new TPad( this->GetClassName().Data(), " ", 0, 0, 1, 1 );
    fPad->Divide( 2 , 2 );
    fPad->cd();
    fPad->cd(1)->DrawFrame( GetMinLinearLength()-10, 0,
            GetMaxLinearLength()+10, 1.05*GetMaxLinearDeposit() );
    fPad->cd(2)->DrawFrame( -10, 0,
            1.05*GetMaxTransversalLength(), 1.05*GetMaxTransversalDeposit() );
    fPad->cd(3)->DrawFrame( GetMinLinearLength()-10, 0,
            GetMaxLinearLength()+10, 1.05*GetMaxLinearDeposit() );
    fPad->cd(4)->DrawFrame( -1, 0,
            1.05*GetMaxTransversalLength(), 1.05*GetMaxTransversalDeposit() );
    fPad->Draw();

    for( int tck = 0; tck < nTracks; tck++ )
    {
        TGraph *linGr = fLinearTrack[tck].GetLinearGraph( tck + 2 );
        TGraph *trGr = fLinearTrack[tck].GetTransversalGraph( tck + 2 );

        if( fLinearTrack[tck].isXZ() )
            fPad->cd(1);
        else
            fPad->cd(3);

        linGr->Draw( "LP" );

        if( fLinearTrack[tck].isXZ() )
            fPad->cd(2);
        else
            fPad->cd(4);

        trGr->Draw( "LP" );
        fPad->Update();
    }

    fPad->Draw();

    return fPad;
}

