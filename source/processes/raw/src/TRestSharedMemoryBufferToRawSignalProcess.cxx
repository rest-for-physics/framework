///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSharedMemoryBufferToRawSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestSharedMemoryBufferToRawSignalProcess
///             How to use: replace TRestSharedMemoryBufferToRawSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             Jun 2018:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________



#include "TRestSharedMemoryBufferToRawSignalProcess.h"
using namespace std;
#include "TTimeStamp.h"

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

void SemahoreRed( int id )
{

    Operacion.sem_num = 0;  // sem_id
    Operacion.sem_op = -1;
    Operacion.sem_flg = 0;

    semop(id, &Operacion, 1);
}

void SemaphoreGreen( int id )
{

    Operacion.sem_num = 0;  // sem_id
    Operacion.sem_op = 1;
    Operacion.sem_flg = 0;

    semop(id, &Operacion, 1);
}


ClassImp(TRestSharedMemoryBufferToRawSignalProcess)

TRestSharedMemoryBufferToRawSignalProcess::TRestSharedMemoryBufferToRawSignalProcess()
{
    Initialize();
}

TRestSharedMemoryBufferToRawSignalProcess::TRestSharedMemoryBufferToRawSignalProcess(char *cfgFileName)
{
    Initialize();
}


//______________________________________________________________________________
TRestSharedMemoryBufferToRawSignalProcess::~TRestSharedMemoryBufferToRawSignalProcess()
{
    // TRestSharedMemoryBufferToRawSignalProcess destructor
}

//______________________________________________________________________________
void TRestSharedMemoryBufferToRawSignalProcess::Initialize()
{
    /// Creating shared memory access

    SetSectionName( this->ClassName() );
    fSignalEvent = new TRestRawSignalEvent( );

    fInputEvent = NULL;
    fOutputEvent = fSignalEvent;

    fIsExternal = true;


}

//______________________________________________________________________________
void TRestSharedMemoryBufferToRawSignalProcess::InitProcess()
{
    cout << "TRestSharedMemoryBufferToRawSignalProcess::InitProcess" << endl;

    key_t MemKey = ftok ("/bin/ls", 3);
    int memId  = shmget (MemKey, sizeof(daqInfo), 0777);
    printf( "memId : %d\n", memId );
    if( memId == -1 )
    {
        printf("Failed to access resource\n");
        return;
    }

    fShMem_daqInfo = (daqInfo *) shmat ( memId, (char *)0, 0);


    key_t SemaphoreKey = ftok ("/bin/ls", 14);
    fSemaphoreId = semget ( SemaphoreKey, 1, 0777 );

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
    }

    int N_DATA = fShMem_daqInfo->bufferSize;

    MemKey = ftok ("/bin/ls", 13);
    memId  = shmget (MemKey, N_DATA * sizeof(unsigned short int), 0777 | IPC_CREAT);
    fShMem_Buffer = (unsigned short int *) shmat ( memId, (char *) 0, 0);
}

//______________________________________________________________________________
TRestEvent* TRestSharedMemoryBufferToRawSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    if( GetVerboseLevel() >= REST_Debug )
        cout << "TRestSharedMemoryBufferToRawSignalProcess::ProcessEvent" << endl;

    while( true )
    {
        SemahoreRed( fSemaphoreId );
        int dataReady = fShMem_daqInfo->dataReady;
        int maxSignals =  fShMem_daqInfo->maxSignals;
        SemaphoreGreen( fSemaphoreId );
        
        usleep( 100000 );

        if( dataReady == 2 )
        {
            usleep( 10000 );
            cout << "Data is ready. I should dump to TRestRawSignal" << endl;

            cout << "Daq channel : " << fShMem_Buffer[0] << endl;
            SemahoreRed( fSemaphoreId );
            for( int n = 1; n < maxSignals + 1; n++ )
                cout << "Sample : " << n-1 << " data : " << fShMem_Buffer[n] << endl;
            SemaphoreGreen( fSemaphoreId );

            if( GetVerboseLevel() >= REST_Info )
            {
                cout << "------------------------------------------" <<endl;
                cout << "Event ID : " << fSignalEvent->GetID() << endl;
                cout << "Time stamp : " << fSignalEvent->GetTimeStamp() << endl;
                cout << "Number of Signals : " << fSignalEvent->GetNumberOfSignals() << endl;
                cout << "------------------------------------------" <<endl;

                if( GetVerboseLevel() >= REST_Debug )
                {
                    for( Int_t n = 0; n < fSignalEvent->GetNumberOfSignals(); n++ )
                        cout << "Signal N : " << n << " daq id : " << fSignalEvent->GetSignal(n)->GetID() << endl;
                    GetChar();
                }
            }

            if( fSignalEvent->GetNumberOfSignals() == 0 ) return NULL;

            return fSignalEvent;
        }
    }


    if( fSignalEvent->GetNumberOfSignals() == 0 ) return NULL;

    return fSignalEvent;

}

void TRestSharedMemoryBufferToRawSignalProcess::InitFromConfigFile( )
{
    fMaxSignals = StringToInteger ( GetParameter( "maxSignals", "1152" ) );
    fMaxPointsPerSignal = StringToInteger ( GetParameter( "maxPoints", "512" ) );

    fNdata = fMaxSignals * ( fMaxPointsPerSignal + 1 );
}

