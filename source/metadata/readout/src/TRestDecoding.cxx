///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDecoding inherited from TRestMetadata
///
///             Load metadata of the decoding and return a readout channel for a given DAQ channel.
///
///             now 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________


#include "TRestDecoding.h"
using namespace std;

ClassImp(TRestDecoding)
    //______________________________________________________________________________
TRestDecoding::TRestDecoding()
{
    Initialize();
}

TRestDecoding::TRestDecoding( const char *cfgFileName) : TRestMetadata (cfgFileName)
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

void TRestDecoding::Initialize()
{
    SetName( "decoding" );
    fReadoutChannelID.clear();
    fDaqChannelID.clear();
}

//______________________________________________________________________________
TRestDecoding::~TRestDecoding()
{
    cout << "Deleting TRestDecoding" << endl;
}


//______________________________________________________________________________
void TRestDecoding::InitFromConfigFile()
{
    fNChannels = StringToInteger(GetParameter("nChannels"));

    cout << "Number of channels " << fNChannels << endl;

    string decodingFilename = GetParameter( "fileName" );

    if( !fileExists( decodingFilename.c_str() ) ) 
    {
        cout << "REST WARNING. File " << decodingFilename << " does not exist" << endl; 
        cout << "Loading basic decoding" << endl;
        LoadDefaultDecoding( );
    }
    else
    {
        cout << "File Name " << fName.Data() << endl;
        if( !LoadDecodingFromFile( decodingFilename ) ) LoadDefaultDecoding(); 
    }
}

void TRestDecoding::LoadDefaultConfig( )
{
    fNChannels = 256;

    LoadDefaultDecoding( );
}

void TRestDecoding::PrintMetadata( )
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "Decoding : " << GetTitle() << endl;
    cout << "N Channels : " << fNChannels << endl;
    cout << "====================================" << endl;
    cout << "Readout\tDaq"<<endl;
    for(unsigned int i = 0; i < fReadoutChannelID.size(); i++ )
        cout << fReadoutChannelID[i] << "\t" << fDaqChannelID[i] << endl;
    cout << endl;

}

Int_t TRestDecoding::GetReadoutChannel( Int_t daqChannel )
{
    for( unsigned int i = 0; i < fDaqChannelID.size(); i++ )
        if( fDaqChannelID[i] == daqChannel) return fReadoutChannelID[i];

    cout << "Daq channel " << daqChannel << " not found" << endl;
    return -1;
}

Int_t TRestDecoding::GetDaqChannel( Int_t readoutChannel )
{
    for( unsigned int i = 0; i < fReadoutChannelID.size(); i++ )
        if( fReadoutChannelID[i] == readoutChannel) return fDaqChannelID[i];

    cout << "Daq channel " << readoutChannel << " not found" << endl;
    return -1;
}


//Default decoding daq and sim channels are the same
void TRestDecoding::LoadDefaultDecoding( )
{
    for( int c = 0; c < fNChannels; c++ ) 
       AddChannel(c,c);  
}

Bool_t TRestDecoding::LoadDecodingFromFile(TString fName)
{
    FILE *f;
    if( ( f = fopen(fName.Data(),"r") ) == NULL )
    {
        cout << "File " << fName.Data() << " not found!!!" << endl;
        return kFALSE;
    }

    cout << "Opening " << fName.Data() << endl;

    int readoutChannel, physChannel;

    int nChan = 0;

    while( !feof( f ) )
    {
        fscanf(f,"%d\t%d\n",&physChannel,&readoutChannel);
        AddChannel(readoutChannel,physChannel);
        nChan++;
    }
    fclose(f);

    if( nChan != fNChannels )
    {
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout<<"WARNING file and config has different numbers of channels "<<endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    }
    return kTRUE;
}


