//////////////////////////////////////////////////////////////////////////
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackLinearizationProcess.h
///
///              Jan 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////


#ifndef RestCore_TRestTrackLinearizationProcess
#define RestCore_TRestTrackLinearizationProcess

#include <TRestTrackEvent.h>
#include <TRestLinearTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackLinearizationProcess:public TRestEventProcess {

    private:
        Double_t fLengthResolution;
        Double_t fTransversalResolution;

        TString fMethod;
        Double_t fRadius;

        Int_t fDivisions;

#ifndef __CINT__
        TRestTrackEvent *fInputTrackEvent;
        TRestLinearTrackEvent *fOutputLinearTrackEvent;
#endif

        void InitFromConfigFile();

        void Initialize();

        TVector2 FindProjection( TVector3 position, TRestHits *nodes );
        Double_t GetReferenceNode( TRestHits *nHits, Double_t dist, Int_t &ref );


    protected:


    public:

        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() 
        { 
            BeginPrintProcess();
            std::cout << "Lenght resolution : " << fLengthResolution << " mm" << std::endl;
            std::cout << "Transversal resolution : " << fTransversalResolution << " mm" << std::endl;
            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "trackLinearization"; }

        //Constructor
        TRestTrackLinearizationProcess();
        TRestTrackLinearizationProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackLinearizationProcess();

        ClassDef( TRestTrackLinearizationProcess, 1);
};
#endif

