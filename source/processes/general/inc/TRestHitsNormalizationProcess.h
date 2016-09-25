///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsNormalizationProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________


#ifndef RestCore_TRestHitsNormalizationProcess
#define RestCore_TRestHitsNormalizationProcess

#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestHitsNormalizationProcess:public TRestEventProcess {
    private:

#ifndef __CINT__

        TRestHitsEvent *fHitsInputEvent;
        TRestHitsEvent *fHitsOutputEvent;

#endif

        void InitFromConfigFile();
        void Initialize();
        void LoadDefaultConfig();

    protected:
        //add here the members of your event process

        Double_t fFactor;


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            std::cout << " Renormalization factor : " << fFactor << std::endl;

            EndPrintProcess();

        }

        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "hitsNormalizationProcess"; }

        //Constructor
        TRestHitsNormalizationProcess();
        TRestHitsNormalizationProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsNormalizationProcess();

        ClassDef(TRestHitsNormalizationProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

