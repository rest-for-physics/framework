//////////////////////////////////////////////////////////////////////////
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsShuffleProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////


#ifndef RestCore_TRestHitsShuffleProcess
#define RestCore_TRestHitsShuffleProcess

#include <TRestHitsEvent.h>
#include "TRestEventProcess.h"

#include <TRandom3.h>

class TRestHitsShuffleProcess:public TRestEventProcess {

    private:

#ifndef __CINT__
        TRestHitsEvent *fInputHitsEvent;//!
        TRestHitsEvent *fOutputHitsEvent;//!

        TRandom3 *fRandom;//!
#endif

        void InitFromConfigFile();

        void Initialize();

    protected:

        Int_t fIterations;

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            std::cout << " Iterations : " << fIterations << std::endl;

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "hitsShuffle"; }

        //Constructor
        TRestHitsShuffleProcess();
        TRestHitsShuffleProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsShuffleProcess();

        ClassDef(TRestHitsShuffleProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

