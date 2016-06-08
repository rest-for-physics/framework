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
#include <TRestHitsToSignalProcess.h>
#include <TRestSignalToHitsProcess.h>
#include <TRestFastHitsToTrackProcess.h>

// physics processes
#include <TRestElectronDiffusionProcess.h>
#include <TRestAvalancheProcess.h>

// hits processes
#include <TRestSmearingProcess.h>

// track processes
#include <TRestTrackReductionProcess.h>
#include <TRestTrackPathMinimizationProcess.h>

// signal processes
#include <TRestAddSignalNoiseProcess.h>
#include <TRestSignalGaussianConvolutionProcess.h>
#include <TRestSignalShapingProcess.h>
#include <TRestSignalToRawSignalProcess.h>

// external processes
#include <TRestFEMINOSToSignalProcess.h>

// analysis processes
#include <TRestGeant4AnalysisProcess.h>
#include <TRestFindG4BlobAnalysisProcess.h>
#include <TRestSignalAnalysisProcess.h>
#include <TRestTrackAnalysisProcess.h>
#include <TRestTriggerAnalysisProcess.h>
#include <TRestHitsAnalysisProcess.h>

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
    SetName("manager");

    fRun = NULL;

    fFirstEntry = 0;
    fLastEntry = 0;
    fNEventsToProcess = 0;
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

    Bool_t isAcquisition = inputFile.EndsWith("aqs");
    if( !isAcquisition ) fRun->OpenInputFile( inputFile );

    TString analysisString = GetParameter( "pureAnalysisOutput", "OFF" );
    if( analysisString == "ON" || analysisString == "On" || analysisString == "on" )
        fRun->SetPureAnalysisOutput();

    // Adding metadata
    size_t position = 0;
    string addMetadataString;
    while( ( addMetadataString = GetKEYDefinition( "addMetadata", position ) ) != "" )
    {
        TString active = GetFieldValue( "value", addMetadataString );
        if( active != "ON" && active != "On" && active != "on" ) continue;

        TString metadataType = GetFieldValue( "type", addMetadataString );

        if( metadataType == "readout" ) AddReadout( addMetadataString );

        if( metadataType == "gas" ) AddGas( addMetadataString );

    }

    position = 0;
    string readoutPlaneString;
    while( ( readoutPlaneString = GetKEYDefinition( "readoutPlane", position ) ) != "" )
    {
        Int_t rId = StringToInteger ( GetFieldValue( "id", readoutPlaneString ) );

        TVector3 plPos = StringTo3DVector( GetFieldValue( "planePosition", readoutPlaneString ) );

        TVector3 cPos = StringTo3DVector( GetFieldValue( "cathodePosition", readoutPlaneString ) );

        TRestReadout *readout = (TRestReadout *) fRun->GetMetadataClass( "TRestReadout" );

        if( plPos != TVector3(0,0,0) )
            readout->GetReadoutPlane( rId )->SetPosition( plPos );

        if( cPos != TVector3(0,0,0) )
            readout->GetReadoutPlane( rId )->SetCathodePosition( cPos );

        readout->GetReadoutPlane( rId )->SetDriftDistance();

        readout->PrintMetadata();
    }

    // Adding processes
    position = 0;
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

}

void TRestManager::LoadProcesses( )
{
    TString processType;
    TString processName;
    TString processesCfgFile;

    for( unsigned int i = 0; i < fProcessType.size(); i++ )
    {
        processType = fProcessType[i];
        processName = fProcessName[i];
        processesCfgFile = fPcsConfigFile[i];

        if( processType == "geant4AnalysisProcess" )
            fRun->AddProcess( new TRestGeant4AnalysisProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "findG4BlobAnalysisProcess" )
            fRun->AddProcess( new TRestFindG4BlobAnalysisProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "electronDiffusionProcess" )
            fRun->AddProcess( new TRestElectronDiffusionProcess( ), (string) processesCfgFile, (string) processName );

        /*
           if( processType == "avalancheProcess" )
           fRun->AddProcess( new TRestAvalancheProcess( ), (string) processesCfgFile, (string) processName );
           */

        if( processType == "hitsToSignalProcess" )
            fRun->AddProcess( new TRestHitsToSignalProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "signalAnalysisProcess" )
            fRun->AddProcess( new TRestSignalAnalysisProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "hitsAnalysisProcess" )
            fRun->AddProcess( new TRestHitsAnalysisProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "signalToHitsProcess" )
            fRun->AddProcess( new TRestSignalToHitsProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "fastHitsToTrackProcess" )
            fRun->AddProcess( new TRestFastHitsToTrackProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "trackReductionProcess" )
            fRun->AddProcess( new TRestTrackReductionProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "trackPathMinimizationProcess" )
            fRun->AddProcess( new TRestTrackPathMinimizationProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "trackAnalysisProcess" )
            fRun->AddProcess( new TRestTrackAnalysisProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "triggerAnalysisProcess" )
            fRun->AddProcess( new TRestTriggerAnalysisProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "feminosToSignalProcess" )
        {
            TRestDetectorSetup *detSetup = new TRestDetectorSetup();
            detSetup->InitFromFileName( fInputFile );

            fRun->AddMetadata( detSetup );

            TRestFEMINOSToSignalProcess *femPcs = new TRestFEMINOSToSignalProcess();

            fRun->AddProcess( femPcs, (string) processesCfgFile, (string) processName );

            if( !femPcs->OpenInputBinFile( fInputFile ) )
            {
                cout << "Error file not found : " << fInputFile << endl;
                GetChar();
                continue;
            }

            fRun->SetParentRunNumber( detSetup->GetRunNumber() );
            fRun->SetRunTag( detSetup->GetRunTag() );
        }

        if( processType == "addSignalNoiseProcess" )
            fRun->AddProcess( new TRestAddSignalNoiseProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "signalGaussianConvolutionProcess" )
            fRun->AddProcess( new TRestSignalGaussianConvolutionProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "signalShapingProcess" )
            fRun->AddProcess( new TRestSignalShapingProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "signalToRawSignalProcess" )
            fRun->AddProcess( new TRestSignalToRawSignalProcess( ), (string) processesCfgFile, (string) processName );

        if( processType == "smearingProcess" )
            fRun->AddProcess( new TRestSmearingProcess( ), (string) processesCfgFile, (string) processName );

        LoadExternalProcess( processType, (string) processesCfgFile, (string) processName );
    }
}


void TRestManager::AddReadout( string readoutDefinition )
{
    TRestReadout *readout = (TRestReadout *) fRun->GetMetadataClass( "TRestReadout" );

    Bool_t fOverwrite = false;
    if( GetFieldValue( "overwrite", readoutDefinition ) == "true" ) fOverwrite = true;

    if( readout != NULL && fOverwrite )
    {
        cout << "Overwritting a readout is not yet IMPLEMENTED." << endl;
        cout << "Please send a request to rest-dev@cern.ch if necessary" << endl;
        cout << "The existing readout will be used." << endl;
        readout->PrintMetadata();
        GetChar();
    } 
    
    if( readout != NULL ) return;

    TString readoutFile = GetParameter( "readoutFile" );
    TString readoutName = GetFieldValue( "name", readoutDefinition );

    Bool_t isRoot = isRootFile( ((string) readoutFile).c_str() );

    if( isRoot ) fRun->ImportMetadata( readoutFile, readoutName );
    else
    {
        readout = new TRestReadout( readoutFile.Data(), (string) readoutName );
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

    gas->PrintMetadata();
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

