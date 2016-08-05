///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPointLikeTrackAnaProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestPointLikeTrackAnaProcess
#define RestCore_TRestPointLikeTrackAnaProcess

#include <TRestTrackEvent.h>

#include "TRestEventProcess.h"

class TRestPointLikeTrackAnaProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestTrackEvent *fTrackEvent;
        // TODO We must get here a pointer to TRestDaqMetadata
        // In order to convert the parameters to time using the sampling time
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        
        //add here the members of your event process

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() { 

            BeginPrintProcess();

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "pointLikeTrackAna"; }
 //       Double_t GetCalibFactor();           ///< Calibration factor is found.

        //Constructor
        TRestPointLikeTrackAnaProcess();
        TRestPointLikeTrackAnaProcess( char *cfgFileName );
        //Destructor
        ~TRestPointLikeTrackAnaProcess();

        ClassDef(TRestPointLikeTrackAnaProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

