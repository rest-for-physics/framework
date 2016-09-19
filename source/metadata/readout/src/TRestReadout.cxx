///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadout.cxx
///
///             aug 2015    Javier Galan
///_______________________________________________________________________________


#include "TRestReadout.h"
using namespace std;

const int debug = 0;

ClassImp(TRestReadout)
    //______________________________________________________________________________
TRestReadout::TRestReadout()
{
    Initialize();

}

TRestReadout::TRestReadout( const char *cfgFileName) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfigFromFile( fConfigFileName );

    /*
    for( int p = 0; p < this->GetNumberOfReadoutPlanes(); p++ )
        for( int m = 0; m < this->GetReadoutPlane(p)->GetNumberOfModules(); m++ )
        {
            cout << "Mapping plane : " << p << " Module : " << m << endl;
            this->GetReadoutPlane(p)->GetReadoutModule(m)->DoReadoutMapping( fMappingNodes );
        }
        */
}

TRestReadout::TRestReadout( const char *cfgFileName, string name) : TRestMetadata (cfgFileName)
{
    Initialize();

    LoadConfigFromFile( fConfigFileName, name );

    /*
    for( int p = 0; p < this->GetNumberOfReadoutPlanes(); p++ )
        for( int m = 0; m < this->GetReadoutPlane(p)->GetNumberOfModules(); m++ )
        {
            this->GetReadoutPlane(p)->GetReadoutModule(m)->DoReadoutMapping( fMappingNodes );
        }
        */
}

void TRestReadout::Initialize()
{
    SetName("readout");

    fDecoding = false;

    fNReadoutPlanes = 0;
    fReadoutPlanes.clear();
}


//______________________________________________________________________________
TRestReadout::~TRestReadout()
{
    cout << "Deleting TRestReadout" << endl;
}

Int_t TRestReadout::GetNumberOfReadoutPlanes( ) 
{ 
    return fReadoutPlanes.size(); 
}

Int_t TRestReadout::GetNumberOfModules( )
{
    Int_t modules = 0;
    for( int p = 0; p < GetNumberOfReadoutPlanes( ); p++ )
        modules += GetReadoutPlane( p )->GetNumberOfModules( );
    return modules;
}

Int_t TRestReadout::GetNumberOfChannels( )
{
    Int_t channels = 0;
    for( int p = 0; p < GetNumberOfReadoutPlanes( ); p++ )
        for( int m = 0; m < GetReadoutPlane( p )->GetNumberOfModules( ); m++ )
            channels += GetReadoutPlane( p )->GetModule( m )->GetNumberOfChannels( );
    return channels;
}

Int_t TRestReadout::GetModuleDefinitionId( TString name )
{
    for( unsigned int i = 0; i < fModuleDefinitions.size(); i++ )
        if( fModuleDefinitions[i].GetName() == name )
            return i;
    return -1;
}


//______________________________________________________________________________
void TRestReadout::InitFromConfigFile()
{

    size_t position = 0;
    string planeString;

    fMappingNodes = StringToInteger( GetParameter( "mappingNodes", "0" ) );

    string moduleString;
    size_t posSection = 0;
    while( ( moduleString = GetKEYStructure( "readoutModule", posSection ) ) != "NotFound" )
    {
        TRestReadoutModule module;

        string moduleDefinition = GetKEYDefinition( "readoutModule", moduleString );

        module.SetName( GetFieldValue( "name", moduleDefinition ) );
        module.SetSize( StringTo2DVector( GetFieldValue( "size", moduleDefinition ) ) );
        module.SetTolerance( StringToDouble( GetFieldValue( "tolerance", moduleDefinition ) ) );

        if( debug )
        {
            cout << "------module-----------------" << endl;
            cout << moduleString << endl;
            cout << "---------------------------" << endl;
            cout << "position : " << posSection << endl;
            GetChar();
        }

        string channelString;
        size_t position2 = 0;
        while( ( channelString = GetKEYStructure( "readoutChannel", position2, moduleString ) ) != "" )
        {
            size_t position3 = 0;
            string channelDefinition = GetKEYDefinition( "readoutChannel", position3, channelString );

            TRestReadoutChannel channel;

            Int_t id = StringToInteger( GetFieldValue( "id", channelDefinition ) );
            channel.SetID( id );
            channel.SetDaqID( -1 );

            string pixelString;
            while( ( pixelString = GetKEYDefinition( "addPixel", position3, channelString ) ) != "" )
            {
                TRestReadoutPixel pixel;

                pixel.SetID( StringToInteger( GetFieldValue( "id", pixelString ) ) );
                pixel.SetOrigin( StringTo2DVector( GetFieldValue( "origin", pixelString ) ) );
                pixel.SetSize( StringTo2DVector( GetFieldValue( "size", pixelString ) ) );
                pixel.SetRotation( StringToDouble( GetFieldValue( "rotation", pixelString ) ) );

                channel.AddPixel( pixel );
            }

            module.AddChannel( channel );

            position2++;
        }

        module.DoReadoutMapping( fMappingNodes );

        fModuleDefinitions.push_back( module );

        posSection++;
    }

    Int_t addedChannels = 0;
    while( ( planeString = GetKEYStructure( "readoutPlane", position ) ) != "NotFound" )
    {
        TRestReadoutPlane plane;

        string planeDefinition = GetKEYDefinition( "readoutPlane", planeString );
        plane.SetID( GetNumberOfReadoutPlanes() );
        plane.SetPosition( Get3DVectorFieldValueWithUnits( "position", planeDefinition ) );
        plane.SetCathodePosition( Get3DVectorFieldValueWithUnits( "cathodePosition", planeDefinition ) );
        plane.SetPlaneVector( StringTo3DVector( GetFieldValue( "planeVector", planeDefinition ) ) );
        plane.SetChargeCollection( StringToDouble( GetFieldValue( "chargeCollection", planeDefinition ) ) );

        Double_t tDriftDistance = plane.GetDistanceTo( plane.GetCathodePosition() );
        plane.SetTotalDriftDistance( tDriftDistance );

        string moduleDefinition;
        size_t posPlane = 0;
        while( ( moduleDefinition = GetKEYDefinition( "addReadoutModule", posPlane, planeString ) ) != "" )
        {
            TString modName = GetFieldValue( "name", moduleDefinition );
            Int_t mid = GetModuleDefinitionId( modName );

            fModuleDefinitions[mid].SetModuleID( StringToInteger( GetFieldValue( "id", moduleDefinition ) ) );
            fModuleDefinitions[mid].SetOrigin( StringTo2DVector( GetFieldValue( "origin", moduleDefinition ) ) );
            fModuleDefinitions[mid].SetRotation( StringToDouble( GetFieldValue( "rotation", moduleDefinition ) ) );

            Int_t firstDaqChannel = StringToInteger( GetFieldValue( "firstDaqChannel", moduleDefinition ) );
            if( firstDaqChannel == -1 ) firstDaqChannel = addedChannels;

            string decodingFile = GetFieldValue( "decodingFile", moduleDefinition );
            if( decodingFile == "Not defined" || decodingFile == "" ) fDecoding = false;
            else fDecoding = true;

            if( fDecoding && !fileExists( decodingFile.c_str() ) )
            { 
                cout << "REST ERROR : The decoding file does not exist" << endl;
                cout << "--------------------------------" << endl;
                cout << "File : " << decodingFile << endl;
                cout << "Default decoding will be used. readoutChannel=daqChannel" << endl;
                cout << "To avoid this message and use the default decoding define : decodingFile=\"\"" << endl;
                cout << "--------------------------------" << endl;
                cout << "Press a KEY to continue..." << endl;
                getchar();
                fDecoding = false;
            }

            vector <Int_t> rChannel;
            vector <Int_t> dChannel;
            if( fDecoding && fileExists( decodingFile.c_str() ) )
            {
                FILE *f = fopen( decodingFile.c_str(), "r" );

                Int_t daq, readout;
                while( !feof( f ) )
                {
                    if( fscanf(f,"%d\t%d\n", &daq, &readout ) <= 0 )
                    {
                        cout << "REST Error!!. TRestReadout::InitFromConfigFile. Contact rest-dev@cern.ch" << endl;
                        exit(-1);
                    }
                    rChannel.push_back( readout );
                    dChannel.push_back( daq + firstDaqChannel );
                }
                fclose(f);
            }

            if( debug )
            {
                cout << "------module-----------------" << endl;
                cout << moduleString << endl;
                cout << "---------------------------" << endl;
                cout << "position : " << posPlane << endl;
                getchar();
            }

            if( fDecoding && (unsigned int) fModuleDefinitions[mid].GetNumberOfChannels() != rChannel.size() )
            {
                cout << "REST ERROR : TRestReadout."
                     << " The number of channels defined in the readout is not the same" 
                     << " as the number of channels found in the decoding." << endl;
                exit(1);
            }

            for( int ch = 0; ch < fModuleDefinitions[mid].GetNumberOfChannels(); ch++ )
            {
                cout << "Channel : " << ch << endl;

                if( !fDecoding )
                {
                    Int_t id = fModuleDefinitions[mid].GetChannel(ch)->GetID( );
                    rChannel.push_back( id );
                    dChannel.push_back( id + firstDaqChannel );
                }

                    fModuleDefinitions[mid].GetChannel(ch)->SetID( rChannel[ch] );
                    fModuleDefinitions[mid].GetChannel(ch)->SetDaqID( dChannel[ch] );

                addedChannels++;

            }
            fModuleDefinitions[mid].SetMinMaxDaqIDs();
            plane.AddModule( fModuleDefinitions[mid] );

            posPlane++;
        }

        this->AddReadoutPlane( plane );

        position++;
    }

    ValidateReadout( );
}

void TRestReadout::ValidateReadout( )
{
    cout << "--------------------------------------------------" << endl;
    cout << "TRestReadout::ValidateReadout:: NOT IMPLEMENTED" << endl;
    cout << "This function should crosscheck that there are no repeated DaqChannels IDs" << endl;
    cout << "No dead area in the readout module" << endl;
    cout << "And other checks" << endl;
    cout << "--------------------------------------------------" << endl;


}


Double_t TRestReadout::GetX( Int_t plane, Int_t modID, Int_t chID )
{
    return GetReadoutPlane( plane )->GetX( modID, chID );
}

Double_t TRestReadout::GetY( Int_t plane, Int_t modID, Int_t chID )
{
    return GetReadoutPlane( plane )->GetY( modID, chID );
}

void TRestReadout::PrintMetadata( Int_t fullDetail )
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "Readout : " << GetTitle() << endl;
    cout << "Number of readout planes : " << fNReadoutPlanes << endl;
    cout << "Decoding was defined : ";
    if( fDecoding ) cout << "YES" << endl; else cout << "NO" << endl;
    cout << "====================================" << endl;
    cout << endl;
    for( int p = 0; p < GetNumberOfReadoutPlanes(); p++ )
            this->GetReadoutPlane( p )->Print( fullDetail );
    cout << "====================================" << endl;

}

void TRestReadout::Draw()
{
    cout << "NOT IMPLEMENTED" << endl;
    /*
       for( int modID = 0; modID < GetNumberOfModules(); modID++ )
       {
       TRestReadoutModule md = GetReadoutModule( modID );

       Double_t x[5], y[5];
       x[0] = md.GetModuleOriginX();
       y[0] = md.GetModuleOriginY();

       x[1] = x[0] + md.GetModuleSizeX();
       y[1] = y[0];

       x[2] = x[1];
       y[2] = y[1] + md.GetModuleSizeY();

       x[3] = x[2] - md.GetModuleSizeX();
       y[3] = y[2];

       x[4] = x[3];
       y[4] = y[3] - md.GetModuleSizeY();


       TGraph modGraph( 5, x, y );

       drawingElement.push_back( modGraph );
       }

       for( unsigned int i = 0; i < drawingElement.size(); i++ )
       {
       drawingElement[i].Draw("same");
       }
       */
}

