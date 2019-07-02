///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAFTERToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestAFTERToSignalProcess
///             How to use: replace TRestAFTERToSignalProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________

#include "TRestAFTERToSignalProcess.h"
using namespace std;
#include "TTimeStamp.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <arpa/inet.h>
#endif
#include "mygblink.h"

#define MAX_UNSIGNED_INT 4294967295

ClassImp(TRestAFTERToSignalProcess)
    //______________________________________________________________________________
    TRestAFTERToSignalProcess::TRestAFTERToSignalProcess() {
    Initialize();
}

TRestAFTERToSignalProcess::TRestAFTERToSignalProcess(char* cfgFileName) { Initialize(); }

//______________________________________________________________________________
TRestAFTERToSignalProcess::~TRestAFTERToSignalProcess() {
    // TRestAFTERToSignalProcess destructor
}

//______________________________________________________________________________
void TRestAFTERToSignalProcess::Initialize() {
    TRestRawToSignalProcess::Initialize();
    // this->SetVerboseLevel(REST_Debug);

    prevTime = 0;
    reducedTime = 0;
}

//______________________________________________________________________________
void TRestAFTERToSignalProcess::InitProcess() {
    // Binary file header

    // The binary starts here
    char runUid[21], initTime[21];
    fread(runUid, 1, 20, fInputBinFile);

    sprintf(initTime, "%s", runUid);
    printf("File UID is %s \n", initTime);
    totalBytesReaded = sizeof(runUid);

    int year, day, month, hour, minute, second;
    sscanf(runUid, "R%d.%02d.%02d-%02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second);
    printf("R%d_%02d_%02d-%02d_%02d_%02d\n", year, month, day, hour, minute, second);
    TTimeStamp tS(year, month, day, hour, minute, second);
    tStart = tS.AsDouble();
    cout << tStart << endl;
    // Timestamp of the run
}

//______________________________________________________________________________
TRestEvent* TRestAFTERToSignalProcess::ProcessEvent(TRestEvent* evInput) {
    EventHeader head;
    DataPacketHeader pHeader;
    DataPacketEnd pEnd;

    // Read next header or quit of end of file
    if (fread(&head, sizeof(EventHeader), 1, fInputBinFile) != 1) {
        fclose(fInputBinFile);
        cout << "Error reading event header :-(" << endl;
        cout << "... or end of file found :-)" << endl;
        return NULL;
    }

    head.eventSize = ntohl(head.eventSize);
    head.eventNumb = ntohl(head.eventNumb);

    payload = head.eventSize;
    frameBits = sizeof(head);

    if (this->GetVerboseLevel() == REST_Debug) {
        cout << "Event number from header --> " << head.eventNumb << endl;
        cout << " event header size " << sizeof(head) << endl;
        cout << " total rawdata size " << head.eventSize << endl;
        cout << "Payload " << payload << endl;
        getchar();
    }

    fSignalEvent->SetID(head.eventNumb);

    int timeBin = 0;

    int fecN;
    int channel;
    int asicN;
    int physChannel;

    unsigned int eventTime, deltaTime;
    unsigned int ts, th, tl;
    int tempAsic1, tempAsic2, sampleCountRead, pay;
    unsigned short data, dat;

    bool isData = false;

    bool first = true;

    // Bucle till it finds the readed bits equals the payload
    while (frameBits < payload) {
        fread(&pHeader, sizeof(DataPacketHeader), 1, fInputBinFile);
        frameBits += sizeof(DataPacketHeader);

        if (first)  // Timestamping (A. Tomas, 30th June 2011)
        {
            // th = (unsigned int) pHeader.ts_h;  //extend zeros;
            th = ntohs(pHeader.ts_h);
            // cout<<th<<endl;
            // tl = (unsigned int)pHeader.ts_l; //extend zeros;
            tl = ntohs(pHeader.ts_l);
            // cout<<tl<<endl;
            ts = th << (sizeof(unsigned short) * 8);  // shift half a 32-bit word
            // cout<<ts<<endl;
            eventTime = ts | tl;  // concatenation
            // cout<<ts<<endl;

            if (eventTime > prevTime)
                deltaTime = eventTime - prevTime;
            else
                deltaTime = (MAX_UNSIGNED_INT - prevTime) + eventTime;

            reducedTime += deltaTime;

            // Set timestamp and event ID
            fSignalEvent->SetTime(tStart + reducedTime * 2.E-8);  // timeStamp

            // printf("Event time read %.3lf\n",tStart+reducedTime*2.E-8);

            prevTime = eventTime;
            first = false;

            if (this->GetVerboseLevel() == REST_Debug) {
                cout << "Timestamp: " << endl;
                cout << "-----------" << endl;
                cout << "directamente : " << endl;
                // cout<<ntohs(pHeader.ts_h)<<endl;
                // cout<<ntohs(pHeader.ts_l)<<endl;
                cout << pHeader.ts_h << " -> " << ntohs(pHeader.ts_h) << " -> " << th << endl;
                cout << pHeader.ts_l << " -> " << ntohs(pHeader.ts_l) << " -> " << tl << endl;
                cout << "concatenados: " << endl;
                cout << ts << endl;
                cout << eventTime << endl;
                // cout<<ntohl(timestamp)<<endl;
            }
        }

        if (this->GetVerboseLevel() == REST_Debug) {
            cout << "Event data packet header: " << endl;
            cout << "Size " << ntohs(pHeader.size) << endl;

#ifdef NEW_DAQ_T2K_2_X
            cout << "RawDCC Head " << ntohs(pHeader.dcc) << " Version " << GET_EVENT_TYPE(ntohs(pHeader.dcc));
            cout << " Flag " << ((ntohs(pHeader.dcc) & 0x3000) >> 12);
            cout << " RT " << ((ntohs(pHeader.dcc) & 0x0C00) >> 10) << " DCCInd "
                 << ((ntohs(pHeader.dcc) & 0x03F0) >> 4);
            cout << " FEMInd " << (ntohs(pHeader.dcc) & 0x000F) << endl;

            cout << "FEM0Ind " << ntohs(pHeader.hdr) << " Type " << ((ntohs(pHeader.hdr) & 0xF000) >> 12);
            cout << " L " << ((ntohs(pHeader.hdr) & 0x0800) >> 11);
            cout << " U " << ((ntohs(pHeader.hdr) & 0x0800) >> 10) << " FECFlags "
                 << ((ntohs(pHeader.hdr) & 0x03F0) >> 4);
            cout << " Index " << (ntohs(pHeader.hdr) & 0x000F) << endl;

            cout << "RawFEM " << ntohs(pHeader.args) << " M " << ((ntohs(pHeader.args) & 0x8000) >> 15);
            cout << " N " << ((ntohs(pHeader.args) & 0x4000) >> 14) << " Zero "
                 << ((ntohs(pHeader.args) & 0x1000) >> 13);
            cout << " Arg2 " << GET_RB_ARG2(ntohs(pHeader.args)) << " Arg2 "
                 << GET_RB_ARG1(ntohs(pHeader.args)) << endl;
            cout << "TimeStampH " << ntohs(pHeader.ts_h) << endl;
            cout << "TimeStampL " << ntohs(pHeader.ts_l) << endl;
            cout << "RawEvType " << ntohs(pHeader.ecnt) << " EvTy " << GET_EVENT_TYPE(ntohs(pHeader.ecnt));
            cout << " EventCount " << GET_EVENT_COUNT(ntohs(pHeader.ecnt)) << endl;
            cout << "Samples " << ntohs(pHeader.scnt) << endl;
#else
            cout << "Event data packet header: " << endl;
            cout << ntohs(pHeader.size) << endl;
            cout << ntohs(pHeader.hdr) << endl;
            cout << ntohs(pHeader.args) << endl;
            cout << ntohs(pHeader.ts_h) << endl;
            cout << ntohs(pHeader.ts_l) << endl;
            cout << ntohs(pHeader.ecnt) << endl;
            cout << ntohs(pHeader.scnt) << endl;
#endif
        }

        tempAsic1 = GET_RB_ARG1(ntohs(pHeader.args));
        tempAsic2 = GET_RB_ARG2(ntohs(pHeader.args));
        channel = tempAsic1 / 6;
        asicN = (10 * (tempAsic1 % 6) / 2 + tempAsic2) % 4;
        fecN = (10 * (tempAsic1 % 6) / 2 + tempAsic2) / 4;

        if (this->GetVerboseLevel() == REST_Debug) {
            cout << " channel " << channel << " asic " << asicN << " fec " << fecN << endl;
        }

        sampleCountRead = ntohs(pHeader.scnt);
        pay = sampleCountRead % 2;

        physChannel = -10;
        if (channel > 2 && channel < 15) {
            physChannel = channel - 3;
        } else if (channel > 15 && channel < 28) {
            physChannel = channel - 4;
        } else if (channel > 28 && channel < 53) {
            physChannel = channel - 5;
        } else if (channel > 53 && channel < 66) {
            physChannel = channel - 6;
        } else if (channel > 66) {
            physChannel = channel - 7;
        }

        if (physChannel >= 0)  // isThisAphysChannel?
                               // in this case we'd hold it in hEvent
                               // but we need to redefine the physChannel number
                               // so as not to overwrite the information
                               // and to know which ASIC and FEC it belongs to.
        {
            isData = true;
            physChannel = fecN * 72 * 4 + asicN * 72 + physChannel;

        } else
            isData = false;

        timeBin = 0;

        if (sampleCountRead < 9) isData = false;
        for (int i = 0; i < sampleCountRead; i++) {
            fread(&dat, sizeof(unsigned short), 1, fInputBinFile);
            frameBits += sizeof(dat);
            data = ntohs(dat);

            if (this->GetVerboseLevel() == REST_Debug) printBits(data);
            if (((data & 0xFE00) >> 9) == 8) {
                timeBin = GET_CELL_INDEX(data);
                if (timeBin == 511) isData = false;
                if (this->GetVerboseLevel() == REST_Debug) cout << data << " Time bin " << timeBin << endl;
            } else if ((((data & 0xF000) >> 12) == 0) && isData) {
                fSignalEvent->AddChargeToSignal(physChannel, timeBin, data);
                if (this->GetVerboseLevel() == REST_Debug)
                    cout << "Time bin " << timeBin << " ADC: " << data << endl;
                timeBin++;
            }
        }

        if (this->GetVerboseLevel() == REST_Debug) cout << pay << endl;
        if (pay) {
            fread(&dat, sizeof(unsigned short), 1, fInputBinFile);
            frameBits += sizeof(unsigned short);
        }

        fread(&pEnd, sizeof(DataPacketEnd), 1, fInputBinFile);
        frameBits += sizeof(DataPacketEnd);

        if (this->GetVerboseLevel() == REST_Debug) {
            th = ntohs(pEnd.crc1);
            tl = ntohs(pEnd.crc2);
            ts = th << (sizeof(unsigned short) * 8);  // shift half a 32-bit word
            eventTime = ts | tl;

            cout << "Readed "
                 << sampleCountRead * sizeof(unsigned short) + sizeof(DataPacketHeader) +
                        sizeof(DataPacketEnd) + sampleCountRead % 2 * sizeof(unsigned short)
                 << " vs HeadSize " << ntohs(pHeader.size) << " Diff "
                 << ntohs(pHeader.size) - (sampleCountRead + sizeof(DataPacketHeader) +
                                           sizeof(DataPacketEnd) + sampleCountRead % 2)
                 << endl;
            cout << "Trailer_H " << ntohs(pEnd.crc1) << " Trailer_L " << ntohs(pEnd.crc2) << endl;
            cout << "Trailer " << eventTime << "\n" << endl;
        }

    }  // end while
    totalBytesReaded += frameBits;

    // printf("Event ID %d time stored
    // %.3lf\n",fSignalEvent->GetID(),fSignalEvent->GetTime());

    if (this->GetVerboseLevel() == REST_Debug) {
        cout << "End of event " << endl;
        getchar();
    }
    // cout<<"Ev ID "<<fSignalEvent->GetID()<<" " <<endl;

    return fSignalEvent;
}
