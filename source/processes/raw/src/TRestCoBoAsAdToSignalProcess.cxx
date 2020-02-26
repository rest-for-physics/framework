///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestCoBoAsAdToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestCoBoAsAdToSignalProcess
///             How to use: replace TRestCoBoAsAdToSignalProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________

// int counter = 0;

#include "TRestCoBoAsAdToSignalProcess.h"
using namespace std;
#include "TTimeStamp.h"

ClassImp(TRestCoBoAsAdToSignalProcess)
    //______________________________________________________________________________
    TRestCoBoAsAdToSignalProcess::TRestCoBoAsAdToSignalProcess() {
    Initialize();
}

TRestCoBoAsAdToSignalProcess::TRestCoBoAsAdToSignalProcess(char* cfgFileName)
    : TRestRawToSignalProcess(cfgFileName) {
    Initialize();
}

//______________________________________________________________________________
TRestCoBoAsAdToSignalProcess::~TRestCoBoAsAdToSignalProcess() {
    // TRestCoBoAsAdToSignalProcess destructor
}

//______________________________________________________________________________
void TRestCoBoAsAdToSignalProcess::Initialize() {
    // this->SetVerboseLevel(REST_Debug);
}

//______________________________________________________________________________
void TRestCoBoAsAdToSignalProcess::InitProcess() {}

Bool_t TRestCoBoAsAdToSignalProcess::OpenInputCoBoAsAdBinFile(TString fName) {
    TRestDetectorSetup* det = GetMetadata<TRestDetectorSetup>();

    if (det != NULL) {
        fRunOrigin = det->GetRunNumber();
        fSubRunOrigin = det->GetSubRunNumber();
    } else {
        cout << "REST WARNING : Detector setup has not been defined. Run and "
                "subRunNumber will not be defined!"
             << endl;
    }

    if (fInputBinFile != NULL) fclose(fInputBinFile);

    const Ssiz_t fnOffset = fName.Index(".graw");
    int year, month, day, hour, minute, second,
        millisecond;  // these parameters have to be extracted from the file name.
                      // So do not change the origin binary file name.
    if (fName.Length() != fnOffset + 5 || fnOffset < 28) {
        cout << "Input binary file name type unknown!" << endl;
        return kFALSE;
    }
    if (fName[fnOffset - 24] != '-' || fName[fnOffset - 21] != '-' || fName[fnOffset - 18] != 'T' ||
        fName[fnOffset - 15] != ':' || fName[fnOffset - 12] != ':' || fName[fnOffset - 9] != '.' ||
        fName[fnOffset - 5] != '_') {
        cout << "Input binary file name unknown!" << endl;
        return kFALSE;
    }

    year = (int)(fName[fnOffset - 28] - 48) * 1000 + (int)(fName[fnOffset - 27] - 48) * 100 +
           (int)(fName[fnOffset - 26] - 48) * 10 + (int)(fName[fnOffset - 25] - 48) * 1;
    month = (int)(fName[fnOffset - 23] - 48) * 10 + (int)(fName[fnOffset - 22] - 48) * 1;
    day = (int)(fName[fnOffset - 20] - 48) * 10 + (int)(fName[fnOffset - 19] - 48) * 1;
    hour = (int)(fName[fnOffset - 17] - 48) * 10 + (int)(fName[fnOffset - 16] - 48) * 1;
    minute = (int)(fName[fnOffset - 14] - 48) * 10 + (int)(fName[fnOffset - 13] - 48) * 1;
    second = (int)(fName[fnOffset - 11] - 48) * 10 + (int)(fName[fnOffset - 10] - 48) * 1;
    millisecond = (int)(fName[fnOffset - 8] - 48) * 100 + (int)(fName[fnOffset - 7] - 48) * 10 +
                  (int)(fName[fnOffset - 6] - 48) * 1;
    // cout<<"time: "<<year<<" "<<month<<" "<<day<<" "<<hour<<" "<<minute<<"
    // "<<second<<" "<<millisecond<<endl;

    fStartTimeStamp.Set(year, month, day, hour, minute, second, millisecond * 1000000, kTRUE,
                        -8 * 3600);  // Offset for Beijing(local) time

    if ((fInputBinFile = fopen(fName.Data(), "rb")) == NULL) {
        cout << "WARNING. Input file does not exist" << endl;
        return kFALSE;
    }

    fInputFileNames.push_back((string)fName);
    fStartTimeStamp.Print();

    return kTRUE;
}

//______________________________________________________________________________
TRestEvent* TRestCoBoAsAdToSignalProcess::ProcessEvent(TRestEvent* evInput) {
    UChar_t frameHeader[256];    // 256: size of header of the cobo data frame
    UChar_t frameDataP[2048];    // for partial readout data frame
    UChar_t frameDataF[278528];  // for full readout data frame
    unsigned int frameSize, frameType, revision, headerSize, itemSize, nItems, eventIdx, asadIdx, readOffset,
        status;
    Long64_t eventTime;
    unsigned int agetIdx, chanIdx, chanIdx0, chanIdx1, chanIdx2, chanIdx3, buckIdx, sample;

    TTimeStamp fEveTimeStamp;
    int fEveTimeSec;  // relative time
    int fEveTimeNanoSec;
    const Long64_t NsPerSec = 1000000000;
    int adcSample[272][512];
    bool chHit[272];
    unsigned int chTmp;
    unsigned int i;
    int j;

    TRestRawSignal sgnl;
    sgnl.SetSignalID(-1);

    if (fread(frameHeader, 256, 1, fInputBinFile) != 1 || feof(fInputBinFile)) {
        fclose(fInputBinFile);
        fOutputEvent = NULL;
        cout << "file end" << endl;
        return NULL;
    }
    frameType = (unsigned int)frameHeader[5] * 0x100 + (unsigned int)frameHeader[6];
    if (frameHeader[0] == 0x08 && frameType == 1)  // frameType: 1:partial readout  2:full readout
    {
        frameSize = (unsigned int)frameHeader[1] * 0x10000 + (unsigned int)frameHeader[2] * 0x100 +
                    (unsigned int)frameHeader[3];
        frameSize *= 256;
        // cout<<"frameSize: "<<frameSize<<endl;

        frameType = (unsigned int)frameHeader[5] * 0x100 + (unsigned int)frameHeader[6];
        // cout<<"frameType: "<<frameType<<endl;
        if (frameType != 1) {
            cout << "unsupported frame type!" << endl;
            return NULL;
        }

        revision = (unsigned int)frameHeader[7];
        // cout<<"revision: "<<revision<<endl;
        if (revision != 5) {
            cout << "unsupported revision!" << endl;
            return NULL;
        }

        headerSize = (unsigned int)frameHeader[8] * 0x100 + (unsigned int)frameHeader[9];
        // cout<<"headerSize: "<<headerSize<<endl;
        if (headerSize != 1) {
            cout << "unsupported header size!" << endl;
            return NULL;
        }

        itemSize = (unsigned int)frameHeader[10] * 0x100 + (unsigned int)frameHeader[11];
        // cout<<"itemSize: "<<itemSize<<endl;
        if (itemSize != 4) {
            cout << "unsupported item size!" << endl;
            return NULL;
        }

        nItems = (unsigned int)frameHeader[12] * 0x1000000 + (unsigned int)frameHeader[13] * 0x10000 +
                 (unsigned int)frameHeader[14] * 0x100 + (unsigned int)frameHeader[15];
        // cout<<"nItems: "<<nItems<<endl;

        eventTime = (Long64_t)frameHeader[16] * 0x10000000000 + (Long64_t)frameHeader[17] * 0x100000000 +
                    (Long64_t)frameHeader[18] * 0x1000000 + (Long64_t)frameHeader[19] * 0x10000 +
                    (Long64_t)frameHeader[20] * 0x100 + (Long64_t)frameHeader[21];
        eventTime *= 10;  // ns at 100MHz experiment clock
        // cout<<"eventTime: "<<eventTime<<endl;

        eventIdx = (unsigned int)frameHeader[22] * 0x1000000 + (unsigned int)frameHeader[23] * 0x10000 +
                   (unsigned int)frameHeader[24] * 0x100 + (unsigned int)frameHeader[25];
        // cout<<"eventIdx: "<<eventIdx<<endl;

        asadIdx = (unsigned int)frameHeader[27];
        // cout<<"asadIdx: "<<asadIdx<<endl;

        readOffset = (unsigned int)frameHeader[28] * 0x100 + (unsigned int)frameHeader[29];
        // cout<<"readOffset: "<<readOffset<<endl;
        if (readOffset != 0) {
            cout << "unsupported readOffset!" << endl;
            return NULL;
        }

        status = (unsigned int)frameHeader[30];
        if (status) {
            cout << "bad frame!" << endl;
            return NULL;
        }
        // cout<<"status: "<<status<<endl;

        //------------read frame data-----------
        if (frameSize > 256 && nItems * 4 == frameSize - 256) {
            for (i = 0; i < 272; i++) {
                chHit[i] = 0;
                for (j = 0; j < 512; j++) adcSample[i][j] = 0;
            }

            for (i = 0; i * 512 < nItems; i++) {
                if ((fread(frameDataP, 2048, 1, fInputBinFile)) != 1 || feof(fInputBinFile)) {
                    fclose(fInputBinFile);
                    fOutputEvent = NULL;
                    cout << "file end 2" << endl;
                    return NULL;
                }
                for (j = 0; j < 2048; j += 4) {
                    agetIdx = (frameDataP[j] >> 6);
                    chanIdx = ((unsigned int)(frameDataP[j] & 0x3f) * 2 + (frameDataP[j + 1] >> 7));
                    chTmp = agetIdx * 68 + chanIdx;
                    buckIdx = ((unsigned int)(frameDataP[j + 1] & 0x7f) * 4 + (frameDataP[j + 2] >> 6));
                    sample = ((unsigned int)(frameDataP[j + 2] & 0x0f) * 0x100 + frameDataP[j + 3]);

                    if (buckIdx >= 512 || chTmp >= 272) {
                        cout << "buck or channel id error!" << endl;
                        return NULL;
                    }

                    chHit[chTmp] = 1;
                    adcSample[chTmp][buckIdx] = sample;
                }
            }
            for (i = 0; i < 272; i++) {
                if (chHit[i]) {
                    sgnl.Initialize();
                    sgnl.SetSignalID(i + asadIdx * 272);
                    for (j = 0; j < 512; j++) sgnl.AddPoint((Short_t)adcSample[i][j]);
                    fSignalEvent->AddSignal(sgnl);
                }
            }

            fEveTimeSec = 0;  // relative time
            fEveTimeNanoSec = 0;
            while (eventTime >= NsPerSec) {
                eventTime -= NsPerSec;
                fEveTimeSec++;
            }
            fEveTimeNanoSec = (int)eventTime;
            fEveTimeStamp.SetSec(fEveTimeSec);
            fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
            fEveTimeStamp.Add(fStartTimeStamp);

            fSignalEvent->SetTimeStamp(fEveTimeStamp);
            fSignalEvent->SetID(eventIdx);
            fSignalEvent->SetRunOrigin(fRunOrigin);
            fSignalEvent->SetSubRunOrigin(fSubRunOrigin);
        }

    } else if (frameHeader[0] == 0x08 && frameType == 2) {
        frameSize = (unsigned int)frameHeader[1] * 0x10000 + (unsigned int)frameHeader[2] * 0x100 +
                    (unsigned int)frameHeader[3];
        frameSize *= 256;
        // cout<<"frameSize: "<<frameSize<<endl;

        frameType = (unsigned int)frameHeader[5] * 0x100 + (unsigned int)frameHeader[6];
        // cout<<"frameType: "<<frameType<<endl;
        if (frameType != 2) {
            cout << "unsupported frame type!" << endl;
            return NULL;
        }

        revision = (unsigned int)frameHeader[7];
        // cout<<"revision: "<<revision<<endl;
        if (revision != 5) {
            cout << "unsupported revision!" << endl;
            return NULL;
        }

        headerSize = (unsigned int)frameHeader[8] * 0x100 + (unsigned int)frameHeader[9];
        // cout<<"headerSize: "<<headerSize<<endl;
        if (headerSize != 1) {
            cout << "unsupported header size!" << endl;
            return NULL;
        }

        itemSize = (unsigned int)frameHeader[10] * 0x100 + (unsigned int)frameHeader[11];
        // cout<<"itemSize: "<<itemSize<<endl;
        if (itemSize != 2) {
            cout << "unsupported item size!" << endl;
            return NULL;
        }

        nItems = (unsigned int)frameHeader[12] * 0x1000000 + (unsigned int)frameHeader[13] * 0x10000 +
                 (unsigned int)frameHeader[14] * 0x100 + (unsigned int)frameHeader[15];
        if (nItems != 139264) {
            cout << "unsupported nItems!" << endl;
            return NULL;
        }
        // cout<<"nItems: "<<nItems<<endl;

        eventTime = (Long64_t)frameHeader[16] * 0x10000000000 + (Long64_t)frameHeader[17] * 0x100000000 +
                    (Long64_t)frameHeader[18] * 0x1000000 + (Long64_t)frameHeader[19] * 0x10000 +
                    (Long64_t)frameHeader[20] * 0x100 + (Long64_t)frameHeader[21];
        eventTime *= 10;  // ns at 100MHz experiment clock
        // cout<<"eventTime: "<<eventTime<<endl;

        eventIdx = (unsigned int)frameHeader[22] * 0x1000000 + (unsigned int)frameHeader[23] * 0x10000 +
                   (unsigned int)frameHeader[24] * 0x100 + (unsigned int)frameHeader[25];
        // cout<<"eventIdx: "<<eventIdx<<endl;

        asadIdx = (unsigned int)frameHeader[27];
        // cout<<"asadIdx: "<<asadIdx<<endl;

        readOffset = (unsigned int)frameHeader[28] * 0x100 + (unsigned int)frameHeader[29];
        // cout<<"readOffset: "<<readOffset<<endl;
        if (readOffset != 0) {
            cout << "unsupported readOffset!" << endl;
            return NULL;
        }

        status = (unsigned int)frameHeader[30];
        if (status) {
            cout << "bad frame!" << endl;
            return NULL;
        }
        // cout<<"status: "<<status<<endl;

        //------------read frame data-----------
        if (frameSize > 256)  // not needed
        {
            for (i = 0; i < 272; i++) {
                chHit[i] = 0;
                for (j = 0; j < 512; j++) adcSample[i][j] = 0;
            }

            if (fread(frameDataF, 2048, 136, fInputBinFile) != 136 || feof(fInputBinFile)) {
                fclose(fInputBinFile);
                fOutputEvent = NULL;
                cout << "file end 2" << endl;
                return NULL;
            }

            int tmpP;
            for (i = 0; i < 512; i++) {
                chanIdx0 = 0;
                chanIdx1 = 0;
                chanIdx2 = 0;
                chanIdx3 = 0;
                for (j = 0; j < 272; j++) {
                    tmpP = (i * 272 + j) * 2;
                    agetIdx = (frameDataF[tmpP] >> 6);
                    sample = ((unsigned int)(frameDataF[tmpP] & 0x0f) * 0x100 + frameDataF[tmpP + 1]);

                    if (agetIdx == 0) {
                        chanIdx = chanIdx0;
                        chanIdx0++;
                    } else if (agetIdx == 1) {
                        chanIdx = chanIdx1;
                        chanIdx1++;
                    } else if (agetIdx == 2) {
                        chanIdx = chanIdx2;
                        chanIdx2++;
                    } else {
                        chanIdx = chanIdx3;
                        chanIdx3++;
                    }

                    // cout<<"agetIdx: "<<agetIdx<<" chanIdx: "<<chanIdx<<endl;
                    if (chanIdx > 67) {
                        cout << "buck or channel id error!" << endl;
                        return NULL;
                    }
                    chTmp = agetIdx * 68 + chanIdx;
                    adcSample[chTmp][i] = sample;
                }
            }

            for (i = 0; i < 272; i++) {
                sgnl.Initialize();
                sgnl.SetSignalID(i + asadIdx * 272);
                for (j = 0; j < 512; j++) sgnl.AddPoint((Short_t)adcSample[i][j]);
                fSignalEvent->AddSignal(sgnl);
            }

            fEveTimeSec = 0;  // relative time
            fEveTimeNanoSec = 0;
            while (eventTime >= NsPerSec) {
                eventTime -= NsPerSec;
                fEveTimeSec++;
            }
            fEveTimeNanoSec = (int)eventTime;
            fEveTimeStamp.SetSec(fEveTimeSec);
            fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
            fEveTimeStamp.Add(fStartTimeStamp);

            fSignalEvent->SetTimeStamp(fEveTimeStamp);
            fSignalEvent->SetID(eventIdx);
            fSignalEvent->SetRunOrigin(fRunOrigin);
            fSignalEvent->SetSubRunOrigin(fSubRunOrigin);
        }

    } else {
        fclose(fInputBinFile);
        fOutputEvent = NULL;
        cout << "file type error!" << endl;
        return NULL;
    }

    if (fSignalEvent->GetNumberOfSignals() == 0) return NULL;
    return fSignalEvent;
}
