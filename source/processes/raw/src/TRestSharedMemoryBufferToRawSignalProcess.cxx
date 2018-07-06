/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// TRestSharedMemoryBufferToRawSignalProcess gets access to an existing
/// buffer in XXXXX
///
/// daqInfo structure
/// 
/// \warning If the value assigned to a data point in the output rawsignal
/// event exceeds 32768 it will cause an overflow, and the event data will
/// be corrupted. If the verboseLevel of the process is warning, an output
/// message will prevent the user. The event status will be invalid.
/// 
/// The following list describes the different parameters that can be 
/// used in this process.
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2018-July:  First implementation of shared memory buffer to rawsignal conversion.
///             Javier Galan
///
/// \class      TRestSharedMemoryBufferToRawSignalProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestSharedMemoryBufferToRawSignalProcess.h"
using namespace std;

#include <sys/shm.h>
#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// The union is already defined in sys/sem.h
#else
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
#endif

struct sembuf Operacion;

ClassImp( TRestSharedMemoryBufferToRawSignalProcess )

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestSharedMemoryBufferToRawSignalProcess::TRestSharedMemoryBufferToRawSignalProcess()
{
    Initialize();
}

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
/// 
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or relative.
///
/// The default behaviour is that the config file must be specified with 
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestSharedMemoryBufferToRawSignalProcess::TRestSharedMemoryBufferToRawSignalProcess( char *cfgFileName )
{
    Initialize();

    LoadConfig( cfgFileName );
}

///////////////////////////////////////////////
/// \brief Default destructor 
/// 
TRestSharedMemoryBufferToRawSignalProcess::~TRestSharedMemoryBufferToRawSignalProcess()
{
    delete fOutputRawSignalEvent;
}

///////////////////////////////////////////////
/// \brief This method will increase the semaphore red level to protect shared memory regions
/// 
void TRestSharedMemoryBufferToRawSignalProcess::SemaphoreRed( int id )
{

    Operacion.sem_num = 0;  // sem_id
    Operacion.sem_op = -1;
    Operacion.sem_flg = 0;

    semop(id, &Operacion, 1);
}

///////////////////////////////////////////////
/// \brief This method will increase the semaphore green level to release shared memory regions
/// 
void TRestSharedMemoryBufferToRawSignalProcess::SemaphoreGreen( int id )
{

    Operacion.sem_num = 0;  // sem_id
    Operacion.sem_op = 1;
    Operacion.sem_flg = 0;

    semop(id, &Operacion, 1);
}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
/// 
void TRestSharedMemoryBufferToRawSignalProcess::LoadDefaultConfig( )
{
    SetName( "sharedMemoryBufferToRawSignal-Default" );
    SetTitle( "Default config" );

    cout << "SharedMemoryBufferToRawSignal metadata not found. Loading default values" << endl;
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration file.
/// 
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the 
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestSharedMemoryBufferToRawSignalProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the section name
/// 
void TRestSharedMemoryBufferToRawSignalProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fOutputRawSignalEvent = new TRestRawSignalEvent();

    fInputEvent = NULL;
    fOutputEvent = fOutputRawSignalEvent;

    fIsExternal = true;

    fReset = true;
}


void TRestSharedMemoryBufferToRawSignalProcess::InitProcess()
{

    cout << "TRestSharedMemoryBufferToRawSignalProcess::InitProcess. Creating access to shared memory" << endl;

    key_t MemKey = ftok ("/bin/ls", fKeyDaqInfo );
    int memId  = shmget (MemKey, sizeof(daqInfo), 0777);
    if( memId == -1 )
    {
        printf("Failed to access daqInfo resource\n");
        exit(1);
    }

    fShMem_daqInfo = (daqInfo *) shmat ( memId, (char *)0, 0);


    key_t SemaphoreKey = ftok ("/bin/ls", fKeySemaphore );
    fSemaphoreId = semget ( SemaphoreKey, 1, 0777 );

    if( fSemaphoreId == -1 )
    {
        printf("Failed to access semaphore resource\n");
        exit(1);
    }

    if( GetVerboseLevel() >= REST_Debug )
    {
	    printf( "Sem id : %d\n", fSemaphoreId );

	    printf( "Data ready : %d\n", fShMem_daqInfo->dataReady );

	    printf( "Number of signals : %d\n", fShMem_daqInfo->nSignals );

	    printf( "Timestamp : %lf\n", fShMem_daqInfo->timeStamp );

	    printf( "Event id : %d\n", fShMem_daqInfo->eventId );

	    printf( "Max signals :  %d\n", fShMem_daqInfo->maxSignals );
	    printf( "Max samples : %d\n", fShMem_daqInfo->maxSamples );
	    printf( "Buffer size : %d\n", fShMem_daqInfo->bufferSize );

	    if( GetVerboseLevel() >= REST_Extreme )
		    GetChar();
    }

    int N_DATA = fShMem_daqInfo->bufferSize;

    MemKey = ftok ("/bin/ls", fKeyBuffer );
    memId  = shmget (MemKey, N_DATA * sizeof(unsigned short int), 0777 | IPC_CREAT);
    if( memId == -1 )
    {
        printf("Failed to access buffer resource\n");
        exit(1);
    }

    fShMem_Buffer = (unsigned short int *) shmat ( memId, (char *) 0, 0);
}

///////////////////////////////////////////////
/// \brief Function including required initialization before each event starts to process.
/// 
void TRestSharedMemoryBufferToRawSignalProcess::BeginOfEventProcess() 
{
    fOutputRawSignalEvent->Initialize(); 
}

///////////////////////////////////////////////
/// \brief The main processing event function
/// 
TRestEvent* TRestSharedMemoryBufferToRawSignalProcess::ProcessEvent( TRestEvent *evInput )
{

    while( true )
    {
        SemaphoreRed( fSemaphoreId );
        int dataReady = fShMem_daqInfo->dataReady;
        int maxSamples =  fShMem_daqInfo->maxSamples;
        SemaphoreGreen( fSemaphoreId );
        
	// We sleep a while to do not saturate/block the daq with the semaphore.
        usleep( fTimeDelay );

        if( dataReady == 2 )
        {

	    //// START Getting access to shared resources
            SemaphoreRed( fSemaphoreId );

	    for( unsigned int s = 0; s < fShMem_daqInfo->nSignals; s++ )
	    {
		TRestRawSignal sgnl;
		sgnl.SetSignalID( fShMem_Buffer[ s * (maxSamples+1) ] );

		if( GetVerboseLevel() >= REST_Debug )
			cout << "s : " << s << " id : " << sgnl.GetSignalID() << endl;

		for( int n = 0; n < maxSamples; n++ )
			sgnl.AddPoint( fShMem_Buffer[ s * (maxSamples+1) + 1 + n] );
		fOutputRawSignalEvent->AddSignal( sgnl );
		
		if( GetVerboseLevel() >= REST_Extreme )
		{
			sgnl.Print();
			GetChar();
		}
	    }

	    if( fReset )
	    {
		for( unsigned int n = 0; n < fShMem_daqInfo->bufferSize; n++ )
			fShMem_Buffer[n] = 0;
	    }

	    fShMem_daqInfo->dataReady = 0;

            fOutputRawSignalEvent->SetID( fShMem_daqInfo->eventId );
            fOutputRawSignalEvent->SetTime ( fShMem_daqInfo->timeStamp );

            SemaphoreGreen( fSemaphoreId );
	    //// END Getting access to shared resources

            if( GetVerboseLevel() >= REST_Info )
            {
                cout << "------------------------------------------" <<endl;
                cout << "Event ID : " << fOutputRawSignalEvent->GetID() << endl;
                cout << "Time stamp : " << fOutputRawSignalEvent->GetTimeStamp() << endl;
                cout << "Number of Signals : " << fOutputRawSignalEvent->GetNumberOfSignals() << endl;
                cout << "------------------------------------------" <<endl;

                if( GetVerboseLevel() >= REST_Debug )
                {
                    for( Int_t n = 0; n < fOutputRawSignalEvent->GetNumberOfSignals(); n++ )
                        cout << "Signal N : " << n << " daq id : " << fOutputRawSignalEvent->GetSignal(n)->GetID() << endl;
                    GetChar();
                }
            }

            if( fOutputRawSignalEvent->GetNumberOfSignals() == 0 ) return NULL;

            return fOutputRawSignalEvent;
        }
    }

    return fOutputRawSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML TRestSharedMemoryBufferToRawSignalProcess metadata section
/// 
void TRestSharedMemoryBufferToRawSignalProcess::InitFromConfigFile( )
{
    fKeyDaqInfo = StringToInteger( GetParameter( "daqInfoKey", "3" ) );

    fKeyBuffer = StringToInteger( GetParameter( "bufferKey", "13" ) );

    fKeySemaphore = StringToInteger( GetParameter( "semaphoreKey", "14" ) );

    fTimeDelay = StringToInteger( GetParameter( "timeDelay", "10000" ) );
}

