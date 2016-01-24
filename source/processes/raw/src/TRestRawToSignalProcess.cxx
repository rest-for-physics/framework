///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawToSignalProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestRawToSignalProcess
///             How to use: replace TRestRawToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestRawToSignalProcess.h"
#include "TTimeStamp.h"

ClassImp(TRestRawToSignalProcess)
//______________________________________________________________________________
TRestRawToSignalProcess::TRestRawToSignalProcess()
{
  Initialize();
}

TRestRawToSignalProcess::TRestRawToSignalProcess(char *cfgFileName)
{
 Initialize();
 
 if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
 PrintMetadata();  
  
}


//______________________________________________________________________________
TRestRawToSignalProcess::~TRestRawToSignalProcess()
{
   // TRestRawToSignalProcess destructor
} 

void TRestRawToSignalProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) ) LoadDefaultConfig( );
    PrintMetadata();  
}

//______________________________________________________________________________
void TRestRawToSignalProcess::Initialize()
{
    SetName( "daq" );
    fSignalEvent = new TRestSignalEvent( );

    fRunNumber=-1;
    fRunIndex=-1;

    fInputEvent = NULL;
    fOutputEvent = fSignalEvent;
    fInputBinFile = NULL;
}

void TRestRawToSignalProcess::BeginOfEventProcess() 
{
   // cout << "Begin of event process" << endl;
    fSignalEvent->Initialize();
}

void TRestRawToSignalProcess::InitFromConfigFile(){

fElectronicsType = GetParameter("electronics");
  if(fElectronicsType==""){
  cout<<"electronic type not found "<<endl;
  LoadDefaultConfig();
  }

  if(fElectronicsType=="AFTER"||fElectronicsType=="AGET")return;
  LoadDefaultConfig();
  

}


void TRestRawToSignalProcess::LoadDefaultConfig(){

cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
cout<<"WARNING "<<endl;
cout<<"Error Loading config file "<<endl;
cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;

cout<<"Press a key to continue..."<<endl;

getchar();

fElectronicsType = "AGET";

}


//______________________________________________________________________________

void TRestRawToSignalProcess::EndOfEventProcess() 
{

//cout << __PRETTY_FUNCTION__ << endl;

}


//______________________________________________________________________________
void TRestRawToSignalProcess::EndProcess()
{

//close binary file??? Already done

 cout << __PRETTY_FUNCTION__ << endl;
 
}
//______________________________________________________________________________
Bool_t TRestRawToSignalProcess::OpenInputBinFile ( TString fName ){

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
void  TRestRawToSignalProcess::printBits(unsigned short num)
{
   for(unsigned short bit=0;bit<(sizeof(unsigned short) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   
   printf("\n");
}

//For debugging
void  TRestRawToSignalProcess::printBits(unsigned int num)
{
   for(unsigned int bit=0;bit<(sizeof(unsigned int) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   
   printf("\n");
}

void TRestRawToSignalProcess::PrintMetadata(){
 cout << endl;
    cout << "====================================" << endl;
    cout << "DAQ : " << GetTitle() << endl;
    cout << "Electronics type : " << fElectronicsType.Data() << endl;
    cout << "====================================" << endl;
    
    cout << endl;

}


