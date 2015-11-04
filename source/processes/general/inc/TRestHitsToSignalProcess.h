///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToSignalProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestHitsToSignalProcess
#define RestCore_TRestHitsToSignalProcess

#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestHitsToSignalProcess:public TRestEventProcess {
    private:
        TRestHitsEvent *fHitsEvent;
        TRestSignalEvent *fSignalEvent;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Int_t fSampling; // us

        Int_t FindModuleAndChannel( Double_t x, Double_t y, Int_t &module, Int_t &channel );

    protected:
        //add here the members of your event process
        TRestReadout *fReadout;

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void PrintMetadata() { cout << "TODO : Needs to be implemented" << endl; }

        TRestMetadata *GetMetadata( ) { return fReadout; }

        TString GetProcessName() { return (TString) "chargeSegmentation"; }

        //Constructor
        TRestHitsToSignalProcess();
        TRestHitsToSignalProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsToSignalProcess();

        ClassDef(TRestHitsToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

