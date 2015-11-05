///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAGETToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestAGETToSignalProcess
///             How to use: replace TRestAGETToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestAGETToSignalProcess.h"
#include "TTimeStamp.h"

ClassImp(TRestAGETToSignalProcess)
//______________________________________________________________________________
TRestAGETToSignalProcess::TRestAGETToSignalProcess()
{
  Initialize();
}

TRestAGETToSignalProcess::TRestAGETToSignalProcess(char *cfgFileName)
{
 Initialize();
}


//______________________________________________________________________________
TRestAGETToSignalProcess::~TRestAGETToSignalProcess()
{
   // TRestAGETToSignalProcess destructor
}

//______________________________________________________________________________
void TRestAGETToSignalProcess::Initialize()
{
   fSignalEvent = new TRestSignalEvent( );
   
   fRunNumber=-1;
   fRunIndex=-1;
   
   fInputEvent = NULL;
   fOutputEvent = fSignalEvent;
   fInputBinFile = NULL;
}

void TRestAGETToSignalProcess::BeginOfEventProcess() 
{
   // cout << "Begin of event process" << endl;
    fSignalEvent->Initialize();
}

void TRestAGETToSignalProcess::InitFromConfigFile(){

 cout << __PRETTY_FUNCTION__ << endl;

}


void TRestAGETToSignalProcess::LoadDefaultConfig(){

 cout << __PRETTY_FUNCTION__ << endl;
}



//______________________________________________________________________________
void TRestAGETToSignalProcess::InitProcess()
{

//Binary file header

  //The binary starts here
  char runUid[26],initTime[25];
  fread(runUid, 1, 26, fInputBinFile);
  
  sprintf(initTime,"%s",runUid);
  printf("File UID is %s \n",initTime); 
  totalBytesReaded = sizeof(runUid);
  
  int year,day,month,hour,minute,second;
  sscanf(initTime,"%*c%*cR%d_%02d_%02d-%02d_%02d_%02d-%*d",&year,&month,&day,&hour,&minute,&second);
  printf("R%d_%02d_%02d-%02d_%02d_%02d\n",year,month,day,hour,minute,second);
  TTimeStamp tS (year,month,day,hour,minute,second);  
  tStart = tS.AsDouble();
  cout<<tStart<<endl;
  //Timestamp of the run
  
  //NULL word
  unsigned short nullword;
  fread(&nullword, sizeof(nullword),1,fInputBinFile);
  if(this->GetVerboseLevel()==REST_Debug){
  cout<<"NULL word "<<nullword<<endl;
  printBits(nullword);
  }
  frameBits=0;
  totalBytesReaded+=sizeof(nullword);
  
  //Start of Data frame
  unsigned short startDF;
  fread(&startDF, sizeof(startDF),1,fInputBinFile);
  if(this->GetVerboseLevel()==REST_Debug){
  cout<<"PFX_START "<<startDF<<endl;
  printBits(startDF);
  cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
  cout<<" FEC "<<(startDF & 0x1F)<<endl;
  }
  frameBits+=sizeof(startDF);
  totalBytesReaded+=sizeof(startDF);
  
  //Payload from start data frame to end of data frame (including both)
  unsigned short payload;
  fread(&payload, sizeof(payload),1,fInputBinFile);
  if(this->GetVerboseLevel()==REST_Debug){
  cout<<"Frame payload "<<payload<<endl;
  printBits(payload);
  }
  frameBits+=sizeof(payload);
  totalBytesReaded+=sizeof(payload);

  
}

//______________________________________________________________________________
TRestEvent* TRestAGETToSignalProcess::ProcessEvent( TRestEvent *evInput )
{

  //Now we start reading the events

  unsigned short startEv;
     
     //Event header
     if(fread(&startEv, sizeof(startEv),1,fInputBinFile) !=1){
     fclose(fInputBinFile);
	cout<<"File END"<<endl;
	return NULL;//File end
     }
     if(this->GetVerboseLevel()==REST_Debug){
     cout<<"Start of Event "<<startEv<<endl;
     printBits(startEv);
     cout<<" Event type "<<(startEv & 0xF)<<endl;
     }
     frameBits+=sizeof(startEv);
     totalBytesReaded+=sizeof(startEv);
     
     //TimeStamp of the event 48 bit word
     unsigned short tsh,tsm,tsl;
     fread(&tsh, sizeof(tsh),1,fInputBinFile);
     fread(&tsm, sizeof(tsm),1,fInputBinFile);
     fread(&tsl, sizeof(tsl),1,fInputBinFile);
     if(this->GetVerboseLevel()==REST_Debug){
     printBits(tsh);
     printBits(tsm);
     printBits(tsl);
     cout<<" TimeStamp "<<2147483648*tsl+32768*tsm+tsh<<endl;
     }

     frameBits+=sizeof(tsh)*3;
     totalBytesReaded+=sizeof(tsh)*3;
     
     //Event ID 32 bit word
     unsigned int evID;
     fread(&evID, sizeof(evID),1,fInputBinFile);
     if(this->GetVerboseLevel()==REST_Debug){
     printBits(evID);
     cout<<"Event ID "<<evID<<endl;
     }
     frameBits+=sizeof(evID);
     totalBytesReaded+=sizeof(evID);
     
     //Timestamp number of clocks since the start of the run (1 clock ~20 ns)
     double timestamp = (double) (2147483648*tsl+32768*tsm+tsh);

//Set timestamp and event ID
fSignalEvent->SetEventTime(tStart+timestamp*2.E-8);
fSignalEvent->SetEventID(evID);

int timeBin = 0;
	
    int fecN;
    int channel;
    int asicN;
    
    int adc;
        
int physChannel;
int chan;

bool skip=false;

unsigned short dat, startDF;;

//Bucle till it finds the data end
   while (((dat & 0xFFF0)>>4) !=14) {
   
  	if(fread(&dat, sizeof(dat), 1, fInputBinFile) !=1){
        fclose(fInputBinFile);
	cout<<"File END 2"<<endl;
	return NULL;//File end
     	}
	frameBits+=sizeof(dat);
	totalBytesReaded +=sizeof(dat);
			
	//New Event
	if((dat & 0xC000) >> 14 == 3 ){
	
	//Storing previous event, skipping if is the first one
	
	fecN = (dat & 0x3E00) >> 9;
	asicN = (dat & 0x180) >> 7;
	channel = (dat & 0x7F);
	
	timeBin =0;
	if(this->GetVerboseLevel()==REST_Debug)cout<<"Reset"<<endl;
	
    	//AGET Short Seq
	physChannel=-10;
	if (channel> 1 && channel < 13 ) {
      	physChannel= channel -2; 
    	} else if (channel> 13 && channel < 24 ) {
      physChannel= channel -3; 
    	} else if (channel> 24 && channel < 47 ) {
      physChannel= channel -4; 
    	} else if (channel> 47 && channel < 58 ) {
      	physChannel= channel -5; 
    	} else if (channel> 58 ) {
      	physChannel= channel -6; 
    	}
    	
    	chan = physChannel;
		
		//Skipping non physical channels
		if(physChannel<0){
		skip=true;
				
		if(this->GetVerboseLevel()==REST_Debug){cout<<"Skipping channel "<<channel<<endl;getchar();}
		
		cout<<"Skipping channel "<<channel<<endl;
		
		}
		else skip=false;
		
	//FECN not included so far....
	physChannel = asicN*72 + chan;
	//physChannel = fecN*4*72+asicN*72 + chan;
	
	if(this->GetVerboseLevel()==REST_Debug)
	{
	cout<<"----------------------"<<endl;
	cout<<"FEC "<<fecN<<endl;
	cout<<"asic "<<asicN<<endl;
	cout<<"Channel "<<chan<<endl;
	cout<<"PhysChannel "<<physChannel<<endl;
	}
	
			
	}
	//Timebin, may be not present if zero-suppresion is not active
	else if((dat & 0xFE00) >> 9 == 7 ){
	timeBin = (dat & 0x1FF);
	if(this->GetVerboseLevel()==REST_Debug)cout<<"TimeBin "<<timeBin<<endl;
	}
	//ADC values
	else if((dat & 0xF000) >> 12 == 3 ){
		
	adc = (dat & 0xFFF);
	
	if(this->GetVerboseLevel()==REST_Debug)cout<<"Time bin "<<timeBin<<"\tADC "<<adc<<endl;
	if(!skip)fSignalEvent->AddChargeToSignal( physChannel, timeBin, adc );
	timeBin++;
	}
	//End of Frame, reading frame header and payload
	else if(dat==15){
	if(this->GetVerboseLevel()==REST_Debug)cout<<" End of frame "<<"Bits readed "<<frameBits<<" /"<<payload<<endl;
	frameBits=0;
	
	fread(&startDF, sizeof(startDF),1,fInputBinFile);
	if(this->GetVerboseLevel()==REST_Debug){
	cout<<"PFX_START "<<startDF<<endl;
	printBits(startDF);
	cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
	cout<<" FEC "<<(startDF & 0x1F)<<endl;
	}
	frameBits+=sizeof(startDF);
	totalBytesReaded+=sizeof(startDF);
	
	fread(&payload, sizeof(payload),1,fInputBinFile);
	if(this->GetVerboseLevel()==REST_Debug){
	cout<<"Frame payload "<<payload<<endl;
	printBits(payload);
	}
	frameBits+=sizeof(payload);
	totalBytesReaded+=sizeof(payload);
	}
  
   }//while
   
//Storing last event 
   
if(this->GetVerboseLevel()==REST_Debug)cout<<" End of event "<< dat<<endl;
//End of event footer
fread(&dat, sizeof(dat),1,fInputBinFile);
frameBits+=sizeof(dat);
totalBytesReaded+=sizeof(dat);

//If data frame is being finish, the data frame header is reader in order to avoid a misalignement reading the file
if(payload<=frameBits+2){
	while( dat !=15 ){
	fread(&dat, sizeof(dat),1,fInputBinFile);
	frameBits+=sizeof(dat);
	totalBytesReaded+=sizeof(dat);
	}
	
	if(this->GetVerboseLevel()==REST_Debug)cout<<" End of frame "<<"Bits readed "<<frameBits<<" /"<<payload<<endl;
	
	frameBits=0;
	fread(&startDF, sizeof(startDF),1,fInputBinFile);
	if(this->GetVerboseLevel()==REST_Debug){
	printBits(startDF);
	cout<<"PFX_START "<<startDF<<endl;
	cout<<" Version "<< ((startDF & 0x1E0) >> 5) <<endl;
	cout<<" FEC "<<(startDF & 0x1F)<<endl;
	}
	frameBits+=sizeof(startDF);
	totalBytesReaded+=sizeof(startDF);
	
	fread(&payload, sizeof(payload),1,fInputBinFile);
	frameBits+=sizeof(payload);
	totalBytesReaded+=sizeof(payload);
	if(this->GetVerboseLevel()==REST_Debug){
	cout<<"Frame payload "<<payload<<endl;
	printBits(payload);
	}
	
}

//cout<<"Ev ID "<<fSignalEvent->GetEventID()<<" "<< <<endl;

return fSignalEvent;
}

void TRestAGETToSignalProcess::EndOfEventProcess() 
{

//cout << __PRETTY_FUNCTION__ << endl;

}


//______________________________________________________________________________
void TRestAGETToSignalProcess::EndProcess()
{

//close binary file??? Already done

 cout << __PRETTY_FUNCTION__ << endl;
 
}
//______________________________________________________________________________
Bool_t TRestAGETToSignalProcess::OpenInputBinFile ( TString fName ){

if(fInputBinFile!= NULL)fclose(fInputBinFile);

 if( (fInputBinFile = fopen(fName.Data(),"rb") )==NULL ) {
        cout << "WARNING. Input file does not exist" << endl;
        return kFALSE;
    }

cout<<"File "<<fName.Data()<<" opened"<<endl;

int size=fName.Sizeof();cout<<size<<endl;
TString fN(fName(size-20,size-1));
cout<<fN.Data()<<endl;
sscanf(fN.Data(),"RUN_%d.%d.acq",&fRunNumber,&fRunIndex);

cout<<"Run# "<<fRunNumber<<" index "<<fRunIndex<<endl;

return kTRUE;
}

//For debugging
void  TRestAGETToSignalProcess::printBits(unsigned short num)
{
   for(unsigned short bit=0;bit<(sizeof(unsigned short) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   
   printf("\n");
}

//For debugging
void  TRestAGETToSignalProcess::printBits(unsigned int num)
{
   for(unsigned short bit=0;bit<(sizeof(unsigned int) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   
   printf("\n");
}





