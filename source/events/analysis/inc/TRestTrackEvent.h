///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________


#ifndef RestDAQ_TRestSignalEvent
#define RestDAQ_TRestSignalEvent

#include <iostream>

#include <TObject.h>
#include <TArrayD.h>
#include <TH2I.h>
#include <TVirtualPad.h>
#include <TGraph.h>

#include "TRestEvent.h"
#include "TRestSignal.h"
#include "TRestTrack.h"

class TRestTrackEvent: public TRestEvent {

    protected:
 
        Int_t fNtracks;       
        Int_t fNtracksX;
        Int_t fNtracksY;
        std::vector <TRestTrack> fTrack; //Collection of tracks that define the event
        Int_t fLevels;

        #ifndef __CINT__
        TGraph *fXYHit;
        TGraph *fXZHit;
        TGraph *fYZHit;
        TGraph *fXYTrack;
        TGraph *fXZTrack;
        TGraph *fYZTrack;
        TPad *fPad; 
        #endif 

    public:

        TRestTrack *GetTrack( Int_t n )  { return &fTrack[n]; }

        TRestTrack *GetTrackById( Int_t id )
        {
            for( int i = 0; i < GetNumberOfTracks(); i++ )
                if( GetTrack( i )->GetTrackID() == id ) return GetTrack( i );
            return NULL;
        }

        Int_t GetLevel( Int_t tck );
        void SetLevels();
        Int_t GetLevels( ) { return fLevels; }

        TPad *DrawEvent();

        //Setters
        void AddTrack( TRestTrack *c ){ fTrack.push_back(*c); fNtracks++; SetLevels(); }
        void RemoveTrack(int n){fTrack.erase(fTrack.begin()+n); fNtracks--; SetLevels(); }  
        void RemoveTrack( ){fTrack.clear();}  

        Bool_t isTopLevel( Int_t tck ) 
        {
            if( GetLevels() == GetLevel( tck ) )
                return true;
            return false; 
        }

        void SetNumberOfXTracks( Int_t x ) { fNtracksX = x; }
        void SetNumberOfYTracks( Int_t y ) { fNtracksY = y; }

        //Getters
        Int_t GetNumberOfTracks() { return fNtracks; }
        Int_t GetNumberOfXTracks() { return fNtracksX; }
        Int_t GetNumberOfYTracks() { return fNtracksY; }

        Int_t GetTotalHits( );
      
        void Initialize();

        void PrintOnlyTracks();
        void PrintEvent();
                
        //Construtor
        TRestTrackEvent();
        //Destructor
        virtual ~ TRestTrackEvent();

        ClassDef(TRestTrackEvent, 1);     // REST event superclass
};
#endif
