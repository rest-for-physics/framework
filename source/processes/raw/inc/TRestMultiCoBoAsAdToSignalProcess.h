///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMultiCoBoAsAdToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestMultiCoBoAsAdToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             May 2017:   First concept
///                 Created as part of the conceptualization of existing REST software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestMultiCoBoAsAdToSignalProcess
#define RestCore_TRestMultiCoBoAsAdToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"
#include <map>

struct CoBoDataFrame {
	CoBoDataFrame()
	{
		timeStamp = 0;
		evId = -1;
		asadId = -1;
		for (int m = 0; m < 272; m++)
			chHit[m] = kFALSE;

		for (int m = 0; m < 272; m++)
			for (int l = 0; l < 512; l++)
				data[m][l] = 0;
	}
	TTimeStamp timeStamp;
	Bool_t chHit[272];
	Int_t data[272][512];
	Int_t evId;//if equals -1, this data frame is used but have not been re-filled
	Int_t asadId;
};

struct CoBoHeaderFrame {
	CoBoHeaderFrame() {
		for (int m = 0; m < 256; m++)
			frameHeader[m] = 0;
	}
	UChar_t frameHeader[256];//256: size of header of the cobo data frame

	unsigned int frameSize;
	unsigned int frameType;
	unsigned int revision;
	unsigned int headerSize;
	unsigned int itemSize;
	unsigned int nItems;
	unsigned int eventIdx;
	unsigned int asadIdx;
	unsigned int readOffset;
	unsigned int status;

	Long64_t eventTime;
	//TTimeStamp fEveTimeStamp;
	//int fEveTimeSec;
	//int fEveTimeNanoSec;

	void Show()
	{
		cout << "------ Frame Header ------" << endl;
		cout << "frameSize " << frameSize << endl;

		cout << "frameType " << frameType << endl;
		cout << "revision " << revision << endl;
		cout << "headerSize " << headerSize << endl;
		cout << "itemSize " << itemSize << endl;
		cout << "nItems " << nItems << endl;
		cout << "eventTime " << eventTime << endl;
		cout << "eventIdx " << eventIdx << endl;

		cout << "asadIdx " << asadIdx << endl;
		cout << "readOffset " << readOffset << endl;
		cout << "status " << status << endl;

	}


};

class TRestMultiCoBoAsAdToSignalProcess :public TRestRawToSignalProcess {

private:



#ifndef __CINT__
	TRestRawSignalEvent *fSignalEvent;//!
	TRestRawSignal sgnl;//!

	UChar_t frameDataP[2048];//!///for partial readout data frame
	UChar_t frameDataF[278528];//!///for full readout data frame


	TTimeStamp fStartTimeStamp;//!

	std::map<int, CoBoDataFrame> fDataFrame;//!///asadId, dataframe

	vector<CoBoHeaderFrame> fHeaderFrame;//!///reserves a header frame for each file

	int fCurrentEvent = -1;//!
	int fNextEvent = -1;//!
#endif

public:
	void InitProcess();
	void Initialize();

	Bool_t InitializeStartTimeStampFromFilename(TString fName);

	TRestEvent *ProcessEvent(TRestEvent *evInput);

	void EndProcess();

	Bool_t FillBuffer(Int_t n);

	bool fillbuffer();

	bool ReadFrameHeader(CoBoHeaderFrame& Frame);


	bool ReadFrameDataP(FILE*f, CoBoHeaderFrame& hdr);
	bool ReadFrameDataF(CoBoHeaderFrame& hdr);


	void ClearBuffer(Int_t n);

	Int_t GetLowestEventId();

	Bool_t EndReading();

	//Constructor
	TRestMultiCoBoAsAdToSignalProcess();
	TRestMultiCoBoAsAdToSignalProcess(char *cfgFileName);
	//Destructor
	~TRestMultiCoBoAsAdToSignalProcess();

	ClassDef(TRestMultiCoBoAsAdToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
