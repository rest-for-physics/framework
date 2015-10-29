///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalEvent.h
///
///             Event class to store DAQ events either from simulation and acquisition 
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#ifndef RestDAQ_TRestSignalEvent
#define RestDAQ_TRestSignalEvent

#include <iostream>
using namespace std;

#include <TObject.h>
#include <TArrayD.h>
#include <TPad.h>

#include "TRestEvent.h"
#include "TRestSignal.h"

class TRestSignalEvent: public TRestEvent {

    protected:
        
        vector <TRestSignal> fSignal; //Collection of signals that define the event
        
    
    public:

        //Setters
        void AddSignal(TRestSignal s){fSignal.push_back(s);}
        
        //Getters
        Int_t GetNumberOfSignals(){return fSignal.size();}
        TRestSignal *GetSignal(Int_t n ){return &fSignal[n];}
        
        void Initialize();

        void PrintEvent();
        
        TPad *DrawEvent();
        
        //Construtor
        TRestSignalEvent();
        //Destructor
        virtual ~ TRestSignalEvent();

        ClassDef(TRestSignalEvent, 1);     // REST event superclass
};
#endif
