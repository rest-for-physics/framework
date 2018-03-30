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

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestEventProcess.h"

#include "TRestAnalysisTree.h"

class TRestRun:public TRestMetadata {
    private:
        void InitFromConfigFile();
        void SetVersion();

        virtual void Initialize();

        TTree *GetOutputEventTree() { return fOutputEventTree; }
        TTree *GetInputEventTree() { return fInputEventTree; }

    protected:
        Int_t fRunNumber;                 //< first identificative number
        Int_t fParentRunNumber;
        TString fRunClassName;
        TString fRunType;             //< Stores bit by bit the type of run. 0: Calibration 1: Background 2: Pedestal 3: Simulation 4: DataTaking 
        TString fRunUser;	          //< To identify the author it has created the run. It might be also a word describing the origin of the run (I.e. REST_Prototype, T-REX, etc)
        TString fRunTag;            //< A tag to be written to the output file
        TString fRunDescription;	  //< A word or sentence describing the run (I.e. Fe55 calibration, cosmics, etc)
        TString fExperimentName;
        TString fOutputFilename;
        TString fInputFilename;
        TString fVersion;

        Double_t fStartTime;            ///< Event absolute starting time/date (unix timestamp)
        Double_t fEndTime;              ///< Event absolute starting time/date (unix timestamp)

        Bool_t fPureAnalysisOutput;
        Bool_t fContainsEventTree;

#ifndef __CINT__

        std::vector <TRestMetadata*> fMetadata;
        std::vector <TRestEventProcess*> fEventProcess;
        std::vector <TRestMetadata*> fHistoricMetadata;  // Open input file should store the metadata (and historic) information in historic metadata
        std::vector <TRestEventProcess*> fHistoricEventProcess;

        Bool_t fSkipEventTree;
        Bool_t fOverwrite;

        Int_t tmpInputRunNumber;
        TString tmpOutputFileName;
        TString tmpOutputDataPath;
        TString tmpInputFileName;
        TString tmpRunType;
        TString tmpRunTag;
        TString tmpExperimentName;

        TTree *fInputEventTree;
        TTree *fOutputEventTree;

        TRestAnalysisTree *fInputAnalysisTree;
        TRestAnalysisTree *fOutputAnalysisTree;

        TRestEvent *fInputEvent;
        TRestEvent *fOutputEvent;
        
        TFile *fInputFile;
        TFile *fOutputFile;
        
        Int_t fCurrentEvent;

	Bool_t fReadOnly;
#endif

        Int_t fProcessedEvents;
        vector <Int_t> fEventIDs;
        vector <Int_t> fSubEventIDs;
        vector <TString> fSubEventTags;
        vector <TString> fSubEventTagList;

        void SetRunFilenameAndIndex();

        TKey *GetObjectKeyByClass( TString className );
        TKey *GetObjectKeyByName( TString name );

    public:
        
        void Start(  );
        void ProcessEvents( Int_t firstEvent = 0, Int_t eventsToProcess = 0, Int_t lastEvent = 0 );
        
        Int_t GetNumberOfProcesses() { return fEventProcess.size(); }
        Int_t GetNumberOfMetadataStructures() { return fMetadata.size(); }
        
        Int_t GetNumberOfHistoricProcesses() { return fHistoricEventProcess.size(); }
        Int_t GetNumberOfHistoricMetadataStructures() { return fHistoricMetadata.size(); }

        std::vector <std::string> GetMetadataStructureNames( );
        std::vector <std::string> GetMetadataStructureTitles( );

        std::vector <std::string> GetProcessNames( );
        std::vector <std::string> GetProcessTitles( );

        // File input/output
        void OpenOutputFile( );
        void CloseOutputFile( );

        TFile *GetInputFile( ) { return fInputFile; }

        void EnableOverWrite( ) { fOverwrite = true; }
        void DisableOverWrite( ) { fOverwrite = false; }

        void OpenInputFile( TString fName );
        void OpenInputFile( TString fName, TString cName );

        TRestEvent *GetEventInput() { return fInputEvent; }

        Int_t GetEventWithID( Int_t eventID, Int_t subEventID = -1 );
        Int_t GetEventWithID( Int_t eventID, TString tag );


        // Quick analysis tree access
        Int_t GetObservableID( TString name ) { return fInputAnalysisTree->GetObservableID( name ); }
        Bool_t ObservableExists( TString name ) { return fInputAnalysisTree->ObservableExists( name ); }

        //TRestMetadata *GetEventMetadata() { return fEventMetadata; }
        TRestEvent *GetOutputEvent() { return fOutputEvent; }
        TFile *GetOutputFile() { return fOutputFile; }
        TString GetOutputFilename() { return fOutputFilename; }
        TString GetInputFilename( ) { return fInputFilename; }

        //Getters
        TString GetVersion() { return  fVersion; }
        Int_t GetParentRunNumber() { return fParentRunNumber; }
        Int_t GetRunNumber() { return fRunNumber; }
        TString GetRunType() { return fRunType; }
        TString GetRunUser() { return fRunUser; }
        TString GetRunTag() { return fRunTag; }
        TString GetRunDescription() { return fRunDescription; }
        Double_t GetStartTimestamp() { return fStartTime; }
        Double_t GetEndTimestamp() { return fEndTime; }
        TString GetExperimentName() { return fExperimentName; }

        Int_t GetEventID( Int_t entry ) { return fEventIDs[entry]; }
        Int_t GetSubEventID( Int_t entry ) { return fSubEventIDs[entry]; }
        TString GetSubEventTag( Int_t entry ) { return fSubEventTags[entry]; }

        TString GetInputEventName( );

        void PrintEvent (  ) { fInputEvent->PrintEvent( ); }

        Int_t GetEntry( Int_t i )
        {
            if( !fSkipEventTree && fInputEventTree == NULL )
            {
                SkipEventTreeWarning();
            }

            if( !fSkipEventTree ) fInputEventTree->GetEntry( i );
            return fInputAnalysisTree->GetEntry( i );
        }

        void SkipEventTreeWarning()
        {
                std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                std::cout << "Input event tree has not been initialized" << std::endl;
                std::cout << "This is a sign that you did not allocate an input event structure in TRestRun." << std::endl;
                std::cout << "The Event tree will be not be accesible" << std::endl;
                std::cout << "You should use : run->SetInputEvent( specificEventPointer )" << std::endl;
                std::cout << "........................... " << std::endl;
                std::cout << "... Skipping event tree ... " << std::endl;
                std::cout << "........................... " << std::endl;
                fSkipEventTree = true;
                std::cout << "If you dont need access to the event tree use fRun->SkipEventTree() to avoid this message" << std::endl;
                std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                GetChar();
        }

        void SetPureAnalysisOutput( ) { fPureAnalysisOutput = true; }
        Bool_t ContainsEventTree( ) { return fContainsEventTree; }

        void SkipEventTree( ) { fSkipEventTree = true; }
        TRestAnalysisTree *GetAnalysisTree( ) { return fInputAnalysisTree; }

        Int_t GetEntries( )
        {
            if( !fSkipEventTree && fInputEventTree == NULL )
            {
                std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                std::cout << "Input event tree has not been initialized" << std::endl;
                std::cout << "This is a sign that you did not allocate an input event structure in TRestRun." << std::endl;
                std::cout << "The Event tree will be not be accesible" << std::endl;
                std::cout << "You should use : run->SetInputEvent( specificEventPointer )" << std::endl;
                std::cout << "........................... " << std::endl;
                std::cout << "... Skipping event tree ... " << std::endl;
                std::cout << "........................... " << std::endl;
                fSkipEventTree = true;
                std::cout << "Use fRun->SkipEventTree() to avoid this message" << std::endl;
                std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                GetChar();
            }

            return fInputAnalysisTree->GetEntries();
        }

        Int_t Fill( );

        Int_t GetNumberOfProcessedEvents() { return fProcessedEvents; }

        TRestMetadata *GetHistoricMetadata(unsigned int index)
        {
            if( index < fHistoricMetadata.size() ) return fHistoricMetadata[index];
            else return NULL;
        }

        Int_t GetNumberOfInitialEvents( ) 
        {
            TRestMetadata *md = this->GetMetadataClass( "TRestG4Metadata" );

            if( md != NULL )
            {
                return StringToInteger ( md->GetParameter("Nevents" ) );
            }
            
            return -1;
        }
        Int_t GetNumberOfEvents( ) { return GetNumberOfInitialEvents(); }

        TString GetInputEventType( )
        {
            if( fContainsEventTree )
            {
                TKey *key = GetObjectKeyByClass( "TTree" );
                std::string name = key->GetName();
                unsigned int pos = name.find( "Tree" );
                return (TString) name.substr( 0, pos );
            }
            return "";
        }

        TRestMetadata *GetMetadata( TString name );
        TRestMetadata *GetMetadataClass( TString className );
        void ImportMetadata( TString rootFile, TString name, Bool_t store = true );

        void SetRunNumber( Int_t number ) { fRunNumber = number; }
        void SetParentRunNumber( Int_t number ) { fParentRunNumber = number; }

        void SetRunType( TString type )
        {
            std::string cleanType = RemoveWhiteSpaces( (std::string) type );
            fRunType = (TString) cleanType;
            fRunType = type;
        }

        void SetRunTag( TString tag )
        {
            std::string cleanTag = RemoveWhiteSpaces( (std::string) tag );
            fRunTag = (TString) cleanTag;
        }

        void SetRunUser( TString user )
        {
            std::string cleanUser = RemoveWhiteSpaces( (std::string) user );
            fRunUser = (TString) cleanUser;
        }

        void SetRunDescription( TString description ) { fRunDescription = description; }
        void SetEndTimeStamp( Double_t tStamp ) { fEndTime = tStamp; }

        void SetInputFileName( TString fN ){ fInputFilename = fN; }
        void SetOutputFileName( TString fN ){ fOutputFilename = fN; }

        TString ConstructFilename( TString filenameIn );

        TString GetDateFormatted( Double_t runTime );
        TString GetDateForFilename( Double_t runTime );
        TString GetTime( Double_t runTime );

        void ResetRunTimes();
	
        Bool_t isClass(TString className);
	
	
        //Setters
        void AddMetadata( TRestMetadata *metadata ) { fMetadata.push_back( metadata ); metadata->PrintMetadata(); }
        void AddHistoricMetadata( TRestMetadata *metadata ) { fHistoricMetadata.push_back( metadata ); }
        void AddProcess( TRestEventProcess *process, std::string cfgFilename, std::string name = "" );

        virtual void SetOutputEvent( TRestEvent *evt );
        virtual void SetInputEvent( TRestEvent *evt );

        Int_t ValidateProcessChain ( );
	
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

        void PrintTagEventList( )
        {
            cout << "Tag event list" << endl;
            cout << "--------------" << endl;
            for( unsigned int n = 0; n < fSubEventTagList.size(); n++ )
                cout << "Tag " << n << " : " << fSubEventTagList[n] << endl;
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
