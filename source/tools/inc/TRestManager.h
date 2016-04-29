///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestManager.h
///
///             apr 2016    Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestManager
#define RestCore_TRestManager

#include <iostream>

#include "TObject.h"

#include <TRestRun.h>
#include <TRestMetadata.h>

class TRestManager:public TRestMetadata {
    private:
        void InitFromConfigFile();

        virtual void Initialize();

        // TODO: In future TRestManager might handle several input files.
        // Still is to be defined how this will be done
        TRestRun *fRun;

        Int_t fFirstEntry;
        Int_t fNEventsToProcess;

        void AddReadout( string readoutDefinition );
        void AddGas( string gasDefinition );

    protected:

    public:

        void ProcessEvents( ) 
        { 
            if( fRun != NULL ) 
                fRun->ProcessEvents( fFirstEntry, fNEventsToProcess ); 
        }

        void Draw();

        void PrintMetadata( );

        //Construtor
        TRestManager();
        TRestManager( const char *cfgFileName);
        //Destructor
        virtual ~ TRestManager();


        ClassDef(TRestManager, 1);     // REST readout class
};
#endif
