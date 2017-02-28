//////////////////////////////////////////////////////////////////////////
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackPathMinimizationProcess.h
///
///              Jan 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////


#ifndef RestCore_TRestTrackPathMinimizationProcess
#define RestCore_TRestTrackPathMinimizationProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

#ifndef __CINT__
#include "trackMinimization.h"
#endif

class TRestTrackPathMinimizationProcess:public TRestEventProcess {

    private:

#ifndef __CINT__
        TRestTrackEvent *fInputTrackEvent;
        TRestTrackEvent *fOutputTrackEvent;
#endif

        void InitFromConfigFile();

        void Initialize();


    protected:

        Int_t fMaxNodes;


    public:

        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() 
        { 
            BeginPrintProcess();
            cout << "Maximum number of nodes (hits) allowed : " << fMaxNodes << endl;
            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "trackPathMinimization"; }

        //Constructor
        TRestTrackPathMinimizationProcess();
        TRestTrackPathMinimizationProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackPathMinimizationProcess();

        ClassDef( TRestTrackPathMinimizationProcess, 1);
};
#endif

