///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMultiFEMINOSToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestMultiFEMINOSToSignalProcess
///             How to use: replace TRestMultiFEMINOSToSignalProcess by your
///             name, fill the required functions following instructions and add
///             all needed additional members and funcionality
///
///             Aug 2017:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

// int counter = 0;

// Prefix Codes for 8-bit data content
//
#define PFX_8_BIT_CONTENT_MASK 0xFF00  // Mask to select 8 MSB's of prefix
#define PFX_ASCII_MSG_LEN 0x0100       // ASCII message length
//
// Prefix Codes for 9-bit data content
//
#define PFX_9_BIT_CONTENT_MASK 0xFE00  // Mask to select 7 MSB's of prefix
#define PFX_TIME_BIN_IX 0x0E00         // Time-bin Index
#define PFX_HISTO_BIN_IX 0x0C00        // Histogram bin Index
#define PFX_PEDTHR_LIST 0x0A00         // List of pedestal or thresholds
#define PFX_START_OF_DFRAME 0x0800     // Start of Data Frame + 5 bit source + 4 bit Version
#define PFX_START_OF_MFRAME 0x0600     // Start of Monitoring Frame + 4 bit Version + 5 bit source
#define PFX_START_OF_CFRAME 0x0400     // Start of Configuration Frame + 4 bit Version + 5 bit source
//
// Macros to act on 8-bit data content
//
#define GET_ASCII_LEN(w) (((w)&0x00FF) >> 0)
#define PUT_ASCII_LEN(w) (PFX_ASCII_MSG_LEN | ((w)&0x00FF))
//
// Prefix Codes for 0-bit data content
//
#define PFX_0_BIT_CONTENT_MASK 0xFFFF    // Mask to select 16 MSB's of prefix
#define PFX_END_OF_FRAME 0x000F          // End of Frame (any type)
#define PFX_DEADTIME_HSTAT_BINS 0x000E   // Deadtime statistics and histogram
#define PFX_PEDESTAL_HSTAT 0x000D        // Pedestal histogram statistics
#define PFX_PEDESTAL_H_MD 0x000C         // Pedestal histogram Mean and Deviation
#define PFX_SHISTO_BINS 0x000B           // Hit S-curve histogram
#define PFX_CMD_STATISTICS 0x000A        // Command server statistics
#define PFX_START_OF_BUILT_EVENT 0x0009  // Start of built event
#define PFX_END_OF_BUILT_EVENT 0x0008    // End of built event
#define PFX_EVPERIOD_HSTAT_BINS 0x0007   // Inter Event Time statistics and histogram
#define PFX_SOBE_SIZE 0x0006             // Start of built event + Size
#define PFX_NULL_CONTENT 0x0000          // Null content
//
// Prefix Codes for 14-bit data content
//
#define PFX_14_BIT_CONTENT_MASK 0xC000     // Mask to select 2 MSB's of prefix
#define PFX_CARD_CHIP_CHAN_HIT_IX 0xC000   // Index of Card, Chip and Channel Hit
#define PFX_CARD_CHIP_CHAN_HIT_CNT 0x8000  // Nb of Channel hit for given Card and Chip
#define PFX_CARD_CHIP_CHAN_HISTO 0x4000    // Pedestal Histogram for given Card and Chip
//
// Prefix Codes for 12-bit data content
//
#define PFX_12_BIT_CONTENT_MASK 0xF000  // Mask to select 4 MSB's of prefix
#define PFX_ADC_SAMPLE 0x3000           // ADC sample
#define PFX_LAT_HISTO_BIN 0x2000        // latency or inter event time histogram bin
#define PFX_CHIP_LAST_CELL_READ 0x1000  // Chip index and last cell read
//
// Prefix Codes for 4-bit data content
//
#define PFX_4_BIT_CONTENT_MASK 0xFFF0  // Mask to select 12 MSB's of prefix
#define PFX_START_OF_EVENT 0x00F0      // Start of Event + 1 bit free + Event Trigger Type
#define PFX_END_OF_EVENT 0x00E0        // End of Event + 4 MSB of size

//
// Macros to act on 4-bit data content
//
#define GET_EVENT_TYPE(w) (((w)&0x0007) >> 0)
#define GET_EOE_SIZE(w) (((w)&0x000F) >> 0)

//
// Macros to extract 14-bit data content
//
#define GET_CARD_IX(w) (((w)&0x3E00) >> 9)
#define GET_CHIP_IX(w) (((w)&0x0180) >> 7)
#define GET_CHAN_IX(w) (((w)&0x007F) >> 0)

//
// Macros to extract 12-bit data content
//
#define GET_ADC_DATA(w) (((w)&0x0FFF) >> 0)
#define GET_LAT_HISTO_BIN(w) (((w)&0x0FFF) >> 0)
#define PUT_LAT_HISTO_BIN(w) (PFX_LAT_HISTO_BIN | (((w)&0x0FFF) >> 0))
#define GET_LST_READ_CELL(w) (((w)&0x03FF) >> 0)
#define GET_LST_READ_CELL_CHIP_IX(w) (((w)&0x0C00) >> 10)

//
// Macros to extract 9-bit data content
//
#define GET_TIME_BIN(w) (((w)&0x01FF) >> 0)
#define GET_HISTO_BIN(w) (((w)&0x01FF) >> 0)
#define GET_PEDTHR_LIST_FEM(w) (((w)&0x01F0) >> 4)
#define GET_PEDTHR_LIST_ASIC(w) (((w)&0x000C) >> 2)
#define GET_PEDTHR_LIST_MODE(w) (((w)&0x0002) >> 1)
#define GET_PEDTHR_LIST_TYPE(w) (((w)&0x0001) >> 0)
#define PUT_FVERSION_FEMID(w, fv, id) (((w)&0xFE00) | (((fv)&0x0003) << 7) | (((id)&0x001F) << 0))
#define GET_FRAMING_VERSION(w) (((w)&0x0180) >> 7)
#define GET_FEMID(w) (((w)&0x001F) >> 0)

// Definition of verboseness flags used by MFrame_Print
#define FRAME_PRINT_ALL 0x00000001
#define FRAME_PRINT_SIZE 0x00000002
#define FRAME_PRINT_HIT_CH 0x00000004
#define FRAME_PRINT_HIT_CNT 0x00000008
#define FRAME_PRINT_CHAN_DATA 0x00000010
#define FRAME_PRINT_HISTO_BINS 0x00000020
#define FRAME_PRINT_ASCII 0x00000040
#define FRAME_PRINT_FRBND 0x00000080
#define FRAME_PRINT_EVBND 0x00000100
#define FRAME_PRINT_NULLW 0x00000200
#define FRAME_PRINT_HISTO_STAT 0x00000400
#define FRAME_PRINT_LISTS 0x00000800
#define FRAME_PRINT_LAST_CELL_READ_0 0x00001000
#define FRAME_PRINT_LAST_CELL_READ_1 0x00002000
#define FRAME_PRINT_LAST_CELL_READ_2 0x00004000
#define FRAME_PRINT_LAST_CELL_READ_3 0x00008000
#define FRAME_PRINT_EBBND 0x00010000

#define MAX_FRAME_SIZE 8192

// Maximum event size is:
// 24 Feminos * 4 ASICs * 80 channels * 512 time-bins * 2 bytes = 7.5 MByte

#define MAX_EVENT_SIZE (24 * 4 * 80 * 512 * 2)

#define ORIGINAL_MCLIENT 0
unsigned char cur_fr[MAX_EVENT_SIZE];  // Current frame

#include "TRestMultiFEMINOSToSignalProcess.h"
using namespace std;
#include "TTimeStamp.h"

Int_t nChannels = 0;

ClassImp(TRestMultiFEMINOSToSignalProcess);
//______________________________________________________________________________
TRestMultiFEMINOSToSignalProcess::TRestMultiFEMINOSToSignalProcess() { Initialize(); }

TRestMultiFEMINOSToSignalProcess::TRestMultiFEMINOSToSignalProcess(char* cfgFileName)
    : TRestRawToSignalProcess(cfgFileName) {
    Initialize();
}

//______________________________________________________________________________
TRestMultiFEMINOSToSignalProcess::~TRestMultiFEMINOSToSignalProcess() {
    // TRestMultiFEMINOSToSignalProcess destructor
}

//______________________________________________________________________________
void TRestMultiFEMINOSToSignalProcess::LoadDetectorSetupData() {
    if (fRunInfo == nullptr) {
        cout << "'fRunInfo' is null" << endl;
        return;
    }
    string file_name = (string)fRunInfo->GetInputFileNamepattern();
    gDetector->RegisterString(file_name);
    fRunOrigin = gDetector->GetRunNumber();
}

//______________________________________________________________________________
void TRestMultiFEMINOSToSignalProcess::Initialize() {
    fLastEventId = 0;
    fLastTimeStamp = 0;
}

//______________________________________________________________________________
void TRestMultiFEMINOSToSignalProcess::InitProcess() {
    debug << "TRestMultiFeminos::InitProcess" << endl;
    // Reading binary file header

    LoadDetectorSetupData();

    unsigned short sh;
    unsigned short al;

    totalBytesReaded = 0;

    // Read prefix
    if (fread(&sh, sizeof(unsigned short), 1, fInputBinFile) != 1) {
        printf("Error: could not read first prefix.\n");
        exit(1);
    }
    totalBytesReaded += sizeof(unsigned short);

    // f->tot_file_rd+= sizeof(unsigned short);

    // This must be the prefix for an ASCII string
    if ((sh & PFX_8_BIT_CONTENT_MASK) != PFX_ASCII_MSG_LEN) {
        printf("Error: missing string prefix in 0x%x\n", sh);
        exit(1);
    }
    al = GET_ASCII_LEN(sh);

    if (!ORIGINAL_MCLIENT) {
        int tt;
        fread(&tt, sizeof(int), 1, fInputBinFile);
        totalBytesReaded += sizeof(int);

        tStart = tt;
        debug << "Timestamp : " << tt << " - " << tStart << endl;
    }

    if (ORIGINAL_MCLIENT) {
        char run_str[256];
        // Read Run information string
        if (fread(&(run_str[0]), sizeof(char), al, fInputBinFile) != al) {
            printf("Error: could not read %d characters.\n", al);
            exit(1);
        }
        totalBytesReaded += sizeof(char);

        // Show run string information if desired
        printf("Run string: %s\n", &(run_str[0]));
    }
}

//______________________________________________________________________________
TRestEvent* TRestMultiFEMINOSToSignalProcess::ProcessEvent(TRestEvent* evInput) {
    if (GetVerboseLevel() >= REST_Debug) cout << "TRestMultiFEMINOSToSignalProcess::ProcessEvent" << endl;

    while (1) {
        unsigned short* sh;
        sh = (unsigned short*)&(cur_fr[2]);
        unsigned int nb_sh;
        int fr_sz;
        int fr_offset;
        int done;

        nChannels = 0;
        Bool_t endOfEvent = false;

        fSignalEvent->Initialize();

        fSignalEvent->SetRunOrigin(fRunOrigin);
        fSignalEvent->SetSubRunOrigin(fSubRunOrigin);

        while (!endOfEvent) {
            done = 0;
            while (!done) {
                // Read one short word
                if (fread(sh, sizeof(unsigned short), 1, fInputBinFile) != 1) {
                    debug << "End of file reached." << endl;

                    // The processing thread will be finished when return NULL is reached
                    return NULL;
                }
                totalBytesReaded += sizeof(unsigned short);

                if ((*sh & PFX_0_BIT_CONTENT_MASK) == PFX_START_OF_BUILT_EVENT) {
                    if (GetVerboseLevel() >= REST_Debug) printf("***** Start of Built Event *****\n");
                    if (GetVerboseLevel() >= REST_Debug) GetChar();
                } else if ((*sh & PFX_0_BIT_CONTENT_MASK) == PFX_END_OF_BUILT_EVENT) {
                    if (GetVerboseLevel() >= REST_Debug) printf("***** End of Built Event *****\n\n");
                    if (GetVerboseLevel() >= REST_Debug) GetChar();
                    endOfEvent = true;
                    done = 1;
                } else if ((*sh & PFX_0_BIT_CONTENT_MASK) == PFX_SOBE_SIZE) {
                    // Read two short words to get the size of the event
                    if (fread((sh + 1), sizeof(unsigned short), 2, fInputBinFile) != 2) {
                        printf("Error: could not read two short words.\n");
                        exit(1);
                    }
                    totalBytesReaded += sizeof(unsigned short) * 2;

                    // Get the size of the event in bytes
                    fr_sz = (int)(((*(sh + 2)) << 16) | (*(sh + 1)));

                    // Compute the number of short words to read for the complete event
                    nb_sh = fr_sz / 2;  // number of short words is half the event size in bytes
                    nb_sh -= 3;         // we have already read three short words from this event
                    fr_offset = 8;

                    done = 1;
                } else if (((*sh & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_DFRAME) ||
                           ((*sh & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_CFRAME) ||
                           ((*sh & PFX_9_BIT_CONTENT_MASK) == PFX_START_OF_MFRAME)) {
                    // Read one short word
                    if (fread((sh + 1), sizeof(unsigned short), 1, fInputBinFile) != 1) {
                        printf("Error: could not read short word.\n");
                        exit(1);
                    }
                    totalBytesReaded += sizeof(unsigned short);

                    // Get the size of the event in bytes
                    fr_sz = (int)*(sh + 1);

                    // Compute the number of short word to read for this frame
                    nb_sh = fr_sz / 2;  // number of short words is half the frame size in bytes
                    nb_sh -= 2;         // we have already read two short words from this frame
                    fr_offset = 6;

                    done = 1;
                } else {
                    printf("Error: cannot interpret short word 0x%x\n", *sh);
                    exit(1);
                }
            }

            // Read binary frame
            if (!endOfEvent) {
                if (fread(&(cur_fr[fr_offset]), sizeof(unsigned short), nb_sh, fInputBinFile) != nb_sh) {
                    printf("Error: could not read %d bytes.\n", (nb_sh * 2));
                    exit(1);
                }
                totalBytesReaded += sizeof(unsigned short) * nb_sh;

                // Zero the first two bytes because these are no longer used to specify
                // the size of the frame
                cur_fr[0] = 0x00;
                cur_fr[1] = 0x00;

                endOfEvent = ReadFrame((void*)&(cur_fr[2]), fr_sz);
            }
        }

        if (fSignalEvent->GetID() == 0 && fLastEventId != 0) {
            fSignalEvent->SetID(fLastEventId);
            fSignalEvent->SetTime(fLastTimeStamp);
            fLastEventId = 0;
        }

        if (GetVerboseLevel() >= REST_Info) {
            cout << "------------------------------------------" << endl;
            cout << "Event ID : " << fSignalEvent->GetID() << endl;
            cout << "Time stamp : " << fSignalEvent->GetTimeStamp() << endl;
            cout << "Number of Signals : " << fSignalEvent->GetNumberOfSignals() << endl;
            cout << "------------------------------------------" << endl;

            if (GetVerboseLevel() >= REST_Debug) {
                for (Int_t n = 0; n < fSignalEvent->GetNumberOfSignals(); n++)
                    cout << "Signal N : " << n << " daq id : " << fSignalEvent->GetSignal(n)->GetID() << endl;
                GetChar();
            }
        }

        if (fSignalEvent->GetNumberOfSignals() != 0) {
            return fSignalEvent;
        } else {
            warning << "blank event " << fSignalEvent->GetID() << "! skipping..." << endl;
        }
    }

    // The processing thread will be finished if return NULL is reached
    return NULL;
}

Bool_t TRestMultiFEMINOSToSignalProcess::ReadFrame(void* fr, int fr_sz) {
    Bool_t endOfEvent = false;

    unsigned short* p;
    int done = 0;
    unsigned short r0, r1, r2;
    unsigned short n0, n1;
    unsigned short cardNumber, chipNumber, daqChannel;
    unsigned int tmp;
    int tmp_i[10];
    int si;

    p = (unsigned short*)fr;

    done = 0;
    si = 0;

    if (GetVerboseLevel() >= REST_Debug) printf("ReadFrame: Frame payload: %d bytes\n", fr_sz);

    Int_t showSamples = fShowSamples;

    TRestRawSignal sgnl;
    sgnl.SetSignalID(-1);
    while (!done) {
        // Is it a prefix for 14-bit content?
        if ((*p & PFX_14_BIT_CONTENT_MASK) == PFX_CARD_CHIP_CHAN_HIT_IX) {
            if (sgnl.GetSignalID() >= 0 && sgnl.GetNumberOfPoints() >= fMinPoints)
                fSignalEvent->AddSignal(sgnl);

            cardNumber = GET_CARD_IX(*p);
            chipNumber = GET_CHIP_IX(*p);
            daqChannel = GET_CHAN_IX(*p);

            if (daqChannel >= 0) {
                daqChannel += cardNumber * 4 * 72 + chipNumber * 72;
                nChannels++;
            }

            if (GetVerboseLevel() >= REST_Debug)
                printf("ReadFrame: Card %02d Chip %01d Daq Channel %02d\n", cardNumber, chipNumber,
                       daqChannel);
            p++;
            si = 0;

            sgnl.Initialize();
            sgnl.SetSignalID(daqChannel);

        }
        // Is it a prefix for 12-bit content?
        else if ((*p & PFX_12_BIT_CONTENT_MASK) == PFX_ADC_SAMPLE) {
            r0 = GET_ADC_DATA(*p);
            if (GetVerboseLevel() >= REST_Debug) {
                if (showSamples > 0) printf("ReadFrame: %03d 0x%04x (%4d)\n", si, r0, r0);
                showSamples--;
            }
            if (sgnl.GetSignalID() >= 0) sgnl.AddPoint((Short_t)r0);
            p++;
            si++;
        }
        // Is it a prefix for 4-bit content?
        else if ((*p & PFX_4_BIT_CONTENT_MASK) == PFX_START_OF_EVENT) {
            r0 = GET_EVENT_TYPE(*p);
            if (GetVerboseLevel() >= REST_Debug) printf("ReadFrame: -- Start of Event (Type %01d) --\n", r0);
            p++;

            // Time Stamp lower 16-bit
            r0 = *p;
            p++;

            // Time Stamp middle 16-bit
            r1 = *p;
            p++;

            // Time Stamp upper 16-bit
            r2 = *p;
            p++;

            if (GetVerboseLevel() >= REST_Debug) {
                printf("ReadFrame: Time 0x%04x 0x%04x 0x%04x\n", r2, r1, r0);
                printf("Timestamp: 0x%04x 0x%04x 0x%04x\n", r2, r1, r0);
                cout << "TimeStamp " << tStart + (2147483648 * r2 + 32768 * r1 + r0) * 2e-8 << endl;
            }

            // Set timestamp and event ID

            // Event Count lower 16-bit
            n0 = *p;
            p++;

            // Event Count upper 16-bit
            n1 = *p;
            p++;

            tmp = (((unsigned int)n1) << 16) | ((unsigned int)n0);
            if (GetVerboseLevel() >= REST_Info) printf("ReadFrame: Event_Count 0x%08x (%d)\n", tmp, tmp);

            // Some times the end of the frame contains the header of the next event.
            // Then, in the attempt to read the header of next event, we must avoid
            // that it overwrites the already assigned id. In that case (id != 0), we
            // do nothing, and we store the values at fLastXX variables, that we will
            // use that for next event.
            if (fSignalEvent->GetID() == 0) {
                if (fLastEventId == 0) {
                    fSignalEvent->SetID(tmp);
                    fSignalEvent->SetTime(tStart + (2147483648 * r2 + 32768 * r1 + r0) * 2e-8);
                } else {
                    fSignalEvent->SetID(fLastEventId);
                    fSignalEvent->SetTime(fLastTimeStamp);
                }
            }

            fLastEventId = tmp;
            fLastTimeStamp = tStart + (2147483648 * r2 + 32768 * r1 + r0) * 2e-8;

            // If it is the first event we use it to define the run start time
            if (fCounter == 0) {
                fRunInfo->SetStartTimeStamp(fLastTimeStamp);
                fCounter++;
            } else {
                // and we keep updating the end run time
                fRunInfo->SetEndTimeStamp(fLastTimeStamp);
            }

            fSignalEvent->SetRunOrigin(fRunOrigin);
            fSignalEvent->SetSubRunOrigin(fSubRunOrigin);
        } else if ((*p & PFX_4_BIT_CONTENT_MASK) == PFX_END_OF_EVENT) {
            tmp = ((unsigned int)GET_EOE_SIZE(*p)) << 16;
            p++;
            tmp = tmp + (unsigned int)*p;
            p++;
            if (GetVerboseLevel() >= REST_Debug)
                printf("ReadFrame: ----- End of Event ----- (size %d bytes)\n", tmp);
            if (GetVerboseLevel() >= REST_Debug) GetChar();

            if (fElectronicsType == "SingleFeminos") endOfEvent = true;
        }

        // Is it a prefix for 0-bit content?
        else if ((*p & PFX_0_BIT_CONTENT_MASK) == PFX_END_OF_FRAME) {
            if (sgnl.GetSignalID() >= 0 && sgnl.GetNumberOfPoints() >= fMinPoints)
                fSignalEvent->AddSignal(sgnl);

            if (GetVerboseLevel() >= REST_Debug) printf("ReadFrame: ----- End of Frame -----\n");
            p++;
            done = 1;
        } else if (*p == PFX_START_OF_BUILT_EVENT) {
            if (GetVerboseLevel() >= REST_Debug) printf("ReadFrame: ***** Start of Built Event *****\n");
            p++;
        } else if (*p == PFX_END_OF_BUILT_EVENT) {
            if (GetVerboseLevel() >= REST_Debug) printf("ReadFrame: ***** End of Built Event *****\n\n");
            p++;
        } else if (*p == PFX_SOBE_SIZE) {
            // Skip header
            p++;

            // Built Event Size lower 16-bit
            r0 = *p;
            p++;
            // Built Event Size upper 16-bit
            r1 = *p;
            p++;
            tmp_i[0] = (int)((r1 << 16) | (r0));

            if (GetVerboseLevel() >= REST_Debug)
                printf("ReadFrame: ***** Start of Built Event - Size = %d bytes *****\n", tmp_i[0]);
        } else {
            p++;
        }
    }

    return endOfEvent;
}
