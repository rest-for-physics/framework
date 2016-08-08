///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestEventProcess.h
///
///             Base class from which to inherit all REST process classes inherit
///             A process class in REST deals with one input, ouput (or both) single REST
///             events. RestMetadata controls the access to config files.
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#ifndef RestCore_TRestEventProcess
#define RestCore_TRestEventProcess

#include "TNamed.h"
#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestAnalysisTree.h"

class TRestEventProcess:public TRestMetadata {
 protected:
   Int_t fStatusOfProcess;		// integer to hold the status of task: 0 = successful >0 = some error happened
   Int_t fVerbose;              //! Verbose level of the process execution [0...3]

   virtual void InitFromConfigFile() = 0;

#ifndef __CINT__
   TRestEvent *fInputEvent;
   TRestEvent *fOutputEvent;

   std::vector <TRestMetadata*> fRunMetadata;

   std::vector <string> fObservableNames;

   TRestAnalysisTree *fAnalysisTree;
#endif

 private:

   virtual void LoadDefaultConfig()=0;

 public:
   virtual TRestEvent *GetInputEvent() { return fInputEvent; }
   virtual TRestEvent *GetOutputEvent() { return fOutputEvent; }

   virtual void InitProcess() { }
   virtual TRestEvent *ProcessEvent( TRestEvent *evInput ) = 0;
   virtual void EndProcess() { }
   virtual void BeginOfEventProcess() { fOutputEvent->Initialize(); }
   virtual void EndOfEventProcess() { }
   virtual TString GetProcessName() = 0;
   virtual void LoadConfig( std::string cfgFilename, std::string cfgName = "" )=0;

   virtual Bool_t isExternal( ) { return false; }

   vector <string> ReadObservables( )
   {
       vector <string> obsList = GetObservablesList( );

       for( unsigned int n = 0; n < obsList.size(); n++ )
       {
	       fAnalysisTree->AddObservable( this->GetName() + (TString) "." + (TString) obsList[n] );
		fObservableNames.push_back ( this->GetName() + (string) "." + obsList[n] );
       }

       return obsList;
   }

   TRestMetadata *GetGasMetadata( );
   TRestMetadata *GetReadoutMetadata( );
   TRestMetadata *GetGeant4Metadata( );
   TRestMetadata *GetDetectorSetup( );

   Double_t GetDoubleParameterFromClass( TString className, TString parName );
   Double_t GetDoubleParameterFromClassWithUnits( TString className, TString parName );

   virtual TRestMetadata *GetProcessMetadata() { return NULL; }
   void SetMetadata( std::vector <TRestMetadata*> meta ) { fRunMetadata = meta; }

   void SetAnalysisTree( TRestAnalysisTree *tree ) { fAnalysisTree = tree; }

   void BeginPrintProcess();
   void EndPrintProcess();

   //Getters
   Int_t GetStatus() { return fStatusOfProcess; }
   Int_t GetVerboseLevel() { return fVerbose; } 
   //Setters
   void SetVerboseLevel(Int_t verbose) { fVerbose = verbose; }

   //Constructor
   TRestEventProcess();
   //Destructor
   ~TRestEventProcess();

   ClassDef(TRestEventProcess, 1);      // Base class for a REST process
};
#endif
