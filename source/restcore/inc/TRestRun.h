///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRun.h
///
///             Base class from which to inherit all other event classes in REST 
///
///             apr 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestRun
#define RestCore_TRestRun

#include <iostream>

#include "TObject.h"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include "TString.h"
#include <TKey.h>
#include <TGeoManager.h>

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestEventProcess.h"

class TRestRun:public TRestMetadata {
    private:
        void InitFromConfigFile();
        void SetVersion();


        virtual void Initialize();

    protected:
        Int_t fRunNumber;                 //< first identificative number
        Int_t fRunIndex;
        TString fRunClassName;
        TString fRunType;             //< Stores bit by bit the type of run. 0: Calibration 1: Background 2: Pedestal 3: Simulation 4: DataTaking 
        TString fRunUser;	          //< To identify the author it has created the run. It might be also a word describing the origin of the run (I.e. REST_Prototype, T-REX, etc)
        TString fRunTag;            //< A tag to be written to the output file
        TString fRunDescription;	  //< A word or sentence describing the run (I.e. Fe55 calibration, cosmics, etc)
        TString fExperimentName;
        TString fOutputFilename;
        TString fInputFilename;
        TString fVersion;
        Bool_t fOverwrite;
        
        Int_t fRunEvents;

        Double_t fStartTime;              ///< Event absolute starting time/date (unix timestamp)
        Double_t fEndTime;              ///< Event absolute starting time/date (unix timestamp)

        std::vector <TRestMetadata*> fMetadata;
        std::vector <TRestEventProcess*> fEventProcess;
        std::vector <TRestMetadata*> fHistoricMetadata;  // Open input file should store the metadata (and historic) information in historic metadata
        std::vector <TRestEventProcess*> fHistoricEventProcess;

#ifndef __CINT__
        TTree *fInputEventTree;
        TTree *fOutputEventTree;

        TRestEvent *fInputEvent;
        TRestEvent *fOutputEvent;
        
        TFile *fInputFile;
        TFile *fOutputFile;
        
        Int_t fCurrentEvent;
        Int_t fProcessedEvents;
#endif

        TGeoManager *fGeometry;

        void SetRunFilenameAndIndex();
        TKey *GetObjectKeyByClass( TString className );
        TKey *GetObjectKeyByName( TString name );

    public:
        
        void Start(  );
        void ProcessEvents( Int_t firstEvent = 0, Int_t eventsToProcess = 0 );
        
        Int_t GetNumberOfProcesses() { return fEventProcess.size(); }

        // File input/output
        void OpenOutputFile( );
        void CloseOutputFile( );

        void OpenInputFile( TString fName );
        void OpenInputFile( TString fName, TString cName );

        TRestEvent *GetEventInput() { return fInputEvent; }
        TTree *GetInputEventTree() { return fInputEventTree; }

        //TRestMetadata *GetEventMetadata() { return fEventMetadata; }
        TRestEvent *GetOutputEvent() { return fOutputEvent; }
        TFile *GetOutputFile() { return fOutputFile; }
        TTree *GetOutputEventTree() { return fOutputEventTree; }

        //Getters
        TString GetVersion() { return  fVersion; }
        Int_t GetRunNumber() { return fRunNumber; }
        Int_t GetRunIndex() { return fRunIndex; }
        TString GetRunType() { return fRunType; }
        TString GetRunUser() { return fRunUser; }
        TString GetRunTag() { return fRunTag; }
        TString GetRunDescription() { return fRunDescription; }
        Int_t GetNumberOfEvents() { return fRunEvents; }
        Double_t GetStartTimestamp() { return fStartTime; }
        Double_t GetEndTimestamp() { return fEndTime; }
        TString GetExperimentName() { return fExperimentName; }
        TGeoManager *GetGeometry() { return fGeometry; }

        TRestMetadata *GetHistoricMetadata(unsigned int index)
        {
            if( index < fHistoricMetadata.size() ) return fHistoricMetadata[index];
            else return NULL;
        }

        TRestMetadata *GetMetadata( TString name );


        void SetRunNumber( Int_t number ) { fRunNumber = number; }
        void SetRunIndex ( Int_t index ) { fRunIndex = index; }
        void SetRunType( TString type ) { fRunType = type; }
        void SetRunTag( TString tag ) { fRunTag = tag; }
        void SetRunUser( TString user ) { fRunUser = user; } 
        void SetRunDescription( TString description ) { fRunDescription = description; }
        void SetNumberOfEvents( Int_t nEvents ) { fRunEvents = nEvents; } 
        void SetEndTimeStamp( Double_t tStamp ) { fEndTime = tStamp; }

        void SetGeometry( TGeoManager *g ) { fGeometry = g; }
        void SetInputFileName( TString fN){fInputFilename=fN;}

        TString GetDateFormatted( Double_t runTime );
        TString GetDateForFilename( Double_t runTime );
        TString GetTime( Double_t runTime );

        void ResetRunTimes();
	
        Bool_t isClass(TString className);
	
	
        //Setters

        void AddMetadata( TRestMetadata *metadata ) { fMetadata.push_back( metadata ); }
        void AddHistoricMetadata( TRestMetadata *metadata ) { fHistoricMetadata.push_back( metadata ); }
        void AddProcess( TRestEventProcess *process, std::string cfgFilename );

        virtual void SetOutputEvent( TRestEvent *evt );
        virtual void SetInputEvent( TRestEvent *evt );
	
        Bool_t GetNextEvent( );
	
        // Printers
        void PrintStartDate();
        void PrintEndDate();

        void PrintInfo( );
        void PrintMetadata() { PrintInfo(); }

        void PrintAllMetadata()
        {
            this->PrintMetadata();
            for( unsigned int i = 0; i < fMetadata.size(); i++ )
                fMetadata[i]->PrintMetadata();
            for( unsigned int i = 0; i < fEventProcess.size(); i++ )
                fEventProcess[i]->PrintMetadata();
            for( unsigned int i = 0; i < fHistoricMetadata.size(); i++ )
                fHistoricMetadata[i]->PrintMetadata();
            for( unsigned int i = 0; i < fHistoricEventProcess.size(); i++ )
                fHistoricEventProcess[i]->PrintMetadata();
        }
        
        void PrintProcessedEvents( Int_t rateE);

        Double_t GetRunLength();

        //Construtor
        TRestRun();
        TRestRun( char *cfgFileName);
        //Destructor
        virtual ~ TRestRun();


        ClassDef(TRestRun, 1);     // REST run class
};
#endif
