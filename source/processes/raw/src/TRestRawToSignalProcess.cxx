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
using namespace std;
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
  
}


//______________________________________________________________________________
TRestRawToSignalProcess::~TRestRawToSignalProcess()
{
   // TRestRawToSignalProcess destructor
} 

void TRestRawToSignalProcess::LoadConfig( string cfgFilename, string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) == -1 ) { cout << "Loading default" << endl; LoadDefaultConfig( ); }
}

//______________________________________________________________________________
void TRestRawToSignalProcess::Initialize()
{
    SetSectionName( this->ClassName() );
    fSignalEvent = new TRestSignalEvent( );

    fInputEvent = NULL;
    fOutputEvent = fSignalEvent;
    fInputBinFile = NULL;

    fMinPoints = 512;

    fRejectNoise = false;

    fIsExternal = true;
}

void TRestRawToSignalProcess::BeginOfEventProcess() 
{
   // cout << "Begin of event process" << endl;
    fSignalEvent->Initialize();
}

void TRestRawToSignalProcess::InitFromConfigFile(){

   if( GetParameter( "rejectNoise", "OFF" ) == "ON" ) 
   {
	cout << "RAWToSignalProcess : Activating noise rejection" << endl;
	fRejectNoise = true;
   }
fElectronicsType = GetParameter("electronics");
 fMinPoints = StringToInteger( GetParameter("minPoints", "512" ) );
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
fMinPoints = 512;

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
Bool_t TRestRawToSignalProcess::OpenInputBinFile ( TString fName )
{
	TRestDetectorSetup *det = (TRestDetectorSetup *) this->GetDetectorSetup();

	if( det != NULL )
	{
		fRunOrigin = det->GetRunNumber();
		fSubRunOrigin = det->GetSubRunNumber();
	}
	else
	{
		cout << "REST WARNING : Detector setup has not been defined. Run and subRunNumber will not be defined!" << endl;

	}

	if(fInputBinFile!= NULL)fclose(fInputBinFile);

	if( (fInputBinFile = fopen(fName.Data(),"rb") )==NULL ) {
		cout << "WARNING. Input file does not exist" << endl;
		return kFALSE;
	}

    struct tm* clock;
    struct stat st;
    stat ( fName.Data(), &st);

    clock = gmtime( &( st.st_mtime ) );

    time_t tstamp = mktime ( clock );

    tStart = (Double_t ) tstamp;
    

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

