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

        // TODO: In future TRestManager might handle several input files.
        // Still is to be defined how this will be done
        TRestRun *fRun;

        Int_t fFirstEntry;
        Int_t fLastEntry;
        Int_t fNEventsToProcess;

        TString fInputFile;

        std::vector <TString> fProcessType;
        std::vector <TString> fProcessName;
        std::vector <TString> fPcsConfigFile;


        void AddReadout( string readoutDefinition );
        void AddGas( string gasDefinition );

    protected:

        void InitFromConfigFile();

        virtual void Initialize();

        void LoadProcesses();

        virtual void LoadExternalProcess( TString processType, std::string processesCfgFile, std::string processName ) {  }

        void AddProcess( TRestEventProcess *evPcs, string cfgFile, string pcsName ) { fRun->AddProcess( evPcs, cfgFile, pcsName ); }

        void AddMetadata( TRestMetadata *meta ) { fRun->AddMetadata( meta ); }


    public:

        void ProcessEvents( ) 
        { 
            LoadProcesses();

            if( fRun != NULL ) 
                fRun->ProcessEvents( fFirstEntry, fNEventsToProcess, fLastEntry ); 
        }

        void PrintMetadata( );

        //Construtor
        TRestManager();
        TRestManager( const char *cfgFileName, const char *name = "" );
        //Destructor
        virtual ~ TRestManager();


        ClassDef(TRestManager, 1);     // REST readout class
};
#endif
