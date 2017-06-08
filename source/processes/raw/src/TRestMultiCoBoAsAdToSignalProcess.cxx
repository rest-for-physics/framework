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

ClassImp(TRestMultiCoBoAsAdToSignalProcess)
	//______________________________________________________________________________
TRestMultiCoBoAsAdToSignalProcess::TRestMultiCoBoAsAdToSignalProcess()
{
	Initialize();
}

TRestMultiCoBoAsAdToSignalProcess::TRestMultiCoBoAsAdToSignalProcess(char *cfgFileName):TRestRawToSignalProcess(cfgFileName)
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

	//this->SetVerboseLevel(REST_Debug);

}

//______________________________________________________________________________
void TRestMultiCoBoAsAdToSignalProcess::InitProcess()
{

}

Bool_t TRestMultiCoBoAsAdToSignalProcess::OpenInputMultiCoBoAsAdBinFile ( std::vector <TString> fNames )
{
	if( fNames.size() == 0 ) return kFALSE;

	if( !InitializeStartTimeStampFromFilename( fNames[0] ) ) return kFALSE;

	fInputFileName = fNames[0];
	fStartTimeStamp.Print();

	nFiles = 0;

	for( unsigned int n = 0; n < fNames.size(); n++ )
	{
		FILE *f = fopen( fNames[n].Data(), "rb" );

		if( f == NULL )
		{
			cout << "WARNING. Input file does not exist" << endl;
			cout << "File : " << fNames[n] << endl;
			return kFALSE;
		}

		
		CoBoDataFrame newDataFrame;
		newDataFrame.timeStamp = 0;

		for( int m = 0; m < 272; m++ )
			newDataFrame.chHit[m] = kFALSE;

		for( int m = 0; m < 272; m++ )
			for( int l = 0; l < 512; l++ )
				newDataFrame.data[m][l] = 0;

		newDataFrame.evId = -1;
		newDataFrame.asadId = -1;

		fDataFrame.push_back( newDataFrame );

		fInputMultiBinFiles.push_back( f );
		nFiles++;
	}

	return kTRUE;

}
	
Bool_t TRestMultiCoBoAsAdToSignalProcess::InitializeStartTimeStampFromFilename( TString fName )
{
	//these parameters have to be extracted from the file name. So do not change the origin binary file name.
	int year,month,day,hour,minute,second,millisecond; 

	const Ssiz_t fnOffset = fName.Index(".graw");

	if( fName.Length() != fnOffset+5 || fnOffset < 28 ) 
	{
		cout << "Input binary file name type unknown!" << endl;
		return kFALSE;
	}

	if( fName[fnOffset-24]!='-' || fName[fnOffset-21]!='-' ||
	 fName[fnOffset-18]!='T' || fName[fnOffset-15]!=':' ||
	 fName[fnOffset-12]!=':' || fName[fnOffset-9]!='.' || fName[fnOffset-5]!='_' )    
	{
		cout<<"Input binary file name unknown!"<<endl;
		return kFALSE;
	}

	year= (int)(fName[fnOffset-28]-48)*1000 + (int)(fName[fnOffset-27]-48)*100 
		+ (int)(fName[fnOffset-26]-48)*10 + (int)(fName[fnOffset-25]-48)*1;
	month= (int)(fName[fnOffset-23]-48)*10 + (int)(fName[fnOffset-22]-48)*1;
	day= (int)(fName[fnOffset-20]-48)*10 + (int)(fName[fnOffset-19]-48)*1;
	hour= (int)(fName[fnOffset-17]-48)*10 + (int)(fName[fnOffset-16]-48)*1;
	minute= (int)(fName[fnOffset-14]-48)*10 + (int)(fName[fnOffset-13]-48)*1;
	second= (int)(fName[fnOffset-11]-48)*10 + (int)(fName[fnOffset-10]-48)*1;
	millisecond= (int)(fName[fnOffset-8]-48)*100 + (int)(fName[fnOffset-7]-48)*10 + (int)(fName[fnOffset-6]-48)*1;

	fStartTimeStamp.Set( year, month, day, hour, minute, second, 
				millisecond * 1000000, kTRUE, -8*3600 );  // Offset for Beijing(local) time
	return kTRUE;
}

TRestEvent* TRestMultiCoBoAsAdToSignalProcess::ProcessEvent( TRestEvent *evInput )
{

	if( EndReading() ) { fOutputEvent = NULL; return NULL; }

	for( int n = 0; n < nFiles; n++ )
	{
		FillBuffer( n );
		if( GetVerboseLevel() >= REST_Debug )
		{
			cout << "------------------------------------" << endl;
			cout << "Event id : " << fDataFrame[n].evId << endl;
			cout << "AsAd id : " << fDataFrame[n].asadId << endl;
			cout << "Time stamp : " << fDataFrame[n].timeStamp << endl;

			for( int m = 0; m < 272; m++ )
			{
				if( fDataFrame[n].chHit[m] )
				{
					cout << "Ch hit : " << m << " Data : ";
					for( int l = 0; l < 512; l++ )
						cout << "\t" << fDataFrame[n].data[m][l];
					cout << endl;
				}
			}
			cout << "------------------------------------" << endl;
		}
	}

	if( GetVerboseLevel() >= REST_Debug )
		GetChar();

	Int_t nextId = GetLowestEventId();

	TTimeStamp tSt = 0;

	for( int n = 0; n < nFiles; n++ )
	{
		if( fDataFrame[n].evId == nextId )
		{
			for( int m = 0; m < 272; m++ )
			{
				if( fDataFrame[n].chHit[m] )
				{
					if( (Double_t) tSt == 0 ) tSt = fDataFrame[n].timeStamp;

					sgnl.Initialize();
					sgnl.SetSignalID( m + fDataFrame[n].asadId * 272 );

					for(int j = 0; j < 512; j++ ) 
						sgnl.AddPoint( (Short_t) fDataFrame[n].data[m][j] );
					
					fSignalEvent->AddSignal(sgnl);
				}
			}
			ClearBuffer(n);
		}
	}

	fSignalEvent->SetTimeStamp( tSt );
	fSignalEvent->SetID( nextId );
	fSignalEvent->SetRunOrigin( fRunOrigin );
	fSignalEvent->SetSubRunOrigin( 0 );

	if( fSignalEvent->GetNumberOfSignals( ) == 0 ) return NULL;

	return fSignalEvent;

}
//______________________________________________________________________________
/* {{{
TRestEvent* TRestMultiCoBoAsAdToSignalProcess::ProcessEvent( TRestEvent *evInput )
{

	UChar_t frameHeader[256];//256: size of header of the cobo data frame
	UChar_t frameDataP[2048];//for partial readout data frame
	UChar_t frameDataF[278528];//for full readout data frame
	unsigned int frameSize, frameType, revision, headerSize, itemSize, nItems, eventIdx, asadIdx, readOffset, status;
	Long64_t eventTime;
	unsigned int agetIdx, chanIdx, chanIdx0, chanIdx1, chanIdx2, chanIdx3, buckIdx, sample;

	TTimeStamp fEveTimeStamp;
	int fEveTimeSec;//relative time
	int fEveTimeNanoSec;
	const Long64_t NsPerSec=1000000000;
	int adcSample[272][512];
	bool chHit[272];
	unsigned int chTmp;
	unsigned int i;
	int j;

	TRestRawSignal sgnl;
	sgnl.SetSignalID(-1);

	if( fread(frameHeader,256,1,fInputBinFile)!=1 || feof(fInputBinFile))
	{
		fclose(fInputBinFile);
		fOutputEvent = NULL;
		cout<<"file end"<<endl;
		return NULL;
	}
	frameType=(unsigned int)frameHeader[5]*0x100 + (unsigned int)frameHeader[6];
	if(frameHeader[0]==0x08 && frameType==1)//frameType: 1:partial readout  2:full readout
	{

		frameSize=(unsigned int)frameHeader[1]*0x10000 + (unsigned int)frameHeader[2]*0x100 + (unsigned int)frameHeader[3];
		frameSize*=256;
		//cout<<"frameSize: "<<frameSize<<endl;

		frameType=(unsigned int)frameHeader[5]*0x100 + (unsigned int)frameHeader[6];
		//cout<<"frameType: "<<frameType<<endl;
		if(frameType!=1)
		{
			cout<<"unsupported frame type!"<<endl;
			return NULL;
		}

		revision=(unsigned int)frameHeader[7];
		//cout<<"revision: "<<revision<<endl;
		if(revision!=5)
		{
			cout<<"unsupported revision!"<<endl;
			return NULL;
		}

		headerSize=(unsigned int)frameHeader[8]*0x100 + (unsigned int)frameHeader[9];
		//cout<<"headerSize: "<<headerSize<<endl;
		if(headerSize!=1)
		{
			cout<<"unsupported header size!"<<endl;
			return NULL;
		}

		itemSize=(unsigned int)frameHeader[10]*0x100 + (unsigned int)frameHeader[11];
		//cout<<"itemSize: "<<itemSize<<endl;
		if(itemSize!=4)
		{
			cout<<"unsupported item size!"<<endl;
			return NULL;
		}

		nItems=(unsigned int)frameHeader[12]*0x1000000 + (unsigned int)frameHeader[13]*0x10000 + (unsigned int)frameHeader[14]*0x100 + (unsigned int)frameHeader[15]; 
		cout<<"nItems: "<<nItems<<endl;

		eventTime=(Long64_t)frameHeader[16]*0x10000000000 + (Long64_t)frameHeader[17]*0x100000000 + (Long64_t)frameHeader[18]*0x1000000 + (Long64_t)frameHeader[19]*0x10000 + (Long64_t)frameHeader[20]*0x100 + (Long64_t)frameHeader[21]; 
		eventTime*=10;//ns at 100MHz experiment clock
		cout<<"eventTime: "<<eventTime<<endl;

		eventIdx=(unsigned int)frameHeader[22]*0x1000000 + (unsigned int)frameHeader[23]*0x10000 + (unsigned int)frameHeader[24]*0x100 + (unsigned int)frameHeader[25]; 
		cout<<"eventIdx: "<<eventIdx<<endl;

		asadIdx=(unsigned int)frameHeader[27];
		cout<<"asadIdx: "<<asadIdx<<endl;

		readOffset=(unsigned int)frameHeader[28]*0x100 + (unsigned int)frameHeader[29];
		cout<<"readOffset: "<<readOffset<<endl;
		if(readOffset!=0)
		{
			cout<<"unsupported readOffset!"<<endl;
			return NULL;
		}

		status=(unsigned int)frameHeader[30];
		if(status)
		{
			cout<<"bad frame!"<<endl;
			return NULL;
		}
		cout<<"status: "<<status<<endl;
		getchar();

		//------------read frame data-----------
		if(frameSize>256 && nItems*4==frameSize-256)
		{

			for(i=0;i<272;i++)
			{
				chHit[i]=0;
				for(j=0;j<512;j++) adcSample[i][j]=0;
			}

			for(i=0;i*512<nItems;i++)
			{
				if( (fread(frameDataP,2048,1,fInputBinFile))!=1 || feof(fInputBinFile))
				{
					fclose(fInputBinFile);
					fOutputEvent = NULL;
					cout<<"file end 2"<<endl;
					return NULL;
				}
				for(j=0;j<2048;j+=4)
				{
					agetIdx=(frameDataP[j]>>6);
					chanIdx=((unsigned int)(frameDataP[j]&0x3f)*2 + (frameDataP[j+1]>>7));
					chTmp = agetIdx*68 + chanIdx;
					buckIdx=((unsigned int)(frameDataP[j+1]&0x7f)*4 + (frameDataP[j+2]>>6)); 
					sample=((unsigned int)(frameDataP[j+2]&0x0f)*0x100 + frameDataP[j+3]);

					if(buckIdx>=512 || chTmp>=272) {cout<<"buck or channel id error!"<<endl; return NULL;}

					chHit[chTmp]=1;
					adcSample[chTmp][buckIdx]=sample;
				}
			}
			for(i=0;i<272;i++)
			{
				if(chHit[i])
				{
					sgnl.Initialize();
					sgnl.SetSignalID(i+asadIdx*272);
					for(j=0;j<512;j++) sgnl.AddPoint( (Short_t) adcSample[i][j] );
					fSignalEvent->AddSignal(sgnl);
				}
			}

			fEveTimeSec=0;//relative time
			fEveTimeNanoSec=0;
			while(eventTime>=NsPerSec)
			{
				eventTime-=NsPerSec;
				fEveTimeSec++;
			}
			fEveTimeNanoSec=(int)eventTime;
			fEveTimeStamp.SetSec(fEveTimeSec);
			fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
			fEveTimeStamp.Add(fStartTimeStamp);

			fSignalEvent->SetTimeStamp(fEveTimeStamp);
			fSignalEvent->SetID(eventIdx);
			fSignalEvent->SetRunOrigin(fRunOrigin);
			fSignalEvent->SetSubRunOrigin(fSubRunOrigin);

		}

	}
	else if(frameHeader[0]==0x08 && frameType==2)
	{

		frameSize=(unsigned int)frameHeader[1]*0x10000 + (unsigned int)frameHeader[2]*0x100 + (unsigned int)frameHeader[3];
		frameSize*=256;
		//cout<<"frameSize: "<<frameSize<<endl;

		frameType=(unsigned int)frameHeader[5]*0x100 + (unsigned int)frameHeader[6];
		//cout<<"frameType: "<<frameType<<endl;
		if(frameType!=2)
		{
			cout<<"unsupported frame type!"<<endl;
			return NULL;
		}

		revision=(unsigned int)frameHeader[7];
		//cout<<"revision: "<<revision<<endl;
		if(revision!=5)
		{
			cout<<"unsupported revision!"<<endl;
			return NULL;
		}

		headerSize=(unsigned int)frameHeader[8]*0x100 + (unsigned int)frameHeader[9];
		//cout<<"headerSize: "<<headerSize<<endl;
		if(headerSize!=1)
		{
			cout<<"unsupported header size!"<<endl;
			return NULL;
		}

		itemSize=(unsigned int)frameHeader[10]*0x100 + (unsigned int)frameHeader[11];
		//cout<<"itemSize: "<<itemSize<<endl;
		if(itemSize!=2)
		{
			cout<<"unsupported item size!"<<endl;
			return NULL;
		}

		nItems=(unsigned int)frameHeader[12]*0x1000000 + (unsigned int)frameHeader[13]*0x10000 + (unsigned int)frameHeader[14]*0x100 + (unsigned int)frameHeader[15]; 
		if(nItems!=139264)
		{
			cout<<"unsupported nItems!"<<endl;
			return NULL;
		}
		//cout<<"nItems: "<<nItems<<endl;

		eventTime=(Long64_t)frameHeader[16]*0x10000000000 + (Long64_t)frameHeader[17]*0x100000000 + (Long64_t)frameHeader[18]*0x1000000 + (Long64_t)frameHeader[19]*0x10000 + (Long64_t)frameHeader[20]*0x100 + (Long64_t)frameHeader[21]; 
		eventTime*=10;//ns at 100MHz experiment clock
		//cout<<"eventTime: "<<eventTime<<endl;

		eventIdx=(unsigned int)frameHeader[22]*0x1000000 + (unsigned int)frameHeader[23]*0x10000 + (unsigned int)frameHeader[24]*0x100 + (unsigned int)frameHeader[25]; 
		//cout<<"eventIdx: "<<eventIdx<<endl;

		asadIdx=(unsigned int)frameHeader[27];
		//cout<<"asadIdx: "<<asadIdx<<endl;

		readOffset=(unsigned int)frameHeader[28]*0x100 + (unsigned int)frameHeader[29];
		//cout<<"readOffset: "<<readOffset<<endl;
		if(readOffset!=0)
		{
			cout<<"unsupported readOffset!"<<endl;
			return NULL;
		}

		status=(unsigned int)frameHeader[30];
		if(status)
		{
			cout<<"bad frame!"<<endl;
			return NULL;
		}
		//cout<<"status: "<<status<<endl;

		//------------read frame data-----------
		if(frameSize>256) //not needed
		{

			for(i=0;i<272;i++)
			{
				chHit[i]=0;
				for(j=0;j<512;j++) adcSample[i][j] = 0;
			}

			if( fread(frameDataF,2048,136,fInputBinFile)!=136 || feof(fInputBinFile))
			{
				fclose(fInputBinFile);
				fOutputEvent = NULL;
				cout<<"file end 2"<<endl;
				return NULL;
			}

			int tmpP;
			for(i=0;i<512;i++)
			{
				chanIdx0=0;
				chanIdx1=0;
				chanIdx2=0;
				chanIdx3=0;
				for(j=0;j<272;j++)
				{
					tmpP=(i*272+j)*2;
					agetIdx=(frameDataF[tmpP]>>6);
					sample=((unsigned int)(frameDataF[tmpP]&0x0f)*0x100 + frameDataF[tmpP+1]);

					if(agetIdx==0) 
					{
						chanIdx=chanIdx0;
						chanIdx0 ++;
					}
					else if(agetIdx==1) 
					{
						chanIdx=chanIdx1;
						chanIdx1 ++;
					}
					else if(agetIdx==2)
					{
						chanIdx=chanIdx2;
						chanIdx2 ++;
					}
					else 
					{
						chanIdx=chanIdx3;
						chanIdx3 ++;
					}

					//cout<<"agetIdx: "<<agetIdx<<" chanIdx: "<<chanIdx<<endl;
					if(chanIdx>67) {cout<<"buck or channel id error!"<<endl; return NULL;} 
					chTmp = agetIdx*68 + chanIdx;
					adcSample[chTmp][i]=sample;
				}
			}

			for(i=0;i<272;i++)
			{
				sgnl.Initialize();
				sgnl.SetSignalID(i+asadIdx*272);
				for(j=0;j<512;j++) sgnl.AddPoint( (Short_t) adcSample[i][j] );
				fSignalEvent->AddSignal(sgnl);
			}

			fEveTimeSec=0;//relative time
			fEveTimeNanoSec=0;
			while(eventTime>=NsPerSec)
			{
				eventTime-=NsPerSec;
				fEveTimeSec++;
			}
			fEveTimeNanoSec=(int)eventTime;
			fEveTimeStamp.SetSec(fEveTimeSec);
			fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
			fEveTimeStamp.Add(fStartTimeStamp);

			fSignalEvent->SetTimeStamp(fEveTimeStamp);
			fSignalEvent->SetID(eventIdx);
			fSignalEvent->SetRunOrigin(fRunOrigin);
			fSignalEvent->SetSubRunOrigin(fSubRunOrigin);

		}

	}
	else 
	{
		fclose(fInputBinFile);
		fOutputEvent = NULL;
		cout<<"file type error!"<<endl;
		return NULL;
	}

	if( fSignalEvent->GetNumberOfSignals()==0 ) return NULL;
	return fSignalEvent;
}
}}} */

Bool_t TRestMultiCoBoAsAdToSignalProcess::FillBuffer( Int_t n )
{
	if( fDataFrame[n].evId != -1 || fInputMultiBinFiles[n] == NULL ) return kFALSE;

	UChar_t frameHeader[256];//256: size of header of the cobo data frame
	UChar_t frameDataP[2048];//for partial readout data frame
	UChar_t frameDataF[278528];//for full readout data frame
	unsigned int frameSize, frameType, revision, headerSize, itemSize, nItems, eventIdx, asadIdx, readOffset, status;
	Long64_t eventTime;
	unsigned int agetIdx, chanIdx, chanIdx0, chanIdx1, chanIdx2, chanIdx3, buckIdx, sample;

	TTimeStamp fEveTimeStamp;
	int fEveTimeSec;//relative time
	int fEveTimeNanoSec;
	const Long64_t NsPerSec=1000000000;
	unsigned int chTmp;
	unsigned int i;
	int j;

	if( fread(frameHeader,256,1,fInputMultiBinFiles[n])!=1 || feof(fInputMultiBinFiles[n]))
	{
		fclose(fInputMultiBinFiles[n]);
		fInputMultiBinFiles[n] = NULL;
		return kFALSE;
	}
	frameType=(unsigned int)frameHeader[5]*0x100 + (unsigned int)frameHeader[6];
	if(frameHeader[0]==0x08 && frameType==1)//frameType: 1:partial readout  2:full readout
	{

		frameSize=(unsigned int)frameHeader[1]*0x10000 + (unsigned int)frameHeader[2]*0x100 + (unsigned int)frameHeader[3];
		frameSize*=256;
		//cout<<"frameSize: "<<frameSize<<endl;

		frameType=(unsigned int)frameHeader[5]*0x100 + (unsigned int)frameHeader[6];
		//cout<<"frameType: "<<frameType<<endl;
		if(frameType!=1)
		{
			cout<<"unsupported frame type!"<<endl;
			return NULL;
		}

		revision=(unsigned int)frameHeader[7];
		//cout<<"revision: "<<revision<<endl;
		if(revision!=5)
		{
			cout<<"unsupported revision!"<<endl;
			return NULL;
		}

		headerSize=(unsigned int)frameHeader[8]*0x100 + (unsigned int)frameHeader[9];
		//cout<<"headerSize: "<<headerSize<<endl;
		if(headerSize!=1)
		{
			cout<<"unsupported header size!"<<endl;
			return NULL;
		}

		itemSize=(unsigned int)frameHeader[10]*0x100 + (unsigned int)frameHeader[11];
		//cout<<"itemSize: "<<itemSize<<endl;
		if(itemSize!=4)
		{
			cout<<"unsupported item size!"<<endl;
			return NULL;
		}

		nItems=(unsigned int)frameHeader[12]*0x1000000 + (unsigned int)frameHeader[13]*0x10000 + (unsigned int)frameHeader[14]*0x100 + (unsigned int)frameHeader[15]; 

		eventTime=(Long64_t)frameHeader[16]*0x10000000000 + (Long64_t)frameHeader[17]*0x100000000 + (Long64_t)frameHeader[18]*0x1000000 + (Long64_t)frameHeader[19]*0x10000 + (Long64_t)frameHeader[20]*0x100 + (Long64_t)frameHeader[21]; 
		eventTime*=10;//ns at 100MHz experiment clock

		fEveTimeSec=0;//relative time
		fEveTimeNanoSec=0;
		while(eventTime>=NsPerSec)
		{
			eventTime-=NsPerSec;
			fEveTimeSec++;
		}
		fEveTimeNanoSec=(int)eventTime;
		fEveTimeStamp.SetSec(fEveTimeSec);
		fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
		fEveTimeStamp.Add(fStartTimeStamp);

		fDataFrame[n].timeStamp = fEveTimeStamp;

		eventIdx=(unsigned int)frameHeader[22]*0x1000000 + (unsigned int)frameHeader[23]*0x10000 + (unsigned int)frameHeader[24]*0x100 + (unsigned int)frameHeader[25]; 
		fDataFrame[n].evId = eventIdx;


		asadIdx=(unsigned int)frameHeader[27];
		fDataFrame[n].asadId = asadIdx;

		readOffset=(unsigned int)frameHeader[28]*0x100 + (unsigned int)frameHeader[29];
		if(readOffset!=0)
		{
			cout<<"unsupported readOffset!"<<endl;
			return NULL;
		}

		status=(unsigned int)frameHeader[30];
		if(status)
		{
			cout<<"bad frame!"<<endl;
			return NULL;
		}

		//------------read frame data-----------
		if(frameSize>256 && nItems*4==frameSize-256)
		{

			for(i=0;i*512<nItems;i++)
			{
				if( (fread(frameDataP,2048,1,fInputMultiBinFiles[n]))!=1 || feof(fInputMultiBinFiles[n]))
				{
					fclose(fInputMultiBinFiles[n]);
					fInputMultiBinFiles[n] = NULL;
					return kFALSE;
				}
				for(j=0;j<2048;j+=4)
				{
					agetIdx=(frameDataP[j]>>6);
					chanIdx=((unsigned int)(frameDataP[j]&0x3f)*2 + (frameDataP[j+1]>>7));
					chTmp = agetIdx*68 + chanIdx;
					buckIdx=((unsigned int)(frameDataP[j+1]&0x7f)*4 + (frameDataP[j+2]>>6)); 
					sample=((unsigned int)(frameDataP[j+2]&0x0f)*0x100 + frameDataP[j+3]);

					if(buckIdx>=512 || chTmp>=272) {cout<<"buck or channel id error!"<<endl; return NULL;}

					fDataFrame[n].chHit[chTmp] = kTRUE;
					fDataFrame[n].data[chTmp][buckIdx] = sample;
				}
			}
		}

	}
	else if(frameHeader[0]==0x08 && frameType==2)
	{

		frameSize=(unsigned int)frameHeader[1]*0x10000 + (unsigned int)frameHeader[2]*0x100 + (unsigned int)frameHeader[3];
		frameSize*=256;
		//cout<<"frameSize: "<<frameSize<<endl;

		frameType=(unsigned int)frameHeader[5]*0x100 + (unsigned int)frameHeader[6];
		//cout<<"frameType: "<<frameType<<endl;
		if(frameType!=2)
		{
			cout<<"unsupported frame type!"<<endl;
			return NULL;
		}

		revision=(unsigned int)frameHeader[7];
		//cout<<"revision: "<<revision<<endl;
		if(revision!=5)
		{
			cout<<"unsupported revision!"<<endl;
			return NULL;
		}

		headerSize=(unsigned int)frameHeader[8]*0x100 + (unsigned int)frameHeader[9];
		//cout<<"headerSize: "<<headerSize<<endl;
		if(headerSize!=1)
		{
			cout<<"unsupported header size!"<<endl;
			return NULL;
		}

		itemSize=(unsigned int)frameHeader[10]*0x100 + (unsigned int)frameHeader[11];
		//cout<<"itemSize: "<<itemSize<<endl;
		if(itemSize!=2)
		{
			cout<<"unsupported item size!"<<endl;
			return NULL;
		}

		nItems=(unsigned int)frameHeader[12]*0x1000000 + (unsigned int)frameHeader[13]*0x10000 + (unsigned int)frameHeader[14]*0x100 + (unsigned int)frameHeader[15]; 
		if(nItems!=139264)
		{
			cout<<"unsupported nItems!"<<endl;
			return NULL;
		}
		//cout<<"nItems: "<<nItems<<endl;

		eventTime=(Long64_t)frameHeader[16]*0x10000000000 + (Long64_t)frameHeader[17]*0x100000000 + (Long64_t)frameHeader[18]*0x1000000 + (Long64_t)frameHeader[19]*0x10000 + (Long64_t)frameHeader[20]*0x100 + (Long64_t)frameHeader[21]; 
		eventTime*=10;//ns at 100MHz experiment clock

		fEveTimeSec=0;//relative time
		fEveTimeNanoSec=0;
		while(eventTime>=NsPerSec)
		{
			eventTime-=NsPerSec;
			fEveTimeSec++;
		}
		fEveTimeNanoSec=(int)eventTime;
		fEveTimeStamp.SetSec(fEveTimeSec);
		fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
		fEveTimeStamp.Add(fStartTimeStamp);

		fDataFrame[n].timeStamp = fEveTimeStamp;

		eventIdx=(unsigned int)frameHeader[22]*0x1000000 + (unsigned int)frameHeader[23]*0x10000 + (unsigned int)frameHeader[24]*0x100 + (unsigned int)frameHeader[25]; 
		fDataFrame[n].evId = eventIdx;

		asadIdx=(unsigned int)frameHeader[27];
		fDataFrame[n].asadId = asadIdx;

		readOffset=(unsigned int)frameHeader[28]*0x100 + (unsigned int)frameHeader[29];
		//cout<<"readOffset: "<<readOffset<<endl;
		if(readOffset!=0)
		{
			cout<<"unsupported readOffset!"<<endl;
			return NULL;
		}

		status=(unsigned int)frameHeader[30];
		if(status)
		{
			cout<<"bad frame!"<<endl;
			return NULL;
		}
		//cout<<"status: "<<status<<endl;

		//------------read frame data-----------
		if(frameSize>256) //not needed
		{

			if( fread(frameDataF,2048,136,fInputMultiBinFiles[n])!=136 || feof(fInputMultiBinFiles[n]))
			{
				fclose(fInputMultiBinFiles[n]);
				fInputMultiBinFiles[n] = NULL;
				return kFALSE;
			}

			int tmpP;
			for(i=0;i<512;i++)
			{
				chanIdx0=0;
				chanIdx1=0;
				chanIdx2=0;
				chanIdx3=0;
				for(j=0;j<272;j++)
				{
					tmpP=(i*272+j)*2;
					agetIdx=(frameDataF[tmpP]>>6);
					sample=((unsigned int)(frameDataF[tmpP]&0x0f)*0x100 + frameDataF[tmpP+1]);

					if(agetIdx==0) 
					{
						chanIdx=chanIdx0;
						chanIdx0 ++;
					}
					else if(agetIdx==1) 
					{
						chanIdx=chanIdx1;
						chanIdx1 ++;
					}
					else if(agetIdx==2)
					{
						chanIdx=chanIdx2;
						chanIdx2 ++;
					}
					else 
					{
						chanIdx=chanIdx3;
						chanIdx3 ++;
					}

					//cout<<"agetIdx: "<<agetIdx<<" chanIdx: "<<chanIdx<<endl;
					if(chanIdx>67) {cout<<"buck or channel id error!"<<endl; return NULL;} 
					chTmp = agetIdx*68 + chanIdx;
					fDataFrame[n].data[chTmp][i] = sample;
				}
			}


			fEveTimeSec=0;//relative time
			fEveTimeNanoSec=0;
			while(eventTime>=NsPerSec)
			{
				eventTime-=NsPerSec;
				fEveTimeSec++;
			}
			fEveTimeNanoSec=(int)eventTime;
			fEveTimeStamp.SetSec(fEveTimeSec);
			fEveTimeStamp.SetNanoSec(fEveTimeNanoSec);
			fEveTimeStamp.Add(fStartTimeStamp);

			fDataFrame[n].timeStamp = fEveTimeStamp;
			fDataFrame[n].evId = eventIdx;
			fDataFrame[n].asadId = asadIdx;
		}

	}
	else 
	{
		fclose( fInputMultiBinFiles[n] );
		fInputMultiBinFiles[n] = NULL;
		return kFALSE;
	}

	return kTRUE;
}

void TRestMultiCoBoAsAdToSignalProcess::ClearBuffer( Int_t n )
{
	fDataFrame[n].evId = -1;
	for( int m = 0; m < 272; m++ )
		fDataFrame[n].chHit[m] = kFALSE;
}

Int_t TRestMultiCoBoAsAdToSignalProcess::GetLowestEventId(  )
{
	Int_t evid = fDataFrame[0].evId;

	for( unsigned int m = 1; m < fDataFrame.size(); m++ )
	{
		if( evid == -1 ) evid = fDataFrame[m].evId;
		if( fDataFrame[m].evId != -1 && fDataFrame[m].evId < evid ) evid = fDataFrame[m].evId;
	}

	return evid;
}

Bool_t TRestMultiCoBoAsAdToSignalProcess::EndReading( )
{
	for( int n = 0; n < nFiles; n++ )
	{
		if( fInputMultiBinFiles[n] != NULL ) return kFALSE;

		if( fDataFrame[n].evId != -1 ) return kFALSE;
	}

	return kTRUE;
}

