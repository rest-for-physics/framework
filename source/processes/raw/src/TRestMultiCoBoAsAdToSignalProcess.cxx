///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMultiCoBoAsAdToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestMultiCoBoAsAdToSignalProcess
///             How to use: replace TRestMultiCoBoAsAdToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________

//int counter = 0;

#include "TRestMultiCoBoAsAdToSignalProcess.h"
using namespace std;
#include "TTimeStamp.h"
#include <bitset>

ClassImp(TRestMultiCoBoAsAdToSignalProcess)
//______________________________________________________________________________
TRestMultiCoBoAsAdToSignalProcess::TRestMultiCoBoAsAdToSignalProcess()
{
	Initialize();
}

TRestMultiCoBoAsAdToSignalProcess::TRestMultiCoBoAsAdToSignalProcess(char *cfgFileName)
{
	Initialize();
}


//______________________________________________________________________________
TRestMultiCoBoAsAdToSignalProcess::~TRestMultiCoBoAsAdToSignalProcess()
{

	// TRestMultiCoBoAsAdToSignalProcess destructor
}

//______________________________________________________________________________
void TRestMultiCoBoAsAdToSignalProcess::Initialize()
{
	TRestRawToSignalProcess::Initialize();

	SetSectionName(this->ClassName());

	fSignalEvent = new TRestRawSignalEvent();

	fOutputEvent = fSignalEvent;

}



Bool_t TRestMultiCoBoAsAdToSignalProcess::InitializeStartTimeStampFromFilename(TString fName)
{
	//these parameters have to be extracted from the file name. So do not change the origin binary file name.
	int year, month, day, hour, minute, second, millisecond;

	const Ssiz_t fnOffset = fName.Index(".graw");

	if (fName.Length() != fnOffset + 5 || fnOffset < 28)
	{
		cout << "Input binary file name type unknown!" << endl;
		return kFALSE;
	}

	if (fName[fnOffset - 24] != '-' || fName[fnOffset - 21] != '-' ||
		fName[fnOffset - 18] != 'T' || fName[fnOffset - 15] != ':' ||
		fName[fnOffset - 12] != ':' || fName[fnOffset - 9] != '.' || fName[fnOffset - 5] != '_')
	{
		cout << "Input binary file name unknown!" << endl;
		return kFALSE;
	}

	year = (int)(fName[fnOffset - 28] - 48) * 1000 + (int)(fName[fnOffset - 27] - 48) * 100
		+ (int)(fName[fnOffset - 26] - 48) * 10 + (int)(fName[fnOffset - 25] - 48) * 1;
	month = (int)(fName[fnOffset - 23] - 48) * 10 + (int)(fName[fnOffset - 22] - 48) * 1;
	day = (int)(fName[fnOffset - 20] - 48) * 10 + (int)(fName[fnOffset - 19] - 48) * 1;
	hour = (int)(fName[fnOffset - 17] - 48) * 10 + (int)(fName[fnOffset - 16] - 48) * 1;
	minute = (int)(fName[fnOffset - 14] - 48) * 10 + (int)(fName[fnOffset - 13] - 48) * 1;
	second = (int)(fName[fnOffset - 11] - 48) * 10 + (int)(fName[fnOffset - 10] - 48) * 1;
	millisecond = (int)(fName[fnOffset - 8] - 48) * 100 + (int)(fName[fnOffset - 7] - 48) * 10 + (int)(fName[fnOffset - 6] - 48) * 1;

	fStartTimeStamp.Set(year, month, day, hour, minute, second,
		millisecond * 1000000, kTRUE, -8 * 3600);  // Offset for Beijing(local) time
	return kTRUE;
}


vector<int> fileerrors;
void TRestMultiCoBoAsAdToSignalProcess::InitProcess()
{
	fDataFrame.clear();
	fHeaderFrame.clear();
	fileerrors.clear();

	for (int n = 0; n < fInputFiles.size(); n++)
	{
		CoBoHeaderFrame hdrtmp;
		fHeaderFrame.push_back(hdrtmp);
		fileerrors.push_back(0);
	}

	fRunOrigin = fRunInfo->GetRunNumber();
	fOutputEvent = fSignalEvent;
	fCurrentEvent = -1;

	totalBytesReaded = 0;
}


TRestEvent* TRestMultiCoBoAsAdToSignalProcess::ProcessEvent(TRestEvent *evInput)
{
	fSignalEvent->Initialize();
	if (EndReading()) { fOutputEvent = NULL; return NULL; }

	if (!fillbuffer()) {
		fSignalEvent->SetOK(false);
		return fSignalEvent;
	}
	//Int_t nextId = GetLowestEventId();

	if (GetVerboseLevel() >= REST_Debug)
	{
		cout << "TRestMultiCoBoAsAdToSignalProcess: Generating event with ID: " << fCurrentEvent << endl;
	}




	TTimeStamp tSt = 0;

	map<int, CoBoDataFrame>::iterator it;
	it = fDataFrame.begin();

	while (it != fDataFrame.end())
	{
		CoBoDataFrame* data = &(it->second);
		if (data->evId == fCurrentEvent)
		{
			for (int m = 0; m < 272; m++)
			{
				if (data->chHit[m])
				{

					if ((Double_t)tSt == 0) tSt = data->timeStamp;

					sgnl.Initialize();
					sgnl.SetSignalID(m + data->asadId * 272);

					for (int j = 0; j < 512; j++)
						sgnl.AddPoint((Short_t)data->data[m][j]);

					fSignalEvent->AddSignal(sgnl);

					if (GetVerboseLevel() >= REST_Extreme)
					{
						cout << "AgetId, chnId, first value, max value: " << m / 68 << ", " << m % 68 << ", " << sgnl.GetData(0) << ", " << sgnl.GetMaxValue() << endl;
					}
				}
			}
			data->evId = -1;
			for (int m = 0; m < 272; m++)
				data->chHit[m] = kFALSE;
		}
		it++;
	}

	if (GetVerboseLevel() >= REST_Debug)
	{
		cout << "TRestMultiCoBoAsAdToSignalProcess: event time is : " << tSt << endl;
		cout << "TRestMultiCoBoAsAdToSignalProcess: " << fSignalEvent->GetNumberOfSignals() << " signals added" << endl;
		cout << "------------------------------------" << endl;
	}
	fSignalEvent->SetTimeStamp(tSt);
	fSignalEvent->SetID(fCurrentEvent);
	fSignalEvent->SetRunOrigin(233);
	fSignalEvent->SetSubRunOrigin(2333);

	//cout << fSignalEvent->GetNumberOfSignals() << endl;
	//if( fSignalEvent->GetNumberOfSignals( ) == 0 ) return NULL;

	return fSignalEvent;

}


void TRestMultiCoBoAsAdToSignalProcess::EndProcess() 
{

	for (int i = 0; i < fileerrors.size(); i++) {
		if (fileerrors[i] > 0) {
			warning << "Found " << fileerrors[i] << " error frame headers in file " << i << endl;
			warning<< "\"" << fInputFileNames[i] << "\"" << endl;
		}
	}

	fileerrors.clear();
}





//true: finish filling
//false: error when filling
bool TRestMultiCoBoAsAdToSignalProcess::fillbuffer()
{
	//if event id = -1(no event has been read before), read header frame for each file
	if (fCurrentEvent == -1) {
		for (int i = 0; i < fInputFiles.size(); i++) {
			if (fread(fHeaderFrame[i].frameHeader, 256, 1, fInputFiles[i]) != 1 || feof(fInputFiles[i])) {
				fclose(fInputFiles[i]);
				fInputFiles[i] = NULL;
				return kFALSE;
			}
			totalBytesReaded+=256;
			if (!ReadFrameHeader(fHeaderFrame[i])) {
				cout << "error when reading frame header in file " << i << " \"" << fInputFileNames[i] << "\"" << endl;
				cout << "event id " << fCurrentEvent + 1 << ". The file will be closed" << endl;
				fHeaderFrame[i].Show();
				cout << endl;
				GetChar();
				fclose(fInputFiles[i]);
				fInputFiles[i] = NULL;
				return false;
			}
		}
	}

	//normally:
	//1.use the smallest event id in header frames as current event id
	int evt = fHeaderFrame[0].eventIdx;
	for (int i = 1; i < fHeaderFrame.size(); i++)
	{
		if (fHeaderFrame[i].eventIdx < evt)evt = fHeaderFrame[i].eventIdx;
	}
	fCurrentEvent = evt;


	//loop for each file
	for (int i = 0; i < fHeaderFrame.size(); i++)
	{
		if (fInputFiles[i] == NULL) { continue; }

		//file position is at the end of last header
		//if the eventid of last header is same as current, do the following:
		//a. read the data frame behind
		//b. read the next frame header
		//c. if eventid is the same as current, return to a, otherwise break.
		while (fHeaderFrame[i].eventIdx == fCurrentEvent) {

			if (GetVerboseLevel() >= REST_Debug) {
				cout << "TRestMultiCoBoAsAdToSignalProcess: retrieving frame header in file " << i << " (" << fInputFileNames[i] << ")" << endl;
				if (GetVerboseLevel() >= REST_Extreme)
					fHeaderFrame[i].Show();
			}

			//reading data according to the header
			unsigned int type = fHeaderFrame[i].frameType;
			if (fHeaderFrame[i].frameHeader[0] == 0x08 && type == 1) //partial readout
			{
				ReadFrameDataP(fInputFiles[i], fHeaderFrame[i]);
			}
			else if (fHeaderFrame[i].frameHeader[0] == 0x08 && type == 2) //full readout
			{
				if (fread(frameDataF, 2048, 136, fInputFiles[i]) != 136 || feof(fInputFiles[i]))
				{
					fclose(fInputFiles[i]);
					fInputFiles[i] = NULL;
					fHeaderFrame[i].eventIdx = (unsigned int)4294967295;
					break;
				}
				totalBytesReaded += 278528;
				ReadFrameDataF(fHeaderFrame[i]);
			}
			else
			{
				fclose(fInputFiles[i]);
				fInputFiles[i] = NULL;
				return false;
			}


			//reading next header
			if (fread(fHeaderFrame[i].frameHeader, 256, 1, fInputFiles[i]) != 1 || feof(fInputFiles[i])) {
				fclose(fInputFiles[i]);
				fInputFiles[i] = NULL;
				fHeaderFrame[i].eventIdx = (unsigned int)4294967295;//maximum of unsigned int
				break;
			}
			totalBytesReaded += 256;
			if (!ReadFrameHeader(fHeaderFrame[i])) { 
				warning << "Event " << fCurrentEvent << " : error when reading next frame header" << endl;
				warning << "in file " << i << " \"" << fInputFileNames[i] << "\"" << endl;
				if (fVerboseLevel>REST_Info)
					fHeaderFrame[i].Show();
				warning << "trying to skip this event and find next header..." << endl;
				fileerrors[i] += 1;
				REST_Verbose_Level tmp = fVerboseLevel;
				bool found = false;
				fVerboseLevel = REST_Silent;
				for (int k = 0; k < 1088; k++) //fullreadoutsize(278528)/headersize(256)=1088
				{
					if (fread(fHeaderFrame[i].frameHeader, 256, 1, fInputFiles[i]) != 1 || feof(fInputFiles[i])) {
						break;
					}
					totalBytesReaded += 256;
					if (ReadFrameHeader(fHeaderFrame[i]))
					{
						fVerboseLevel = tmp;
						warning << "Successfully found next header (EventId : " << fHeaderFrame[i].eventIdx<<")" << endl;
						if(fVerboseLevel>REST_Info)
							fHeaderFrame[i].Show();
						cout << endl;
						//GetChar();
						found = true;
						fSignalEvent->SetOK(false);
						break;
					}
				}
				if (!found) {
					fclose(fInputFiles[i]);
					fInputFiles[i] = NULL;
					fHeaderFrame[i].eventIdx = (unsigned int)4294967295;//maximum of unsigned int
				}

			}




		}

	}

	return true;

}


bool TRestMultiCoBoAsAdToSignalProcess::ReadFrameHeader(CoBoHeaderFrame& HdrFrame)
{
	UChar_t* Header = &(HdrFrame.frameHeader[0]);

	HdrFrame.frameSize = (unsigned int)Header[1] * 0x10000 + (unsigned int)Header[2] * 0x100 + (unsigned int)Header[3];
	HdrFrame.frameSize *= 256;

	HdrFrame.frameType = (unsigned int)Header[5] * 0x100 + (unsigned int)Header[6];
	HdrFrame.revision = (unsigned int)Header[7];
	HdrFrame.headerSize = (unsigned int)Header[8] * 0x100 + (unsigned int)Header[9];
	HdrFrame.itemSize = (unsigned int)Header[10] * 0x100 + (unsigned int)Header[11];
	HdrFrame.nItems = (unsigned int)Header[12] * 0x1000000 + (unsigned int)Header[13] * 0x10000 + (unsigned int)Header[14] * 0x100 + (unsigned int)Header[15];
	HdrFrame.eventTime = (Long64_t)Header[16] * 0x10000000000 + (Long64_t)Header[17] * 0x100000000 + (Long64_t)Header[18] * 0x1000000 + (Long64_t)Header[19] * 0x10000 + (Long64_t)Header[20] * 0x100 + (Long64_t)Header[21];
	HdrFrame.eventTime *= 10;//ns at 100MHz experiment clock
	HdrFrame.eventIdx = (unsigned int)Header[22] * 0x1000000 + (unsigned int)Header[23] * 0x10000 + (unsigned int)Header[24] * 0x100 + (unsigned int)Header[25];

	HdrFrame.asadIdx = (unsigned int)Header[27];
	HdrFrame.readOffset = (unsigned int)Header[28] * 0x100 + (unsigned int)Header[29];
	HdrFrame.status = (unsigned int)Header[30];


	//if (fCurrentEvent == -1) { fCurrentEvent = HdrFrame.eventIdx; }
	//if (fCurrentEvent != HdrFrame.eventIdx) { fNextEvent = HdrFrame.eventIdx; return 1; }

	//HdrFrame.fEveTimeSec = HdrFrame.eventTime / (Long64_t)1e9;//relative time
	//HdrFrame.fEveTimeNanoSec = HdrFrame.eventTime % (Long64_t)1e9;

	//HdrFrame.fEveTimeNanoSec = (int)HdrFrame.eventTime;
	//HdrFrame.fEveTimeStamp.SetSec(HdrFrame.fEveTimeSec);
	//HdrFrame.fEveTimeStamp.SetNanoSec(HdrFrame.fEveTimeNanoSec);
	//HdrFrame.fEveTimeStamp.Add(fStartTimeStamp);

	if (HdrFrame.frameType == 1) {
		if (HdrFrame.itemSize != 4)
		{
			warning << "unsupported item size!" << endl;
			return false;
		}

	}
	else if (HdrFrame.frameType == 2)
	{
		if (HdrFrame.itemSize != 2)
		{
			warning << "unsupported item size!" << endl;
			return false;
		}
		if (HdrFrame.nItems != 139264)
		{
			warning << "unsupported nItems!" << endl;
			return false;
		}
	}
	else
	{
		warning << "unknown frame type" << endl;
		return false;
	}

	//warning<<"revision: "<<revision<<endl;
	if (HdrFrame.revision != 5)
	{
		warning << "unsupported revision!" << endl;
		return false;
	}

	//warning<<"frameHeaderSize: "<<frameHeaderSize<<endl;
	if (HdrFrame.headerSize != 1)
	{
		warning << "unsupported frameHeader size!" << endl;
		return false;
	}

	//warning<<"readOffset: "<<readOffset<<endl;
	if (HdrFrame.readOffset != 0)
	{
		warning << "unsupported readOffset!" << endl;
		return false;
	}

	if (HdrFrame.status)
	{
		warning << "bad frame!" << endl;
		return false;
	}

	if (HdrFrame.nItems * HdrFrame.itemSize+256!= HdrFrame.frameSize) {
		warning << "Event "<<fCurrentEvent<<" : item number and frame size unmatch!" << endl;
		
		//sometimes there is a itemnumber-framesize unmatch problem
		//nItems*itemSize(=4)+256=frameSize
		//because nitems/512 should be a integer(signal number), we can make a fix
		if (HdrFrame.nItems % 512 == 0) {
			warning << "...frameSize (" << HdrFrame.frameSize;
			HdrFrame.frameSize = HdrFrame.nItems * HdrFrame.itemSize + 256;
			warning << ") fixed to " << HdrFrame.frameSize << "..." << endl;
			warning << endl;
		}
		else
		{
			if(fVerboseLevel>=REST_Info)
				HdrFrame.Show();
			if (((HdrFrame.frameSize - 256) / HdrFrame.itemSize) % 512 == 0) 
			{
				HdrFrame.nItems = (HdrFrame.frameSize - 256) / HdrFrame.itemSize;
				warning << "...nItems fixed to " << HdrFrame.nItems << "..." << endl;
				warning << endl;
			}
			else
			{
				warning << "frame unfixed" << endl;
				warning << endl;
			}
		}

	}

	return true;


}

bool TRestMultiCoBoAsAdToSignalProcess::ReadFrameDataP(FILE*f, CoBoHeaderFrame& hdr)
{
	unsigned int i;
	int j;
	unsigned int agetIdx, chanIdx, buckIdx, sample, chTmp;

	unsigned int asadid = hdr.asadIdx;
	unsigned int size = hdr.frameSize;
	unsigned int items = hdr.nItems;
	unsigned int eventid = hdr.eventIdx;
	Long64_t time = hdr.eventTime;
	TTimeStamp eveTimeStamp;
	CoBoDataFrame& dataf = fDataFrame[asadid];



	//------------read frame data-----------
	if (size > 256)
	{
		
		for (i = 0; i * 512 < items; i++)
		{
			if ((fread(frameDataP, 2048, 1, f)) != 1 || feof(f))
			{
				fclose(f);
				f = NULL;
				return kFALSE;
			}
			totalBytesReaded += 2048;
			for (j = 0; j < 2048; j += 4)
			{
				//total 8*4= 32 bits 
				//11         111111|1     1111111|11   11        1111|11111111
				//agetIdx    chanIdx      buckIdx      unused    samplepoint
				agetIdx = (frameDataP[j] >> 6);//first 2 bits of the byte
				chanIdx = ((unsigned int)(frameDataP[j] & 0x3f) * 2 + (frameDataP[j + 1] >> 7));
				chTmp = agetIdx * 68 + chanIdx;
				buckIdx = ((unsigned int)(frameDataP[j + 1] & 0x7f) * 4 + (frameDataP[j + 2] >> 6));
				sample = ((unsigned int)(frameDataP[j + 2] & 0x0f) * 0x100 + frameDataP[j + 3]);

				if (chTmp >= 272) { cout << "channel id error!" << endl; continue; }

				dataf.chHit[chTmp] = kTRUE;
				dataf.data[chTmp][buckIdx] = sample;

			}

		}
	}



	eveTimeStamp.SetNanoSec(time % ((Long64_t)1e9));
	eveTimeStamp.SetSec(time / ((Long64_t)1e9));
	eveTimeStamp.Add(fStartTimeStamp);

	dataf.asadId = asadid;
	dataf.evId = eventid;
	dataf.timeStamp = eveTimeStamp;

	return true;

}


bool TRestMultiCoBoAsAdToSignalProcess::ReadFrameDataF(CoBoHeaderFrame& hdr)
{
	int i;
	int j;
	unsigned int agetIdx, chanIdx, chanIdx0, chanIdx1, chanIdx2, chanIdx3, buckIdx, sample, chTmp;

	unsigned int asadid = hdr.asadIdx;
	unsigned int eventid = hdr.eventIdx;
	Long64_t time = hdr.eventTime;
	TTimeStamp eveTimeStamp;
	CoBoDataFrame& dataf = fDataFrame[asadid];

	int tmpP;
	for (i = 0; i < 512; i++)
	{
		chanIdx0 = 0;
		chanIdx1 = 0;
		chanIdx2 = 0;
		chanIdx3 = 0;
		for (j = 0; j < 272; j++)
		{
			//total 8*2= 16 bits 
			//11         11        1111|11111111
			//agetIdx    unused    samplepoint

			tmpP = (i * 272 + j) * 2;
			agetIdx = (frameDataF[tmpP] >> 6);
			sample = ((unsigned int)(frameDataF[tmpP] & 0x0f) * 0x100 + frameDataF[tmpP + 1]);

			if (agetIdx == 0)
			{
				chanIdx = chanIdx0;
				chanIdx0++;
			}
			else if (agetIdx == 1)
			{
				chanIdx = chanIdx1;
				chanIdx1++;
			}
			else if (agetIdx == 2)
			{
				chanIdx = chanIdx2;
				chanIdx2++;
			}
			else
			{
				chanIdx = chanIdx3;
				chanIdx3++;
			}
			//cout<<"agetIdx: "<<agetIdx<<" chanIdx: "<<chanIdx<<endl;

			if (chanIdx > 67) { /*cout << "buck or channel id error! ChannelId, AgetId: " << chanIdx << ", " << agetIdx << endl;*/ continue; }
			chTmp = agetIdx * 68 + chanIdx;
			dataf.chHit[chTmp] = kTRUE;
			dataf.data[chTmp][i] = sample;
		}
	}


	eveTimeStamp.SetNanoSec(time % ((Long64_t)1e9));
	eveTimeStamp.SetSec(time / ((Long64_t)1e9));
	eveTimeStamp.Add(fStartTimeStamp);

	dataf.asadId = asadid;
	dataf.evId = eventid;
	dataf.timeStamp = eveTimeStamp;

	return true;

}

void TRestMultiCoBoAsAdToSignalProcess::ClearBuffer(Int_t n)
{
	fDataFrame[n].evId = -1;
	for (int m = 0; m < 272; m++)
		fDataFrame[n].chHit[m] = kFALSE;
}

Int_t TRestMultiCoBoAsAdToSignalProcess::GetLowestEventId()
{
	Int_t evid = fDataFrame[0].evId;

	for (unsigned int m = 1; m < fDataFrame.size(); m++)
	{
		if (evid == -1) evid = fDataFrame[m].evId;
		if (fDataFrame[m].evId != -1 && fDataFrame[m].evId < evid) evid = fDataFrame[m].evId;
	}

	return evid;
}

Bool_t TRestMultiCoBoAsAdToSignalProcess::EndReading()
{
	for (int n = 0; n < nFiles; n++)
	{
		if (fInputFiles[n] != NULL) return kFALSE;
		if (fDataFrame[n].evId != -1) return kFALSE;
	}

	return kTRUE;
}
