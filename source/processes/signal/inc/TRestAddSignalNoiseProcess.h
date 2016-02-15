///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAddSignalNoiseProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestAddSignalNoiseProcess
#define RestCore_TRestAddSignalNoiseProcess

#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestAddSignalNoiseProcess:public TRestEventProcess {

    private:
        TRestSignalEvent *fInputSignalEvent;
        TRestSignalEvent *fOutputSignalEvent;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Double_t fNoiseLevel;

    protected:
        //add here the members of your event process

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            std::cout << "Noise Level : " << fNoiseLevel << std::endl;

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "addSignalNoiseProcess"; }

        //Constructor
        TRestAddSignalNoiseProcess();
        TRestAddSignalNoiseProcess( char *cfgFileName );
        //Destructor
        ~TRestAddSignalNoiseProcess();

        ClassDef(TRestAddSignalNoiseProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

