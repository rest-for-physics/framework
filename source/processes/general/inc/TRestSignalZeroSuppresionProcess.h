///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalZeroSuppresionProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalZeroSuppresionProcess
#define RestCore_TRestSignalZeroSuppresionProcess

#include <TRestGas.h>
#include <TRestReadout.h>

#include <TRestSignalEvent.h>
#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalZeroSuppresionProcess:public TRestEventProcess {
    private:

#ifndef __CINT__
        TRestRawSignalEvent *fRawSignalEvent;
        TRestSignalEvent *fSignalEvent;

#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:

        TVector2 fBaseLineRange;
        Double_t fPointThreshold;
        Double_t fSignalThreshold;
        Int_t fNPointsOverThreshold;


    public:

        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() 
        {
            BeginPrintProcess();

            std::cout << "Base line range definition : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) " << std::endl;
            cout << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
            cout << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
            cout << "Number of points over threshold : " << fNPointsOverThreshold << endl;

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "signalToHits"; }

        //Constructor
        TRestSignalZeroSuppresionProcess();
        TRestSignalZeroSuppresionProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalZeroSuppresionProcess();

        ClassDef(TRestSignalZeroSuppresionProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

