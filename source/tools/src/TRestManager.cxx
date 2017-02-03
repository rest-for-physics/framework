///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestManager.cxx
///
///             apr 2016    Javier Galan
///_______________________________________________________________________________


#include "TRestManager.h"
using namespace std;

// specific metadata
#include <TRestReadout.h>
#include <TRestGas.h>
#include <TRestDetectorSetup.h>

// REST processes
/*
#include <TRestSignalToHitsProcess.h>
#include <TRestFastHitsToTrackProcess.h>
#include <TRestHitsToTrackProcess.h>
#include <TRestG4toHitsProcess.h>

// physics processes
#include <TRestElectronDiffusionProcess.h>
#include <TRestAvalancheProcess.h>

// hits processes
#include <TRestSmearingProcess.h>
#include <TRestHitsNormalizationProcess.h>
#include <TRestHitsReductionProcess.h>
#include <TRestHitsShuffleProcess.h>

// track processes
#include <TRestTrackReductionProcess.h>
#include <TRestTrackPathMinimizationProcess.h>
#include <TRestTrackReconnectionProcess.h>

// signal processes
#include <TRestAddSignalNoiseProcess.h>
#include <TRestSignalGaussianConvolutionProcess.h>
#include <TRestSignalShapingProcess.h>
#include <TRestSignalToRawSignalProcess.h>

*/
// external processes
#include <TRestFEMINOSToSignalProcess.h>
#include <TRestCoBoAsAdToSignalProcess.h>

// analysis processes
/*
#include <TRestGeant4AnalysisProcess.h>
#include <TRestFindG4BlobAnalysisProcess.h>
#include <TRestSignalAnalysisProcess.h>
#include <TRestTrackAnalysisProcess.h>
#include <TRestTriggerAnalysisProcess.h>
#include <TRestHitsAnalysisProcess.h>
#include <TRestFindTrackBlobsProcess.h>
*/


// task processes
#include <TRestAnalysisPlot.h>

const int debug = 0;

ClassImp(TRestManager)
    //______________________________________________________________________________
TRestManager::TRestManager()
{
    Initialize();

}

TRestManager::TRestManager( const char *cfgFileName, const char *name) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfigFromFile( fConfigFileName, name );
}

void TRestManager::Initialize()
{
    SetSectionName( this->ClassName() );

    fRun = NULL;

    fFirstEntry = 0;
    fLastEntry = 0;
    fNEventsToProcess = 0;

    fEventsProcessed = false;
}


//______________________________________________________________________________
TRestManager::~TRestManager()
{
    if( fRun != NULL ) delete fRun;
}

//______________________________________________________________________________
void TRestManager::InitFromConfigFile()
{
    TString inputFile = GetParameter("inputFile" );
 
    fInputFile = inputFile;

    char *cfgFile = (char *) fConfigFileName.c_str(); 
    fRun = new TRestRun( cfgFile );

    fFirstEntry = StringToInteger( GetParameter( "firstEntry", "0") );
    fNEventsToProcess = StringToInteger( GetParameter( "eventsToProcess", "0") );
    fLastEntry = StringToInteger( GetParameter( "lastEntry", "0") );

    // Adding processes
    size_t position = 0;
    string addProcessString;
    while( ( addProcessString = GetKEYDefinition( "addProcess", position ) ) != "" )
    {
        TString active = GetFieldValue( "value", addProcessString );
        if( active != "ON" && active != "On" && active != "on" ) continue;

        TString processesCfgFile = GetParameter( "processesFile" );
        TString processName = GetFieldValue( "name", addProcessString );

        TString processType = GetFieldValue( "type", addProcessString );

        fProcessType.push_back( processType );
        fProcessName.push_back( processName );
        fPcsConfigFile.push_back( processesCfgFile ); 
    }

    if( fProcessType.size() > 0 )
    {
        TClass *cl = TClass::GetClass( fProcessType[0] );
        if( cl == NULL )
        {
            cout << "TRestManager. " << GetName() << " : ERROR" << endl;
            cout << "Process : " << fProcessType[0] << " unknown!!" << endl;
            exit(0);
        }

        TRestEventProcess *pc = (TRestEventProcess *) cl->New();
        
        if( !pc->isExternal() ) fRun->OpenInputFile( inputFile );
    }

    TString analysisString = GetParameter( "pureAnalysisOutput", "OFF" );
    if( analysisString == "ON" || analysisString == "On" || analysisString == "on" )
        fRun->SetPureAnalysisOutput();

    // Adding metadata
    position = 0;
    string addMetadataString;
    while( ( addMetadataString = GetKEYDefinition( "addMetadata", position ) ) != "" )
    {
        TString active = GetFieldValue( "value", addMetadataString );
        if( active != "ON" && active != "On" && active != "on" ) continue;

        TString metadataType = GetFieldValue( "type", addMetadataString );

        if( metadataType == "TRestReadout" ) AddReadout( addMetadataString );

        if( metadataType == "TRestGas" ) AddGas( addMetadataString );

    }

    position = 0;
    string readoutPlaneString;
    while( ( readoutPlaneString = GetKEYDefinition( "readoutPlane", position ) ) != "" )
    {
        Int_t rId = StringToInteger ( GetFieldValue( "id", readoutPlaneString ) );

        TVector3 plPos = StringTo3DVector( GetFieldValue( "planePosition", readoutPlaneString ) );

        TVector3 vPos = StringTo3DVector( GetFieldValue( "planeVector", readoutPlaneString ) );

        TVector3 cPos = StringTo3DVector( GetFieldValue( "cathodePosition", readoutPlaneString ) );

        TRestReadout *readout = (TRestReadout *) fRun->GetMetadataClass( "TRestReadout" );

        // Removed condition when it was (0,0,0). Since it is a valid value.
        // Some other value should be given as default value
        readout->GetReadoutPlane( rId )->SetPosition( plPos );

        readout->GetReadoutPlane( rId )->SetCathodePosition( cPos );

        if( vPos != TVector3(0, 0, 0) )
            readout->GetReadoutPlane( rId )->SetPlaneVector( vPos );

        readout->GetReadoutPlane( rId )->SetDriftDistance();

        readout->PrintMetadata();
    }

    // Adding tasks
    position = 0;
    string addTaskString;
    while( ( addTaskString = GetKEYDefinition( "addTask", position ) ) != "" )
    {
        TString active = GetFieldValue( "value", addTaskString );
        if( active != "ON" && active != "On" && active != "on" ) continue;

        TString tasksCfgFile = GetParameter( "tasksFile" );

        TString taskName = GetFieldValue( "name", addTaskString );
        TString taskType = GetFieldValue( "type", addTaskString );

        fTaskType.push_back( taskType );
        fTaskName.push_back( taskName );
        fTasksConfigFile.push_back( tasksCfgFile ); 
    }

}

void TRestManager::LaunchTasks( )
{

    for( unsigned int n = 0; n < fTaskType.size(); n++ )
    {
        if( fTaskType[n] == "analysisPlot" )
        {
            TRestAnalysisPlot *anPlot = new TRestAnalysisPlot( fTasksConfigFile[n], fTaskName[n] );

            TString fName = fInputFile;
            if( fEventsProcessed ) fName = fRun->GetOutputFilename( );

            anPlot->AddFile( fName );

            anPlot->PlotCombinedCanvas( );

            delete anPlot;
        }
        else if( fTaskType[n] == "processEvents" )
        {
            ProcessEvents( );
        }
        else
        {
            cout << "REST WARNING : TRestManager::LaunchTasks(). Task type : " << fTaskType[n] << " not recognized" << endl;

        }

    }

}

Int_t TRestManager::LoadProcesses( )
{
    TString processType;
    TString processName;
    TString processesCfgFile;

    Int_t nProcesses = 0;

    for( unsigned int i = 0; i < fProcessType.size(); i++ )
    {
        TClass *cl = TClass::GetClass( fProcessType[i] );

        if( cl == NULL )
        {
            cout << " " << endl;
            cout << "REST ERROR. TRestManager. Process : " << fProcessType[i] << " not found!!" << endl;
            cout << "Please verify the process type name and launch again." << endl;
            cout << "If you are not willing to use this process you can deactivate using value=\"off\"" << endl;
            cout << " " << endl;
            cout << "This process will be skipped." << endl;
            GetChar();
            continue;
        }

        // TODO: Still we need to improve this so that is more generic.
        // I.e. OpenInputBinFile can be done in InitProcess
        if( i == 0 && fProcessType[i] == "TRestFEMINOSToSignalProcess" )
        {
            TRestDetectorSetup *detSetup = new TRestDetectorSetup();
            detSetup->SetName("DetectorSetup" );
            detSetup->SetTitle("FeminosSetup" );
            detSetup->InitFromFileName( fInputFile );

            fRun->AddMetadata( detSetup );

            TRestFEMINOSToSignalProcess *femPcs = new TRestFEMINOSToSignalProcess();

            fRun->AddProcess( femPcs, (string) fPcsConfigFile[i], (string) fProcessName[i] );

            if( !femPcs->OpenInputBinFile( fInputFile ) )
            {
                cout << "Error file not found : " << fInputFile << endl;
                GetChar();
                continue;
            }

            fRun->SetParentRunNumber( detSetup->GetSubRunNumber() );
            fRun->SetRunNumber( detSetup->GetRunNumber() );
            fRun->SetRunTag( detSetup->GetRunTag() );
        }
        else if( i == 0 && fProcessType[i] == "TRestCoboAsadToSignalProcess" )
        {

            //TRestDetectorSetup *detSetup = new TRestDetectorSetup();
            //detSetup->InitFromFileName( fInputFile );

            //fRun->AddMetadata( detSetup );

            TRestCoBoAsAdToSignalProcess *coboPcs = new TRestCoBoAsAdToSignalProcess();

            fRun->AddProcess( coboPcs, (string) fPcsConfigFile[i], (string) fProcessName[i] );

            if( !coboPcs->OpenInputCoBoAsAdBinFile( fInputFile ) )
  //          if( !coboPcs->OpenInputBinFile( fInputFile ) )
            {
                cout << "REST ERROR: TRestManager. File not found : " << fInputFile << endl;
                exit(1);
            }

            //fRun->SetParentRunNumber( detSetup->GetSubRunNumber() );
            //fRun->SetRunNumber( detSetup->GetRunNumber() );
            //fRun->SetRunTag( detSetup->GetRunTag() );
        }
        else
        {
            TRestEventProcess *pc = (TRestEventProcess *) cl->New( );

            if( i != 0 && pc->isExternal( ) )
            {
                cout << "REST ERROR: TRestManager. Only the first process can be external" << endl;
                exit(1);
            }

            if( pc->isExternal() ) pc->OpenInputFile( fInputFile );

            fRun->AddProcess( pc, (string) fPcsConfigFile[i], (string) fProcessName[i] );
        }

        nProcesses++;
    }
    
    cout << fRun->GetNumberOfProcesses() << " processes loaded." << endl;
    if (nProcesses !=  fRun->GetNumberOfProcesses() )
         cout << "WARNING: no all requested proceses were loaded." << endl;
   
    return nProcesses;
}


void TRestManager::AddReadout( string readoutDefinition )
{
    TRestReadout *readout = (TRestReadout *) fRun->GetMetadataClass( "TRestReadout" );

    Bool_t fOverwrite = false;
    Bool_t fStore = true;
    if( GetFieldValue( "overwrite", readoutDefinition ) == "true" ) fOverwrite = true;
    if( GetFieldValue( "store", readoutDefinition ) == "false" ) fStore = false;

    if( readout != NULL && fOverwrite )
    {
        readout->PrintMetadata();
        cout << "Overwritting a readout is not yet IMPLEMENTED." << endl;
        cout << "Please send a request to rest-dev@cern.ch if necessary" << endl;
        cout << "The existing readout will be used." << endl;
        GetChar();
    } 
    
    if( readout != NULL )
    {
        if( !fStore ) { readout->DoNotStore(); }
        return;
    }

    TString readoutFile = GetParameter( "readoutFile" );
    TString readoutName = GetFieldValue( "name", readoutDefinition );

    Bool_t isRoot = isRootFile( ((string) readoutFile).c_str() );

    if( isRoot ) fRun->ImportMetadata( readoutFile, readoutName, fStore );
    else
    {
        readout = new TRestReadout( readoutFile.Data(), (string) readoutName );
        if( !fStore ) readout->DoNotStore();
        fRun->AddMetadata( readout );
    }

}

void TRestManager::AddGas( string gasDefinition )
{
    TRestGas *gas = (TRestGas *) fRun->GetMetadataClass( "TRestGas" );

    Bool_t fOverwrite = false;
    if( GetFieldValue( "overwrite", gasDefinition ) == "true" ) fOverwrite = true;

    if( gas != NULL && fOverwrite )
    {
        cout << "Overwritting a gas is not yet IMPLEMENTED." << endl;
        cout << "Please send a request to rest-dev@cern.ch if necessary" << endl;
        cout << "The existing gas will be used." << endl;
        gas->PrintMetadata();
        GetChar();
    } 

    if( gas != NULL )
    {
        gas->LoadGasFile();
        return;
    }

    TString gasFile = GetParameter( "gasFile" );
    TString gasName = GetFieldValue( "name", gasDefinition );

    gas = new TRestGas( gasFile.Data(), (string) gasName );
    gas->SetPressure( StringToDouble( GetFieldValue( "pressure", gasDefinition ) ) );

    fRun->AddMetadata( gas );

    gas = (TRestGas *) fRun->GetMetadataClass( "TRestGas" );
    gas->LoadGasFile();
}

void TRestManager::PrintMetadata( )
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "Manager : " << GetTitle() << endl;
 //   cout << "Number of processes : " << fProcesses.size() << endl;
    cout << "====================================" << endl;
    cout << endl;
    cout << "====================================" << endl;

}

