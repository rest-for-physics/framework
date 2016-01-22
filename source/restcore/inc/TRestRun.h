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
using namespace std;

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
        
        Int_t fRunEvents;

        Double_t fStartTime;              ///< Event absolute starting time/date (unix timestamp)
        Double_t fEndTime;              ///< Event absolute starting time/date (unix timestamp)

        vector <TRestMetadata*> fMetadata;
        vector <TRestEventProcess*> fEventProcess;
        vector <TRestMetadata*> fHistoricMetadata;  // Open input file should store the metadata (and historic) information in historic metadata
        vector <TRestEventProcess*> fHistoricEventProcess;

#ifndef __CINT__
        TTree *fInputEventTree;
        TTree *fOutputEventTree;

        TRestEvent *fInputEvent;
        TRestEvent *fOutputEvent;
        
        TFile *fInputFile;
        TFile *fOutputFile;
        
        Int_t fCurrentEvent;
#endif

        TGeoManager *fGeometry;

        void SetRunFilenameAndIndex();

    public:
        
        void Start();
        void ProcessAll();
        
        Int_t GetNumberOfProcesses()
        {
            return fEventProcess.size();

        }

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
        TRestMetadata *GetHistoricMetadata(unsigned int index){
        if(index<fHistoricMetadata.size())return fHistoricMetadata[index];
        else return NULL;
        }


        void SetRunNumber( Int_t number ) { fRunNumber = number; }
        void SetRunIndex ( Int_t index ) { fRunIndex = index; }
        void SetRunType( TString type ) { fRunType = type; }
        void SetRunTag( TString tag ) { fRunTag = tag; }
        void SetRunUser( TString user ) { fRunUser = user; } 
        void SetRunDescription( TString description ) { fRunDescription = description; }
        void SetNumberOfEvents( Int_t nEvents ) { fRunEvents = nEvents; } 
        void SetEndTimeStamp( Double_t tStamp ) { fEndTime = tStamp; }

        void SetGeometry( TGeoManager *g ) { cout << "AA" << endl; fGeometry = g; cout << "fGeo ::" << fGeometry << endl; } // fGeometry->SetName( "GDML_Geometry"); cout << "CC" << endl; }
        void SetInputFileName( TString fN){fInputFilename=fN;}
        
        

        TString GetDateFormatted( Double_t runTime );
        TString GetDateForFilename( Double_t runTime );
        TString GetTime( Double_t runTime );

        void ResetRunTimes();
	
        Bool_t isClass(TString className);
	
	
        //Setters

        void AddMetadata( TRestMetadata *metadata ) { fMetadata.push_back( metadata ); }
        void AddHistoricMetadata( TRestMetadata *metadata ) { fHistoricMetadata.push_back( metadata ); }
        void AddProcess( TRestEventProcess *process, string cfgFilename ) 
        {

            // We give a pointer to the metadata stored in TRestRun to the processes. This metadata will be destroyed afterwards
            // it is not intended for storage, just for the processes so that they are aware of all metadata information.
            // Each proccess is responsible to implement GetProcessMetadata so that TRestRun stores this metadata.

            vector <TRestMetadata*> metadata;
            for( size_t i = 0; i < fMetadata.size(); i++ )
                metadata.push_back( fMetadata[i] );
            for( size_t i = 0; i < fHistoricMetadata.size(); i++ )
                metadata.push_back( fHistoricMetadata[i] );
            for( size_t i = 0; i < fEventProcess.size(); i++ )
                metadata.push_back( fEventProcess[i] );
            for( size_t i = 0; i < fHistoricEventProcess.size(); i++ )
                metadata.push_back( fHistoricEventProcess[i] );

            process->SetMetadata( metadata );

            cout << "Metadata given to process : " << process->GetName() << endl;
            cout << "------------------------------------------------------" << endl;
            for( size_t i = 0; i < metadata.size(); i++ )
                cout << metadata[i]->ClassName() << endl;
            cout << "---------------------------" << endl;

            process->LoadConfig( cfgFilename );

            //process->LoadConfigFromFile( cfgFilename );
            // Each proccess is responsible to implement GetMetadata so that TRestRun stores this metadata.

            TRestMetadata *meta = process->GetProcessMetadata();
            if( meta != NULL ) this->AddMetadata( meta );

            process->PrintMetadata( );

            fEventProcess.push_back( process ); 

        }

        virtual void SetOutputEvent( TRestEvent *evt ) 
        { 
            fOutputEvent = evt;
            TString treeName = (TString) evt->GetName() + " Tree";
            fOutputEventTree->SetName( treeName );
            fOutputEventTree->Branch("eventBranch", evt->GetClassName(), fOutputEvent);
        }

        virtual void SetInputEvent( TRestEvent *evt ) 
        { 
            fInputEvent = evt;
	    
	    if(evt==NULL)return;
	    
            TString treeName = (TString) evt->GetName() + " Tree";
            fInputEventTree = (TTree * ) fInputFile->Get( treeName );

            TBranch *br = fInputEventTree->GetBranch( "eventBranch" );

            br->SetAddress( &fInputEvent );
            
        }

	
	Bool_t GetNextEvent( );
	
        // Printers
        void PrintStartDate();
        void PrintEndDate();

        void PrintInfo( );
        void PrintMetadata() { PrintInfo(); }
        
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
