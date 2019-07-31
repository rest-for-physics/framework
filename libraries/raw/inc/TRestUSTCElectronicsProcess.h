///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestUSTCElectronicsProcess.h
///
///             Template to use to design "event process" classes inherited from
///             TRestProcess
///             How to use: replace TRestUSTCElectronicsProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             May 2017:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software. Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestUSTCElectronicsProcess
#define RestCore_TRestUSTCElectronicsProcess

#include <map>
#include "TRestRawToSignalProcess.h"

#define Long_Readout_Format
//#define Incoherent_Event_Generation

#ifdef Long_Readout_Format
#define N 1048
#else
#define N 1040
#endif  // Long_ReadoutFormat

struct USTCDataFrame {
    // a signal-level data frame
    // e.g.
    // EEEE | E0A0 | 246C0686 | 0001 | 2233 | (A098)(A09C)... | FFFF
    // header | board number | event time | channel id(0~63) | event id | [chip id
    // + data(0~4095)]*512 | ending
    USTCDataFrame() {
        boardId = 0;
        chipId = 0;
        readoutType = 0;
        eventTime = 0;
        channelId = 0;
        evId = -1;
        signalId = 0;
    }
    UChar_t data[N];  // the size of a signal frame

    Int_t boardId;       // 0~n
    Int_t chipId;        // 0~3 aget number
    Int_t readoutType;   // 2:full readout  1:partial readout
    Long64_t eventTime;  // event time in na
    Int_t channelId;     // 0~63 channels
    Int_t evId;          // if equals -1, this data frame is used but have not been
                         // re-filled

    Int_t signalId;
    Int_t dataPoint[512];
};

class TRestUSTCElectronicsProcess : public TRestRawToSignalProcess {
   private:
#ifndef __CINT__
    TRestRawSignalEvent* fSignalEvent;  //!
    TRestRawSignal sgnl;                //!

    vector<vector<USTCDataFrame>> fEventBuffer;  //!
    int nBufferedEvent;                          //!
    int fCurrentFile = 0;                        //!
    int fCurrentEvent = -1;                      //!
    int fCurrentBuffer = 0;                      //!
    Long64_t fTimeOffset = 0;
    int fLastBufferedId = 0;  //!

    vector<int> errorevents;  //!
    int unknownerrors = 0;    //!

#endif

   public:
    void InitProcess();
    void Initialize();

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void EndProcess();

    bool FillBuffer();

    bool GetNextFrame(USTCDataFrame&);

    void FixToNextFrame(FILE* f);

    bool ReadFrameData(USTCDataFrame& Frame);

    bool AddBuffer(USTCDataFrame& Frame);

    void ClearBuffer();

    Bool_t EndReading();

    // Constructor
    TRestUSTCElectronicsProcess();
    TRestUSTCElectronicsProcess(char* cfgFileName);
    // Destructor
    ~TRestUSTCElectronicsProcess();

    ClassDef(TRestUSTCElectronicsProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
