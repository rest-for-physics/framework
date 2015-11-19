
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
using namespace std;

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
 
        Int_t nTracks;       
        vector <TRestTrack> fTrack; //Collection of tracks that define the event

        #ifndef __CINT__
	TGraph *fGr;
	TPad *fPad;  
	#endif
    public:

        TRestTrack *GetTrack( int n)  { return &fTrack[n]; }

        TPad *DrawEvent();

        //Setters
        void AddTrack(TRestTrack c){fTrack.push_back(c); nTracks++;}
        void RemoveTrack(int n){fTrack.erase(fTrack.begin()+n); nTracks--;}  
        void RemoveTrack( ){fTrack.clear();}  
        //Getters
        Int_t GetNumberOfTracks(){return nTracks;}
      
        void Initialize();

        void PrintEvent();
                
        //Construtor
        TRestTrackEvent();
        //Destructor
        virtual ~ TRestTrackEvent();

        ClassDef(TRestTrackEvent, 1);     // REST event superclass
};
#endif
