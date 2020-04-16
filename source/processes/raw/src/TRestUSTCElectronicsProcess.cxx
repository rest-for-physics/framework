///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestUSTCElectronicsProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestUSTCElectronicsProcess
///             How to use: replace TRestUSTCElectronicsProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________

// int counter = 0;

#include "TRestUSTCElectronicsProcess.h"
using namespace std;
#include <bitset>
#include "TTimeStamp.h"

ClassImp(TRestUSTCElectronicsProcess)
    //______________________________________________________________________________
    TRestUSTCElectronicsProcess::TRestUSTCElectronicsProcess() {
    Initialize();
}

TRestUSTCElectronicsProcess::TRestUSTCElectronicsProcess(char* cfgFileName) { Initialize(); }

//______________________________________________________________________________
TRestUSTCElectronicsProcess::~TRestUSTCElectronicsProcess() {
    // TRestUSTCElectronicsProcess destructor
}

//______________________________________________________________________________
void TRestUSTCElectronicsProcess::Initialize() {
    TRestRawToSignalProcess::Initialize();

    SetSectionName(this->ClassName());
}

void TRestUSTCElectronicsProcess::InitProcess() {
    fEventBuffer.clear();
    errorevents.clear();
    unknownerrors = 0;
    fLastBufferedId = 0;

#ifndef Incoherent_Event_Generation
    nBufferedEvent = StringToInteger(GetParameter("BufferNumber", "2"));
    if (nBufferedEvent < 1) nBufferedEvent = 1;
#else
    nBufferedEvent = 2;
#endif  // !Incoherent_Readout

    for (int n = 0; n < nBufferedEvent + 1; n++) {
        fEventBuffer.push_back(vector<USTCDataFrame>());
    }

    fRunOrigin = fRunInfo->GetRunNumber();
    fCurrentFile = 0;
    fCurrentBuffer = 0;
    totalBytesReaded = 0;

    USTCDataFrame frame;
    if ((!GetNextFrame(frame)) || (!ReadFrameData(frame))) {
        FixToNextFrame(fInputFiles[fCurrentFile]);
        if ((!GetNextFrame(frame)) || (!ReadFrameData(frame))) {
            ferr << "TRestUSTCElectronicsProcess: Failed to read the first data frame in file, may be wrong "
                    "input?"
                 << endl;
            exit(1);
        }
    }

    fCurrentEvent = frame.evId;
    AddBuffer(frame);

    if (fCurrentEvent != 0) {
        warning << "TRestUSTCElectronicsProcess : first event is not with id 0 !" << endl;
        warning << "The first Id is " << fCurrentEvent << ". May be input file not the first file?" << endl;
    }
}

TRestEvent* TRestUSTCElectronicsProcess::ProcessEvent(TRestEvent* evInput) {
    while (1) {
        if (EndReading()) {
            return NULL;
        }
        if (!FillBuffer()) {
            fSignalEvent->SetOK(false);
        }
        if (fEventBuffer[fCurrentBuffer].size() == 0) {
            debug << "Blank event " << fCurrentEvent << " !" << endl;
            fCurrentEvent++;
            ClearBuffer();
        } else {
            break;
        }
    }

    fSignalEvent->Initialize();
    fSignalEvent->SetID(fCurrentEvent);

    debug << "------------------------------------" << endl;
    debug << "Generating event with ID: " << fCurrentEvent << endl;

    // some event level operation
    USTCDataFrame* frame0 = &fEventBuffer[fCurrentBuffer][0];
    TTimeStamp tSt = 0;
    Long64_t evtTime = frame0->eventTime;
    tSt.SetNanoSec((fTimeOffset + evtTime) % ((Long64_t)1e9));
    tSt.SetSec((fTimeOffset + evtTime) / ((Long64_t)1e9));

    // some signal level operation
    for (int i = 0; i < fEventBuffer[fCurrentBuffer].size(); i++) {
        USTCDataFrame* frame = &fEventBuffer[fCurrentBuffer][i];
        if (frame->evId == fCurrentEvent && frame->eventTime == evtTime) {
            sgnl.Initialize();
            sgnl.SetSignalID(frame->signalId);
            for (int j = 0; j < 512; j++) {
                sgnl.AddPoint((Short_t)frame->dataPoint[j]);
            }
            fSignalEvent->AddSignal(sgnl);

            debug << "AsAdId, AgetId, chnId, max value: " << frame->boardId << ", " << frame->chipId << ", "
                  << frame->channelId << ", " << sgnl.GetMaxValue() << endl;

        } else {
            warning << "TRestUSTCElectronicsProcess : unmatched signal frame!" << endl;
            warning << "ID (supposed, received): " << fCurrentEvent << ", " << frame->evId << endl;
            warning << "Time (supposed, received) : " << evtTime << ", " << frame->eventTime << endl;
            warning << endl;
            fSignalEvent->SetOK(false);
            fCurrentEvent++;
            ClearBuffer();
            return fSignalEvent;
        }
    }

    ClearBuffer();

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "------------------------------------" << endl;
        GetChar();
    }
    fSignalEvent->SetTimeStamp(tSt);
    fSignalEvent->SetRunOrigin(fRunOrigin);
    fSignalEvent->SetSubRunOrigin(fSubRunOrigin);

    // cout << fSignalEvent->GetNumberOfSignals() << endl;
    // if( fSignalEvent->GetNumberOfSignals( ) == 0 ) return NULL;
    fCurrentEvent++;

    return fSignalEvent;
}

void TRestUSTCElectronicsProcess::EndProcess() {
    for (int i = 0; i < errorevents.size(); i++) {
        warning << "Event " << errorevents[i] << " contains error !" << endl;
    }
    if (errorevents.size() > 0 && unknownerrors > 0) {
        warning << "There are also " << unknownerrors << " errors from unknown events! " << endl;
    } else if (unknownerrors > 0) {
        warning << "There are " << unknownerrors << " errors from unknown events! " << endl;
    }

    errorevents.clear();
}

bool TRestUSTCElectronicsProcess::FillBuffer() {
#ifdef Incoherent_Event_Generation
    bool unknowncurrentevent = false;
    if (fEventBuffer[fCurrentBuffer].size() > 0) {
        fCurrentEvent = fEventBuffer[fCurrentBuffer][0].evId;
    } else {
        unknowncurrentevent = true;
    }

    while (1)
#else
    while (fLastBufferedId < fCurrentEvent + (fEventBuffer.size() - 1) / 2)
#endif
    {
        bool errortag = false;
        bool breaktag = false;
        USTCDataFrame frame;
        if (!GetNextFrame(frame)) {
            break;
        }
        if (!ReadFrameData(frame)) {
            warning << "error reading frame data in file " << fCurrentFile << endl;
            FixToNextFrame(fInputFiles[fCurrentFile]);
            GetNextFrame(frame);
            ReadFrameData(frame);
            errortag = true;
        }
#ifdef Incoherent_Event_Generation
        if (unknowncurrentevent) {
            cout << frame.evId << endl;
            fCurrentEvent = frame.evId;
            unknowncurrentevent = false;
        }

        if (frame.evId != fCurrentEvent) {
            breaktag = true;
        }
#else
        if (frame.evId >= fCurrentEvent + (fEventBuffer.size() - 1) / 2) {
            breaktag = true;
        }
#endif  // Incoherent_Event_Generation

        if (!AddBuffer(frame)) {
            errortag = true;
        }

        if (errortag) {
            if (frame.evId != -1) {
                if (errorevents.size() == 0) {
                    errorevents.push_back(frame.evId);
                } else {
                    for (int i = 0; i < errorevents.size(); i++) {
                        if (errorevents[i] == frame.evId) {
                            break;
                        } else if (i == errorevents.size() - 1) {
                            errorevents.push_back(frame.evId);
                            break;
                        }
                    }
                }

            } else {
                unknownerrors++;
            }
        }

        if (breaktag) {
            fLastBufferedId = frame.evId;
            break;
        }
    }
    for (int i = 0; i < errorevents.size(); i++) {
        if (errorevents[i] == fCurrentEvent) return false;
    }
    return true;
}

bool TRestUSTCElectronicsProcess::OpenNextFile(USTCDataFrame& frame) {
    if (fCurrentFile < fInputFiles.size() - 1)  // try to get frame form next file
    {
        fCurrentFile++;
        return GetNextFrame(frame);
    } else {
        return false;
    }
}

bool TRestUSTCElectronicsProcess::GetNextFrame(USTCDataFrame& frame) {
    if (fInputFiles[fCurrentFile] == NULL) {
        return OpenNextFile(frame);
    }
#ifdef V4_Readout_Format
    while (1) {
        UChar_t Protocol[PROTOCOL_SIZE];
        if (fread(Protocol, PROTOCOL_SIZE, 1, fInputFiles[fCurrentFile]) != 1 ||
            feof(fInputFiles[fCurrentFile])) {
            fclose(fInputFiles[fCurrentFile]);
            fInputFiles[fCurrentFile] = NULL;
            return OpenNextFile(frame);
        }
        totalBytesReaded += PROTOCOL_SIZE;

        if (!(Protocol[0] ^ 0xac) && !(Protocol[1] ^ 0x0f)) {
            // the first 2 bytes must be 0xac0f, otherwise it is wrong

            int flag = Protocol[2] >> 5;
            if (flag & 0x1) {
                // this is the evt_ending frame
                memcpy(fEnding, Protocol, PROTOCOL_SIZE);
                if (fread(fEnding + PROTOCOL_SIZE, ENDING_SIZE - PROTOCOL_SIZE, 1,
                          fInputFiles[fCurrentFile]) != 1 ||
                    feof(fInputFiles[fCurrentFile])) {
                    fclose(fInputFiles[fCurrentFile]);
                    fInputFiles[fCurrentFile] = NULL;
                    return OpenNextFile(frame);
                }
                totalBytesReaded += ENDING_SIZE;
            } else if (flag & 0x2) {
                // this is the evt_header frame
                memcpy(fHeader, Protocol, PROTOCOL_SIZE);
                if (fread(fHeader + PROTOCOL_SIZE, HEADER_SIZE - PROTOCOL_SIZE, 1,
                          fInputFiles[fCurrentFile]) != 1 ||
                    feof(fInputFiles[fCurrentFile])) {
                    fclose(fInputFiles[fCurrentFile]);
                    fInputFiles[fCurrentFile] = NULL;
                    return OpenNextFile(frame);
                }
                totalBytesReaded += HEADER_SIZE;
            } else {
                // this is the evt_data frame
                memcpy(frame.data, Protocol, PROTOCOL_SIZE);
                if (fread(frame.data + PROTOCOL_SIZE, DATA_SIZE - PROTOCOL_SIZE, 1,
                          fInputFiles[fCurrentFile]) != 1 ||
                    feof(fInputFiles[fCurrentFile])) {
                    fclose(fInputFiles[fCurrentFile]);
                    fInputFiles[fCurrentFile] = NULL;
                    return OpenNextFile(frame);
                }
                totalBytesReaded += DATA_SIZE;
                return true;
            }
        } else {
            return false;
        }
    }
#else
    if (fread(frame.data, DATA_SIZE, 1, fInputFiles[fCurrentFile]) != 1 || feof(fInputFiles[fCurrentFile])) {
        fclose(fInputFiles[fCurrentFile]);
        fInputFiles[fCurrentFile] = NULL;
        return OpenNextFile(frame);
    }
    totalBytesReaded += DATA_SIZE;

    if (frame.data[0] * 0x100 + frame.data[1] != 0xEEEE) {
        warning << "wrong header!" << endl;
        return false;
    }
#endif  // V4_Readout_Format

    return true;
}

// it find the next flag of frame, e.g. 0xffff or 0xac0f
void TRestUSTCElectronicsProcess::FixToNextFrame(FILE* f) {
    if (f == NULL) return;
    UChar_t buffer[PROTOCOL_SIZE];
    int n = 0;
    while (1) {
        if (fread(buffer, PROTOCOL_SIZE, 1, f) != 1 || feof(f)) {
            return;
        }
        n += PROTOCOL_SIZE;
#ifdef V4_Readout_Format
        if (!(buffer[0] ^ 0xac) && !(buffer[1] ^ 0x0f)) {
            int flag = buffer[2] >> 5;
            if (flag & 0x2) {
                // we have meet the next event header
                memcpy(fHeader, buffer, PROTOCOL_SIZE);
                if (fread(fHeader + PROTOCOL_SIZE, HEADER_SIZE - PROTOCOL_SIZE, 1,
                          fInputFiles[fCurrentFile]) != 1 ||
                    feof(fInputFiles[fCurrentFile])) {
                    fclose(f);
                    f = NULL;
                    break;
                }
                n += HEADER_SIZE;
                warning << "successfully switched to next frame ( + " << n << " byte)" << endl;
                warning << endl;
                break;
            }
        }
#else
        if (!(buffer[0] ^ 0xff) && !(buffer[1] ^ 0xff) && !(buffer[2] ^ 0xff) && !(buffer[3] ^ 0xff)) {
            warning << "successfully switched to next frame ( + " << n << " byte)" << endl;
            warning << endl;
            break;
        }
#endif
    }
    totalBytesReaded += n;
}

bool TRestUSTCElectronicsProcess::ReadFrameData(USTCDataFrame& frame) {
#ifdef V3_Readout_Format_Long

    // EEEE | E0A0 | 246C 0686 4550 504E | 0001 | 2233 4455 6677 | (A098)(A09C)... | FFFF FFFF
    // 0~1header | 2~3board number | 4~11event time | 12~13channel id(0~63)
    // | 14~19event id | [chip id + data(0~4095)]*512 | ending
    frame.boardId = frame.data[2] & 0x0F;
    frame.chipId = (frame.data[3] & 0xF0) / 16 - 10;
    frame.readoutType = frame.data[3] & 0x0F;
    Long64_t tmp = (Long64_t)frame.data[5] * 0x10000 + (Long64_t)frame.data[6] * 0x100 +
                   (Long64_t)frame.data[7];  // we omit the first byte in case the number is too large
    frame.eventTime = tmp * 0x100000000 + (Long64_t)frame.data[8] * 0x1000000 +
                      (Long64_t)frame.data[9] * 0x10000 + (Long64_t)frame.data[10] * 0x100 +
                      (Long64_t)frame.data[11];
    frame.channelId = frame.data[12] * 0x100 + frame.data[13];
    frame.evId = (frame.data[16] & 0x7F) * 0x1000000 + frame.data[17] * 0x10000 + frame.data[18] * 0x100 +
                 frame.data[19];  // we omit the first 17 bits in case the number
                                  // is too large

    frame.signalId = frame.boardId * 4 * 64 + frame.chipId * 64 + frame.channelId;
#endif

#ifdef V3_Readout_Format_Short
    // EEEE | E0A0 | 246C 0686 | 0001 | 2233 | (A098)(A09C)... | FFFF
    // 0~1header | 2~3board number | 4~7event time | 8~9channel id(0~63) |
    // 10~11event id | [chip id + data(0~4095)]*512 | ending
    frame.boardId = frame.data[2] & 0x0F;
    frame.chipId = (frame.data[3] & 0xF0) / 16 - 10;
    frame.readoutType = frame.data[3] & 0x0F;
    Long64_t tmp = (Long64_t)frame.data[4] * 0x1000000 + (Long64_t)frame.data[5] * 0x10000 +
                   (Long64_t)frame.data[6] * 0x100 + (Long64_t)frame.data[7];
    frame.eventTime = tmp;
    frame.channelId = frame.data[8] * 0x100 + frame.data[9];
    frame.evId = frame.data[10] * 256 + frame.data[11];

    frame.signalId = frame.boardId * 4 * 64 + frame.chipId * 64 + frame.channelId;

#endif  // Long_Readout_Format

#ifdef V4_Readout_Format

    // the evt header frame
    // AC0F        | 401C         | 0300                                     | 010A 3140 0000            |
    // 0500 0000               | .... | .... | 8E95 B452 0~1Protocol | 2~3 010+size | 4~5:
    // 00000011+ETYPE(2)+ST(1)+SOURCEID(5) | 6~11 time stamp(inverted) | 12~15 event id(inverted) | not used
    // | ending
    frame.readoutType = fHeader[5] & 0xc0;

    int t_high = fHeader[10] * 0x100 + fHeader[11];
    int t_mid = fHeader[8] * 0x100 + fHeader[9];
    int t_low = fHeader[6] * 0x100 + fHeader[7];
    Long64_t tmp = (Long64_t)t_high * 0x100000000 + (Long64_t)t_mid * 0x10000 + (Long64_t)t_low;
    frame.eventTime = tmp;

    int id_high = fHeader[14] * 0x100 + fHeader[15];
    int id_low = fHeader[12] * 0x100 + fHeader[13];
    frame.evId = id_high * 0x10000 + id_low;

    // the signal frame
    // AC0F        | 0404         | C000                               | (3163)(316C)...         | 0000 BCEB
    // 5742 0~1Protocol | 2~3 not used | 4~5: 11+card(5)+chip(2)+channel(7) | [0011+data(0~4095)]*512 | ending
    // event info(time, id, etc.) is in event header
    frame.boardId = frame.data[4] & 0x3e;
    frame.chipId = (frame.data[4] & 0x01) * 2 + (frame.data[5] >> 7);
    frame.channelId = frame.data[5] & 0x7f;

    frame.signalId = frame.boardId * 4 * 64 + frame.chipId * 68 + frame.channelId;
#endif

    // sampling point data
    for (int i = 0; i < 512; i++) {
        int pos = i * 2 + DATA_OFFSET;
        frame.dataPoint[i] = (int)((frame.data[pos] & 0x0F) * 0x100 + frame.data[pos + 1]);
    }

    // if (frame.data[DATA_SIZE - 4] * 0x1000000 + frame.data[DATA_SIZE - 3] * 0x10000 +
    //	frame.data[DATA_SIZE - 2] * 0x100 + frame.data[DATA_SIZE - 1] !=
    //	0xFFFFFFFF) {
    //	warning << "wrong ending of frame! Event Id : " << frame.evId << " Channel Id : " << frame.channelId
    //		<< endl;
    //	return false;
    //}

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    return true;
}

bool TRestUSTCElectronicsProcess::AddBuffer(USTCDataFrame& frame) {
#ifdef Incoherent_Event_Generation
    if (frame.evId == fCurrentEvent) {
        fEventBuffer[fCurrentBuffer].push_back(frame);
    } else {
        int pos = 1 + fCurrentBuffer;
        if (pos >= fEventBuffer.size()) pos -= fEventBuffer.size();
        fEventBuffer[pos].push_back(frame);
    }
#else
    if (frame.evId >= fCurrentEvent + fEventBuffer.size()) {
        warning << "too large event id for buffering!" << endl;
        warning << "this may due to the inconherence of event id. Increase the "
                   "buffer number!"
                << endl;
        warning << "Current Event, Burrfering event : " << fCurrentEvent << ", " << frame.evId << endl;
        return false;
    }
    if (frame.evId < fCurrentEvent) {
        warning << "skipping a signal from old event!" << endl;
        warning << "the cause may be that too much events are mixing. Increase the "
                   "buffer number!"
                << endl;
        warning << "Current Event, Burrfering event : " << fCurrentEvent << ", " << frame.evId << endl;
        return false;
    }
    int pos = frame.evId - fCurrentEvent + fCurrentBuffer;
    if (pos >= fEventBuffer.size()) pos -= fEventBuffer.size();
    fEventBuffer[pos].push_back(frame);
#endif

    return true;
}

void TRestUSTCElectronicsProcess::ClearBuffer() {
    fEventBuffer[fCurrentBuffer].clear();
    fCurrentBuffer += 1;
    if (fCurrentBuffer >= fEventBuffer.size()) {
        fCurrentBuffer -= fEventBuffer.size();
    }
}

Bool_t TRestUSTCElectronicsProcess::EndReading() {
    for (int n = 0; n < nFiles; n++) {
        if (fInputFiles[n] != NULL) return kFALSE;
    }
    for (int n = 0; n < fEventBuffer.size(); n++) {
        if (fEventBuffer[n].size() != 0) return kFALSE;
    }

    return kTRUE;
}
