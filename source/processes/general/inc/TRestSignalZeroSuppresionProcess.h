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
        TRestRawSignalEvent *fRawSignalEvent;//!
        TRestSignalEvent *fSignalEvent;//!

#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:

        TVector2 fBaseLineRange;
		TVector2 fIntegralRange;
        Double_t fPointThreshold;
        Double_t fSignalThreshold;
		Int_t fNPointsOverThreshold;
		Int_t fNPointsFlatThreshold;
		bool fBaseLineCorrection;
		Double_t fSampling; // us


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

            essential << "Base line range definition : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) " << endl;
			essential << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) " << endl;
			essential << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
            essential << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
            essential << "Number of points over threshold : " << fNPointsOverThreshold << endl;
			essential << "Max Number of points of flat signal tail : " << fNPointsFlatThreshold << endl;

			if(fBaseLineCorrection)
				essential << "BaseLine correction is enabled for TRestRawSignalAnalysisProcess"<< endl;

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "signalZeroSuppresion"; }

        //Constructor
        TRestSignalZeroSuppresionProcess();
        TRestSignalZeroSuppresionProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalZeroSuppresionProcess();

        ClassDef(TRestSignalZeroSuppresionProcess, 2);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

