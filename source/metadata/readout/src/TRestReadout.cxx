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

    for( int p = 0; p < this->GetNumberOfReadoutPlanes(); p++ )
        for( int m = 0; m < this->GetReadoutPlane(p)->GetNumberOfModules(); m++ )
            this->GetReadoutPlane(p)->GetReadoutModule(m)->DoReadoutMapping();
}

void TRestReadout::Initialize()
{
    SetName("readout");

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


//______________________________________________________________________________
void TRestReadout::InitFromConfigFile()
{

    size_t position = 0;
    string planeString;

    while( ( planeString = GetKEYStructure( "readoutPlane", position ) ) != "NotFound" )
    {
        TRestReadoutPlane plane;

        string planeDefinition = GetKEYDefinition( "readoutPlane", planeString );
        plane.SetPosition( Get3DVectorFieldValueWithUnits( "position", planeDefinition ) );
        plane.SetCathodePosition( Get3DVectorFieldValueWithUnits( "cathodePosition", planeDefinition ) );
        plane.SetPlaneVector( StringTo3DVector( GetFieldValue( "planeVector", planeDefinition ) ) );
        plane.SetChargeCollection( StringToDouble( GetFieldValue( "chargeCollection", planeDefinition ) ) );

        string moduleString;
        while( ( moduleString = GetKEYStructure( "readoutModule", position ) ) != "NotFound" )
        {
            TRestReadoutModule module;

            string moduleDefinition = GetKEYDefinition( "readoutModule", moduleString );
            module.SetModuleID( StringToInteger( GetFieldValue( "id", moduleDefinition ) ) );
            module.SetOrigin( StringTo2DVector( GetFieldValue( "origin", moduleDefinition ) ) );
            module.SetSize( StringTo2DVector( GetFieldValue( "size", moduleDefinition ) ) );
            module.SetRotation( StringToDouble( GetFieldValue( "rotation", moduleDefinition ) ) );

            if( debug )
            {
                cout << "------module-----------------" << endl;
                cout << moduleString << endl;
                cout << "---------------------------" << endl;
                cout << "position : " << position << endl;
                getchar();
            }

            string channelString;
            size_t position2 = 0;
            while( ( channelString = GetKEYStructure( "readoutChannel", position2, moduleString ) ) != "" )
            {

                size_t position3 = 0;
                string channelDefinition = GetKEYDefinition( "readoutChannel", position3, channelString );

                TRestReadoutChannel channel;
                channel.SetID( StringToInteger( GetFieldValue( "id", channelDefinition ) ) );

                if( debug )
                {
                    cout << "-----channel-----------------" << endl;
                    cout << channelString << endl;
                    cout << "---------------------------" << endl;
                    getchar();
                }

                string pixelString;
                while( ( pixelString = GetKEYDefinition( "addPixel", position3, channelString ) ) != "" )
                {
                    TRestReadoutPixel pixel;

                    pixel.SetID( StringToInteger( GetFieldValue( "id", pixelString ) ) );
                    pixel.SetOrigin( StringTo2DVector( GetFieldValue( "origin", pixelString ) ) );
                    pixel.SetSize( StringTo2DVector( GetFieldValue( "size", pixelString ) ) );
                    pixel.SetRotation( StringToDouble( GetFieldValue( "rotation", pixelString ) ) );

                    //               cout << "Rotation : " << StringToDouble( GetFieldValue( "rotation", pixelString ) ) << endl;
                    //               cout << "Rotation : " << pixel.GetRotation() << endl;

                    channel.AddPixel( pixel );

                    if(  debug )
                    {
                        cout << "pixel" << endl;
                        cout << pixelString << endl;
                    }
                }

                module.AddChannel( channel );

                position2++;

            }

            plane.AddModule( module );

            position++;
        }
    }

    this->Print();
}


Double_t TRestReadout::GetX( Int_t plane, Int_t modID, Int_t chID )
{
    return GetReadoutPlane( plane )->GetX( modID, chID );
}

Double_t TRestReadout::GetY( Int_t plane, Int_t modID, Int_t chID )
{
    return GetReadoutPlane( plane )->GetY( modID, chID );
}

void TRestReadout::Print( )
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "Readout : " << GetTitle() << endl;
    cout << "Number of readout planes : " << fNReadoutPlanes << endl;
    cout << "====================================" << endl;
    cout << endl;
    for( int p = 0; p < GetNumberOfReadoutPlanes(); p++ )
            this->GetReadoutPlane( p )->Print();
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

